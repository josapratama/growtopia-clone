/**
 * 2D Sandbox MMO — Authoritative Game Server & WebSocket Netcode
 */

const express = require("express");
const http = require("http");
const path = require("path");
const { WebSocketServer, WebSocket } = require("ws");

const { MSG_TYPES, TILE_SIZE } = require("./engine/protocol");
const { Tilemap } = require("./engine/tilemap");
const { Player } = require("./engine/player");

const PORT = process.env.PORT || 8085;
const TICK_RATE = 20; // 20 ticks per second (50ms)
const TICK_DT = 1.0 / TICK_RATE;

const app = express();
const server = http.createServer(app);
const wss = new WebSocketServer({ server });

// Serve static web client
const publicDir = path.join(__dirname, "../public");
app.use(express.static(publicDir));

// Game State
const tilemap = new Tilemap(120, 60);
const players = new Map();       // socket -> Player
const playerSockets = new Map(); // playerId -> socket
const pendingInputs = new Map(); // playerId -> Array of inputs
let serverTick = 0;

wss.on("connection", (ws) => {
  const playerId = `p_${Date.now().toString(36)}_${Math.random().toString(36).substring(2, 5)}`;
  let player = null;

  ws.on("message", (raw) => {
    try {
      const msg = JSON.parse(raw.toString());
      handleClientMessage(ws, playerId, msg);
    } catch (e) {
      console.error("Malformed packet:", e);
    }
  });

  ws.on("close", () => {
    if (player) {
      players.delete(ws);
      playerSockets.delete(playerId);
      pendingInputs.delete(playerId);
      broadcast({
        type: "PLAYER_LEAVE",
        id: playerId
      });
      console.log(`[NETCODE] Player ${player.name} (${playerId}) disconnected.`);
    }
  });
});

function handleClientMessage(ws, playerId, msg) {
  let player = players.get(ws);

  switch (msg.type) {
    case MSG_TYPES.JOIN: {
      // Find safe surface spawn position
      const spawnTileX = 30 + Math.floor(Math.random() * 20);
      let spawnTileY = 10;
      while (spawnTileY < tilemap.height && !tilemap.isSolid(spawnTileX, spawnTileY)) {
        spawnTileY++;
      }
      const spawnX = spawnTileX * TILE_SIZE;
      const spawnY = (spawnTileY - 2) * TILE_SIZE;

      player = new Player(playerId, msg.name, spawnX, spawnY);
      players.set(ws, player);
      playerSockets.set(playerId, ws);
      pendingInputs.set(playerId, []);

      // 1. Send Init World packet
      send(ws, {
        type: MSG_TYPES.INIT_WORLD,
        selfId: playerId,
        width: tilemap.width,
        height: tilemap.height,
        tileSize: TILE_SIZE,
        tiles: Array.from(tilemap.tiles),
        player: player.serialize(true),
        serverTick
      });

      console.log(`[NETCODE] Player ${player.name} joined world at (${spawnX}, ${spawnY}).`);
      break;
    }

    case MSG_TYPES.INPUT: {
      if (!player) return;
      // Buffer input for authoritative tick processing
      const queue = pendingInputs.get(playerId);
      if (queue && queue.length < 30) {
        queue.push(msg.input);
      }
      break;
    }

    case MSG_TYPES.TILE_INTERACT: {
      if (!player) return;
      const tx = msg.x;
      const ty = msg.y;

      // Distance validation (anti-reach check: max 5 tiles ~160px)
      const dist = Math.hypot((tx * TILE_SIZE + 16) - player.x, (ty * TILE_SIZE + 16) - (player.y + player.height / 2));
      if (dist > 180) return;

      if (msg.action === "BREAK") {
        tilemap.hitBlock(tx, ty, 1);
      } else if (msg.action === "PLACE") {
        const itemType = msg.tileType;
        if (player.inventory[itemType] && player.inventory[itemType] > 0) {
          const placed = tilemap.placeBlock(tx, ty, itemType);
          if (placed) {
            player.inventory[itemType]--;
          }
        }
      }
      break;
    }

    case MSG_TYPES.CHAT: {
      if (!player) return;
      broadcast({
        type: MSG_TYPES.CHAT,
        id: playerId,
        name: player.name,
        text: String(msg.text).substring(0, 100)
      });
      break;
    }

    case MSG_TYPES.PING: {
      send(ws, {
        type: MSG_TYPES.PONG,
        clientTime: msg.clientTime,
        serverTime: Date.now()
      });
      break;
    }
  }
}

function send(ws, msg) {
  if (ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify(msg));
  }
}

function broadcast(msg) {
  const json = JSON.stringify(msg);
  for (const ws of players.keys()) {
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(json);
    }
  }
}

// ─── Fixed Authoritative Server Game Loop (20 Hz) ───
setInterval(() => {
  serverTick++;

  // 1. Process client inputs & update physics
  for (const [ws, player] of players.entries()) {
    const queue = pendingInputs.get(player.id) || [];
    
    // Process all pending inputs from client
    while (queue.length > 0) {
      const input = queue.shift();
      player.applyInput(input, tilemap, TICK_DT);
    }

    // Check item pickup collision
    for (let i = tilemap.itemDrops.length - 1; i >= 0; i--) {
      const drop = tilemap.itemDrops[i];
      const dist = Math.hypot(drop.x - player.x, drop.y - (player.y + player.height / 2));
      if (dist < 26) {
        // Collect drop!
        player.inventory[drop.type] = (player.inventory[drop.type] || 0) + 1;
        tilemap.itemDrops.splice(i, 1);

        send(ws, {
          type: MSG_TYPES.ITEM_PICKUP,
          dropId: drop.id,
          itemType: drop.type,
          inventory: player.inventory
        });
      }
    }
  }

  // 2. Step item drops physics
  tilemap.updateDrops(TICK_DT);

  // 3. Broadcast Snapshot to each client
  const dirtyTiles = tilemap.drainDirtyTiles();
  const serializedPlayers = Array.from(players.values()).map(p => p.serialize(false));

  for (const [ws, player] of players.entries()) {
    send(ws, {
      type: MSG_TYPES.SNAPSHOT,
      tick: serverTick,
      lastAckSeq: player.lastProcessedSeq,
      self: player.serialize(true),
      players: serializedPlayers,
      dirtyTiles: dirtyTiles.length > 0 ? dirtyTiles : undefined,
      drops: tilemap.itemDrops
    });
  }
}, 1000 / TICK_RATE);

server.listen(PORT, () => {
  console.log(`[SANDBOX MMO] Server listening on http://localhost:${PORT}`);
  console.log(`[SANDBOX MMO] Authoritative Tick Rate: ${TICK_RATE} Hz (${TICK_DT * 1000}ms)`);
});

