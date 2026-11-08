/**
 * Main Client Game Orchestrator & Loop
 * Integrates Web Audio 8-bit Sound Effects, Netcode Diagnostics, and Input Loop.
 */

class SoundEffects {
  constructor() {
    this.ctx = null;
    this.enabled = true;
  }

  _init() {
    if (!this.ctx && typeof AudioContext !== 'undefined') {
      const AudioCtx = window.AudioContext || window.webkitAudioContext;
      this.ctx = new AudioCtx();
    }
    if (this.ctx && this.ctx.state === 'suspended') {
      this.ctx.resume();
    }
  }

  playJump() {
    if (!this.enabled) return;
    this._init();
    if (!this.ctx) return;

    const osc = this.ctx.createOscillator();
    const gain = this.ctx.createGain();
    const now = this.ctx.currentTime;

    osc.type = 'square';
    osc.frequency.setValueAtTime(140, now);
    osc.frequency.exponentialRampToValueAtTime(320, now + 0.12);

    gain.gain.setValueAtTime(0.08, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + 0.12);

    osc.connect(gain);
    gain.connect(this.ctx.destination);
    osc.start(now);
    osc.stop(now + 0.13);
  }

  playMineHit() {
    if (!this.enabled) return;
    this._init();
    if (!this.ctx) return;

    const osc = this.ctx.createOscillator();
    const gain = this.ctx.createGain();
    const now = this.ctx.currentTime;

    osc.type = 'triangle';
    osc.frequency.setValueAtTime(90, now);
    osc.frequency.exponentialRampToValueAtTime(45, now + 0.08);

    gain.gain.setValueAtTime(0.12, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + 0.08);

    osc.connect(gain);
    gain.connect(this.ctx.destination);
    osc.start(now);
    osc.stop(now + 0.09);
  }

  playBlockPlace() {
    if (!this.enabled) return;
    this._init();
    if (!this.ctx) return;

    const osc = this.ctx.createOscillator();
    const gain = this.ctx.createGain();
    const now = this.ctx.currentTime;

    osc.type = 'sine';
    osc.frequency.setValueAtTime(260, now);
    osc.frequency.exponentialRampToValueAtTime(520, now + 0.07);

    gain.gain.setValueAtTime(0.1, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + 0.07);

    osc.connect(gain);
    gain.connect(this.ctx.destination);
    osc.start(now);
    osc.stop(now + 0.08);
  }

  playItemPickup() {
    if (!this.enabled) return;
    this._init();
    if (!this.ctx) return;

    const now = this.ctx.currentTime;
    [392, 523, 659].forEach((freq, i) => {
      const osc = this.ctx.createOscillator();
      const gain = this.ctx.createGain();
      const noteTime = now + i * 0.05;

      osc.type = 'square';
      osc.frequency.setValueAtTime(freq, noteTime);

      gain.gain.setValueAtTime(0.06, noteTime);
      gain.gain.exponentialRampToValueAtTime(0.001, noteTime + 0.08);

      osc.connect(gain);
      gain.connect(this.ctx.destination);
      osc.start(noteTime);
      osc.stop(noteTime + 0.09);
    });
  }
}

class GameApp {
  constructor() {
    this.canvas = document.getElementById('game-canvas');
    this.sfx = new SoundEffects();

    this.renderer = new CanvasRenderer(this.canvas);
    this.netcode = new ClientNetcode(this.renderer);
    this.input = new InputHandler(this.canvas, this.netcode, this.renderer);

    this.lastTime = performance.now();
    this.fps = 60;
    this.frameCount = 0;
    this.fpsTimer = 0;

    this.wasJumping = false;
    this._initUI();
    this._connect();
  }

