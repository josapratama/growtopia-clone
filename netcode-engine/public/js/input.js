/**
 * Input Handler for 2D Sandbox MMO
 * Captures keyboard movement, mouse mining/building, hotbar selection, and chat input.
 */

class InputHandler {
  constructor(canvas, netcode, renderer) {
    this.canvas = canvas;
    this.netcode = netcode;
    this.renderer = renderer;

    this.keys = {
      left: false,
      right: false,
      jump: false,
    };

    this.mouse = {
      x: 0,
      y: 0,
      worldX: 0,
      worldY: 0,
      tileX: 0,
      tileY: 0,
      isDownLeft: false,
      isDownRight: false,
    };

    this.selectedSlot = 1; // 1 - 8
    this.selectedTileType = 1; // Default DIRT
    this.chatActive = false;
    this.interactCooldown = 0; // ms

    this._bindEvents();
  }

  _bindEvents() {
    // Keyboard key down
    window.addEventListener('keydown', (e) => {
      if (this.chatActive) {
        if (e.key === 'Enter') {
          this._submitChat();
        } else if (e.key === 'Escape') {
          this._cancelChat();
        }
        return;
      }

      if (e.key === 'Enter') {
        this._openChat();
        e.preventDefault();
        return;
      }

      switch (e.code) {
        case 'KeyA':
        case 'ArrowLeft':
          this.keys.left = true;
          break;
        case 'KeyD':
        case 'ArrowRight':
          this.keys.right = true;
          break;
        case 'KeyW':
        case 'Space':
        case 'ArrowUp':
          this.keys.jump = true;
          break;
        case 'Digit1':
        case 'Digit2':
        case 'Digit3':
        case 'Digit4':
        case 'Digit5':
        case 'Digit6':
        case 'Digit7':
        case 'Digit8':
          const slot = parseInt(e.key, 10);
          this.selectSlot(slot);
          break;
      }
    });

    // Keyboard key up
    window.addEventListener('keyup', (e) => {
      if (this.chatActive) return;

      switch (e.code) {
        case 'KeyA':
        case 'ArrowLeft':
          this.keys.left = false;
          break;
        case 'KeyD':
        case 'ArrowRight':
          this.keys.right = false;
          break;
        case 'KeyW':
        case 'Space':
        case 'ArrowUp':
          this.keys.jump = false;
          break;
      }
    });

    // Mouse Move
    this.canvas.addEventListener('mousemove', (e) => {
      const rect = this.canvas.getBoundingClientRect();
      this.mouse.x = e.clientX - rect.left;
      this.mouse.y = e.clientY - rect.top;

      if (this.renderer) {
        const world = this.renderer.screenToWorld(this.mouse.x, this.mouse.y);
        this.mouse.worldX = world.x;
        this.mouse.worldY = world.y;
        this.mouse.tileX = Math.floor(world.x / 16);
        this.mouse.tileY = Math.floor(world.y / 16);
      }
    });

    // Mouse Down (Left = Mine, Right = Place)
    this.canvas.addEventListener('mousedown', (e) => {
      if (this.chatActive) {
        this._cancelChat();
      }

      if (e.button === 0) {
        this.mouse.isDownLeft = true;
        this._tryInteract('mine');
      } else if (e.button === 2) {
        this.mouse.isDownRight = true;
        this._tryInteract('place');
      }
    });

    // Mouse Up
    window.addEventListener('mouseup', (e) => {
      if (e.button === 0) this.mouse.isDownLeft = false;
      if (e.button === 2) this.mouse.isDownRight = false;
    });

    // Prevent context menu on right click
    this.canvas.addEventListener('contextmenu', (e) => e.preventDefault());
  }

  selectSlot(slotNumber) {
    this.selectedSlot = Math.max(1, Math.min(8, slotNumber));
    // Tile mapping:
    // 1: DIRT (1), 2: STONE (2), 3: WOOD (3), 4: BRICK (4), 5: GLASS (5), 6: GOLD (6), 7: LAVA (7), 8: BEDROCK (9)
    const slotToType = {
      1: 1, // Dirt
      2: 2, // Stone
      3: 3, // Wood
      4: 4, // Brick
      5: 5, // Glass
      6: 6, // Gold
      7: 7, // Lava
      8: 9, // Bedrock
    };
    this.selectedTileType = slotToType[this.selectedSlot] || 1;

    // Update UI hotbar highlight
    const slots = document.querySelectorAll('.hotbar-slot');
    slots.forEach((s, idx) => {
      if (idx + 1 === this.selectedSlot) {
        s.classList.add('active');
      } else {
        s.classList.remove('active');
      }
    });
  }

  _tryInteract(action) {
    if (!this.netcode || !this.netcode.localPlayer) return;

    const player = this.netcode.localPlayer;
    const tileX = this.mouse.tileX;
    const tileY = this.mouse.tileY;

    // Reach limit: 6 tiles (approx 96 pixels)
    const playerCenterX = player.x + player.width / 2;
    const playerCenterY = player.y + player.height / 2;
    const targetCenterX = tileX * 16 + 8;
    const targetCenterY = tileY * 16 + 8;
    const dist = Math.hypot(targetCenterX - playerCenterX, targetCenterY - playerCenterY);

    if (dist > 120) return; // out of reach

    if (action === 'mine') {
      this.netcode.sendTileInteract(tileX, tileY, 'mine', 0);
    } else if (action === 'place') {
      this.netcode.sendTileInteract(tileX, tileY, 'place', this.selectedTileType);
    }
  }

  update(dt) {
    // Handle held mouse mining or placing
    if (this.mouse.isDownLeft) {
      this.interactCooldown -= dt;
      if (this.interactCooldown <= 0) {
        this._tryInteract('mine');
        this.interactCooldown = 180; // ms between consecutive hits
      }
    } else if (this.mouse.isDownRight) {
      this.interactCooldown -= dt;
      if (this.interactCooldown <= 0) {
        this._tryInteract('place');
        this.interactCooldown = 220; // ms between placements
      }
    } else {
      this.interactCooldown = 0;
    }
  }

  _openChat() {
    this.chatActive = true;
    const chatInput = document.getElementById('chat-input');
    if (chatInput) {
      chatInput.classList.remove('hidden');
      chatInput.value = '';
      chatInput.focus();
    }
    // Release movement keys while typing
    this.keys.left = false;
    this.keys.right = false;
    this.keys.jump = false;
  }

  _submitChat() {
    const chatInput = document.getElementById('chat-input');
    if (chatInput && this.netcode) {
      const msg = chatInput.value.trim();
      if (msg.length > 0) {
        this.netcode.sendChat(msg);
      }
      chatInput.value = '';
      chatInput.blur();
      chatInput.classList.add('hidden');
    }
    this.chatActive = false;
  }

  _cancelChat() {
    const chatInput = document.getElementById('chat-input');
    if (chatInput) {
      chatInput.value = '';
      chatInput.blur();
      chatInput.classList.add('hidden');
    }
    this.chatActive = false;
  }
}

if (typeof window !== 'undefined') {
  window.InputHandler = InputHandler;
}

