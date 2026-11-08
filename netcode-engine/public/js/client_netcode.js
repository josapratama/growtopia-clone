/**
 * 2D Sandbox MMO — Client Netcode (Prediction, Reconciliation & Entity Interpolation)
 */

class ClientNetcode {
  constructor(game) {
    this.game = game;
    this.ws = null;
    this.connected = false;
    this.selfId = null;

    // Client-Side Prediction State
    this.sequenceNumber = 0;
    this.pendingInputs = []; // [{ seq, left, right, jump, dt }]
    this.reconciliationsCount = 0;
    this.lastDeviations = 0;

    // Entity Interpolation Buffer for Remote Players
    this.snapshotBuffer = []; // [{ timestamp, players }]
    this.interpolationDelayMs = 100; // 100ms render buffer

    // Latency & RTT Tracking
    this.pingMs = 0;
    this.artificialLagMs = 0;
    this.artificialDropRate = 0; // 0 to 1
  }

  connect(name = "Player") {
    const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
    const url = `${protocol}//${window.location.host}`;
    this.ws = new WebSocket(url);

    this.ws.onopen = () => {
      this.connected = true;
      this.send({ type: "JOIN", name });
      this.startPingLoop();
    };

    this.ws.onmessage = (event) => {
      // Simulate artificial packet loss
      if (this.artificialDropRate > 0 && Math.random() < this.artificialDropRate) {
        return; // drop packet
      }

      // Simulate artificial lag
      if (this.artificialLagMs > 0) {
        setTimeout(() => this.handleMessage(JSON.parse(event.data)), this.artificialLagMs);
      } else {
        this.handleMessage(JSON.parse(event.data));
      }
    };

    this.ws.onclose = () => {
      this.connected = false;
      setTimeout(() => this.connect(name), 2000);
    };
  }

  send(msg) {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      if (this.artificialLagMs > 0) {
        setTimeout(() => {
          if (this.ws.readyState === WebSocket.OPEN) this.ws.send(JSON.stringify(msg));
        }, this.artificialLagMs);
      } else {
        this.ws.send(JSON.stringify(msg));
      }
    }
  }

  handleMessage(msg) {
    switch (msg.type) {
      case "INIT_WORLD": {
        this.selfId = msg.selfId;
        this.game.initWorld(msg);
        break;
      }

      case "SNAPSHOT": {
        this.handleSnapshot(msg);
        break;
      }

      case "ITEM_PICKUP": {
        this.game.handlePickup(msg);
        break;
      }

      case "CHAT": {
        this.game.handleChat(msg);
        break;
      }

      case "PONG": {
        this.pingMs = Date.now() - msg.clientTime;
        break;
      }
    }
  }

  startPingLoop() {
    setInterval(() => {
      if (this.connected) {
        this.send({ type: "PING", clientTime: Date.now() });
      }
    }, 1000);
  }

  /**
   * Process local input with Client-Side Prediction (CSP).
   * Runs immediately on the client frame.
   */
  predictMovement(input, dt) {
    this.sequenceNumber++;
    const fullInput = {
      seq: this.sequenceNumber,
      left: input.left,
      right: input.right,
      jump: input.jump,
      dt
    };

    // Store in unacknowledged input buffer
    this.pendingInputs.push(fullInput);

    // Apply movement locally immediately (0-lag feel)
    const localPlayer = this.game.localPlayer;
    if (localPlayer) {
      localPlayer.applyInput(fullInput, this.game.tilemap, dt);
    }

    // Send input packet to server
    this.send({ type: "INPUT", input: fullInput });
  }

  /**
   * Server Reconciliation:
   * When server snapshot arrives, check if client predicted position matches server.
   * If there's an error/drift, snap to server position and replay remaining inputs.
   */
  handleSnapshot(snapshot) {
    const localPlayer = this.game.localPlayer;
    if (!localPlayer) return;

    // 1. Process dirty tiles & drops
    if (snapshot.dirtyTiles) {
      for (const t of snapshot.dirtyTiles) {
        this.game.tilemap.setTile(t.x, t.y, t.type);
      }
    }
    if (snapshot.drops) {
      this.game.drops = snapshot.drops;
    }

    // 2. Authoritative Server Reconciliation for Local Player
    if (snapshot.self) {
      const serverState = snapshot.self;
      const lastAck = snapshot.lastAckSeq;

      // Discard all inputs up to lastAckSeq
      this.pendingInputs = this.pendingInputs.filter(inp => inp.seq > lastAck);

      // Save ghost position for debug visualization
      this.game.serverGhostPos = { x: serverState.x, y: serverState.y };

      // Calculate position deviation
      const distDev = Math.hypot(localPlayer.x - serverState.x, localPlayer.y - serverState.y);
      this.lastDeviations = distDev;

      // If deviation is significant (> 1.5px), reconcile!
      if (distDev > 1.5) {
        this.reconciliationsCount++;
        localPlayer.x = serverState.x;
        localPlayer.y = serverState.y;
        localPlayer.vx = serverState.vx;
        localPlayer.vy = serverState.vy;
        localPlayer.isGrounded = serverState.isGrounded;

        // Replay all unacknowledged inputs on top of authoritative server state
        for (const unackInput of this.pendingInputs) {
          localPlayer.applyInput(unackInput, this.game.tilemap, unackInput.dt);
        }
      }

      if (serverState.inventory) {
        this.game.inventory = serverState.inventory;
      }
    }

    // 3. Buffer snapshot for Entity Interpolation of Remote Players
    this.snapshotBuffer.push({
      timestamp: Date.now(),
      players: snapshot.players.filter(p => p.id !== this.selfId)
    });

    if (this.snapshotBuffer.length > 30) {
      this.snapshotBuffer.shift();
    }
  }

  /**
   * Interpolate Remote Players using snapshot buffer.
   * Renders other players with buttery-smooth 60 FPS motion.
   */
  getInterpolatedRemotePlayers() {
    const renderTime = Date.now() - this.interpolationDelayMs;

    // Find two snapshots flanking renderTime
    let prev = null;
    let next = null;

    for (let i = 0; i < this.snapshotBuffer.length - 1; i++) {
      if (this.snapshotBuffer[i].timestamp <= renderTime && renderTime <= this.snapshotBuffer[i + 1].timestamp) {
        prev = this.snapshotBuffer[i];
        next = this.snapshotBuffer[i + 1];
        break;
      }
    }

    if (!prev || !next) {
      // Fallback to latest snapshot
      const latest = this.snapshotBuffer[this.snapshotBuffer.length - 1];
      return latest ? latest.players : [];
    }

    const t0 = prev.timestamp;
    const t1 = next.timestamp;
    const alpha = (renderTime - t0) / Math.max(1, t1 - t0);

    const interpolated = [];
    for (const pNext of next.players) {
      const pPrev = prev.players.find(p => p.id === pNext.id);
      if (pPrev) {
        interpolated.push({
          id: pNext.id,
          name: pNext.name,
          color: pNext.color,
          facing: pNext.facing,
          x: pPrev.x + (pNext.x - pPrev.x) * alpha,
          y: pPrev.y + (pNext.y - pPrev.y) * alpha,
          isGrounded: pNext.isGrounded
        });
      } else {
        interpolated.push(pNext);
      }
    }

    return interpolated;
  }
}

window.ClientNetcode = ClientNetcode;