  _initUI() {
    // Artificial Lag & Drop Sliders
    const lagSlider = document.getElementById('slider-lag');
    const lagVal = document.getElementById('val-lag');
    const dropSlider = document.getElementById('slider-drop');
    const dropVal = document.getElementById('val-drop');

    if (lagSlider) {
      lagSlider.addEventListener('input', (e) => {
        const val = parseInt(e.target.value, 10);
        if (lagVal) lagVal.textContent = `${val}ms`;
        const drop = dropSlider ? parseFloat(dropSlider.value) : 0;
        this.netcode.setSimulatedConditions(val, drop);
      });
    }

    if (dropSlider) {
      dropSlider.addEventListener('input', (e) => {
        const val = parseFloat(e.target.value);
        if (dropVal) dropVal.textContent = `${Math.round(val * 100)}%`;
        const lag = lagSlider ? parseInt(lagSlider.value, 10) : 0;
        this.netcode.setSimulatedConditions(lag, val);
      });
    }

    // Toggle Ghost Checkbox
    const chkGhost = document.getElementById('chk-ghost');
    if (chkGhost) {
      chkGhost.addEventListener('change', (e) => {
        this.renderer.showServerGhost = e.target.checked;
      });
    }

    // Toggle Sound Button
    const btnSound = document.getElementById('btn-sound');
    if (btnSound) {
      btnSound.addEventListener('click', () => {
        this.sfx.enabled = !this.sfx.enabled;
        btnSound.textContent = this.sfx.enabled ? '🔊 SFX On' : '🔇 SFX Off';
      });
    }

    // Hotbar click events
    document.querySelectorAll('.hotbar-slot').forEach((slot) => {
      slot.addEventListener('click', () => {
        const slotIdx = parseInt(slot.dataset.slot, 10);
        this.input.selectSlot(slotIdx);
      });
    });

    // Start with default slot 1
    this.input.selectSlot(1);
  }

  _connect() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${window.location.host}/ws`;

    const playerName = 'Crafter_' + Math.floor(100 + Math.random() * 900);
    this.netcode.connect(wsUrl, playerName);

    // Audio SFX hooks on netcode events
    window.addEventListener('tile_mined', () => this.sfx.playMineHit());
    window.addEventListener('tile_placed', () => this.sfx.playBlockPlace());
    window.addEventListener('item_pickup', () => this.sfx.playItemPickup());

    // Start Main Loop
    requestAnimationFrame((t) => this._loop(t));
  }

  _loop(timestamp) {
    const dt = Math.min(timestamp - this.lastTime, 100); // clamp max dt to 100ms
    this.lastTime = timestamp;

    // Calculate FPS
    this.frameCount++;
    this.fpsTimer += dt;
    if (this.fpsTimer >= 500) {
      this.fps = Math.round((this.frameCount * 1000) / this.fpsTimer);
      this.frameCount = 0;
      this.fpsTimer = 0;
    }

    // 1. Process Input
    this.input.update(dt);

    // Jump audio trigger on transition
    if (this.input.keys.jump && !this.wasJumping && this.netcode.localPlayer && this.netcode.localPlayer.isGrounded) {
      this.sfx.playJump();
    }
    this.wasJumping = this.input.keys.jump;

    // 2. Client Netcode Fixed Step / CSP
    this.netcode.processInput(this.input.keys, dt);

    // 3. Render Canvas
    this.renderer.render(
      this.netcode.localPlayer,
      this.netcode.remotePlayers,
      this.netcode.tilemap,
      this.netcode.floatingItems,
      this.netcode.serverGhost
    );

    // 4. Update HUD Diagnostics
    this._updateHUD();

    requestAnimationFrame((t) => this._loop(t));
  }

  _updateHUD() {
    const elFps = document.getElementById('stat-fps');
    const elPing = document.getElementById('stat-ping');
    const elRecon = document.getElementById('stat-recon');
    const elEntities = document.getElementById('stat-entities');
    const elCoords = document.getElementById('stat-coords');

    if (elFps) elFps.textContent = `${this.fps} FPS`;
    if (elPing) elPing.textContent = `${this.netcode.metrics.rtt} ms`;
    if (elRecon) elRecon.textContent = `${this.netcode.metrics.reconciliationCount}`;

    const totalEntities = (this.netcode.localPlayer ? 1 : 0) +
      this.netcode.remotePlayers.size +
      this.netcode.floatingItems.length;
    if (elEntities) elEntities.textContent = `${totalEntities}`;

    if (this.netcode.localPlayer && elCoords) {
      const tx = Math.floor(this.netcode.localPlayer.x / 16);
      const ty = Math.floor(this.netcode.localPlayer.y / 16);
      elCoords.textContent = `X: ${tx}, Y: ${ty}`;
    }
  }
}

// Bootstrap on DOM Loaded
window.addEventListener('DOMContentLoaded', () => {
  window.gameApp = new GameApp();
});

