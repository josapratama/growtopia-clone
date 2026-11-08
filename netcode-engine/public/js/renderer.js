/**
 * 2D Sandbox MMO — HTML5 Canvas 2D Pixel Graphics Renderer
 */

class GameRenderer {
  constructor(canvasId) {
    this.canvas = document.getElementById(canvasId);
    this.ctx = this.canvas.getContext("2d");
    this.tileSize = 32;

    // Camera
    this.camX = 0;
    this.camY = 0;

    this.resize();
    window.addEventListener("resize", () => this.resize());
  }

  resize() {
    this.width = window.innerWidth;
    this.height = window.innerHeight;
    this.canvas.width = this.width;
    this.canvas.height = this.height;
    this.ctx.imageSmoothingEnabled = false; // crisp pixel art
  }

  render(game) {
    const ctx = this.ctx;
    const w = this.width;
    const h = this.height;

    // 1. Update Camera to smoothly follow local player
    if (game.localPlayer) {
      const targetCamX = game.localPlayer.x - w / 2;
      const targetCamY = game.localPlayer.y - h / 2;
      this.camX += (targetCamX - this.camX) * 0.15;
      this.camY += (targetCamY - this.camY) * 0.15;
    }

    ctx.save();
    ctx.clearRect(0, 0, w, h);

    // Sky Background Gradient
    const skyGrad = ctx.createLinearGradient(0, 0, 0, h);
    skyGrad.addColorStop(0, "#0f172a");
    skyGrad.addColorStop(1, "#1e293b");
    ctx.fillStyle = skyGrad;
    ctx.fillRect(0, 0, w, h);

    // Apply Camera Transform
    ctx.translate(-Math.round(this.camX), -Math.round(this.camY));

    // 2. Render Visible Tilemap Chunks
    this.renderTilemap(ctx, game.tilemap);

    // 3. Render Item Drops
    this.renderDrops(ctx, game.drops);

    // 4. Render Server Ghost (for Netcode debugging)
    if (game.showNetcodeGhost && game.serverGhostPos) {
      this.renderGhostPlayer(ctx, game.serverGhostPos);
    }

    // 5. Render Remote Players (Interpolated)
    for (const remote of game.remotePlayers) {
      this.renderPlayer(ctx, remote, false);
    }

    // 6. Render Local Player (Predicted)
    if (game.localPlayer) {
      this.renderPlayer(ctx, game.localPlayer, true);
    }

    // 7. Render Block Placement / Mining Cursor
    if (game.cursorTile) {
      this.renderCursor(ctx, game.cursorTile, game.localPlayer);
    }

    // 8. Render Floating Chat Bubbles
    for (const chat of game.activeChats) {
      this.renderChatBubble(ctx, chat);
    }

    ctx.restore();
  }

  renderTilemap(ctx, tilemap) {
    if (!tilemap) return;
    const startTileX = Math.max(0, Math.floor(this.camX / this.tileSize));
    const endTileX = Math.min(tilemap.width, Math.ceil((this.camX + this.width) / this.tileSize));
    const startTileY = Math.max(0, Math.floor(this.camY / this.tileSize));
    const endTileY = Math.min(tilemap.height, Math.ceil((this.camY + this.height) / this.tileSize));

    for (let ty = startTileY; ty < endTileY; ty++) {
      for (let tx = startTileX; tx < endTileX; tx++) {
        const tile = tilemap.getTile(tx, ty);
        if (tile === 0) continue; // Air

        const px = tx * this.tileSize;
        const py = ty * this.tileSize;

        this.drawTile(ctx, tile, px, py);
      }
    }
  }

  drawTile(ctx, tile, x, y) {
    const s = this.tileSize;

    switch (tile) {
      case 1: // DIRT
        ctx.fillStyle = "#854d0e";
        ctx.fillRect(x, y, s, s);
        ctx.fillStyle = "#713f12";
        ctx.fillRect(x + 4, y + 4, 8, 8);
        ctx.fillRect(x + 18, y + 16, 6, 6);
        break;

      case 2: // GRASS
        ctx.fillStyle = "#854d0e";
        ctx.fillRect(x, y, s, s);
        ctx.fillStyle = "#22c55e";
        ctx.fillRect(x, y, s, 8); // lush green top
        ctx.fillStyle = "#16a34a";
        ctx.fillRect(x + 3, y + 6, 6, 6);
        ctx.fillRect(x + 16, y + 6, 8, 5);
        break;

      case 3: // STONE
        ctx.fillStyle = "#475569";
        ctx.fillRect(x, y, s, s);
        ctx.fillStyle = "#334155";
        ctx.fillRect(x + 2, y + 2, 12, 12);
        ctx.fillRect(x + 16, y + 16, 14, 14);
        break;

      case 4: // WOOD
        ctx.fillStyle = "#9a3412";
        ctx.fillRect(x, y, s, s);
        ctx.fillStyle = "#7c2d12";
        ctx.fillRect(x + 2, y, 4, s);
        ctx.fillRect(x + 16, y, 4, s);
        break;

      case 5: // BRICK
        ctx.fillStyle = "#b91c1c";
        ctx.fillRect(x, y, s, s);
        ctx.strokeStyle = "#450a0a";
        ctx.lineWidth = 1;
        ctx.strokeRect(x + 0.5, y + 0.5, s - 1, s - 1);
        ctx.strokeRect(x + 0.5, y + 15.5, s - 1, 1);
        break;

      case 6: // GOLD
        ctx.fillStyle = "#eab308";
        ctx.fillRect(x, y, s, s);
        ctx.fillStyle = "#fef08a";
        ctx.fillRect(x + 4, y + 4, 6, 6); // sparkle
        ctx.fillRect(x + 20, y + 18, 5, 5);
        break;

      case 7: // LAVA
        ctx.fillStyle = "#ea580c";
        ctx.fillRect(x, y, s, s);
        ctx.fillStyle = "#fbbf24";
        ctx.fillRect(x + 4, y + 6, 12, 4);
        break;

      case 8: // BEDROCK
        ctx.fillStyle = "#090d16";
        ctx.fillRect(x, y, s, s);
        ctx.strokeStyle = "#1e293b";
        ctx.lineWidth = 2;
        ctx.strokeRect(x + 2, y + 2, s - 4, s - 4);
        break;
    }
  }

  renderPlayer(ctx, p, isLocal = false) {
    const w = 20;
    const h = 30;
    const x = p.x - w / 2;
    const y = p.y;

    // Body
    ctx.fillStyle = p.color || "#00f0ff";
    ctx.fillRect(x, y + 8, w, h - 8);

    // Head
    ctx.fillStyle = "#fde047";
    ctx.fillRect(x + 2, y, w - 4, 12);

    // Eyes (facing direction)
    ctx.fillStyle = "#000";
    const eyeX = p.facing === 1 ? x + w - 7 : x + 3;
    ctx.fillRect(eyeX, y + 4, 3, 3);

    // Name tag
    ctx.fillStyle = "#fff";
    ctx.font = "bold 10px monospace";
    ctx.textAlign = "center";
    ctx.fillText(p.name, p.x, y - 6);

    // Local Player Indicator Pill
    if (isLocal) {
      ctx.fillStyle = "#00f0ff";
      ctx.beginPath();
      ctx.moveTo(p.x, y - 16);
      ctx.lineTo(p.x - 4, y - 22);
      ctx.lineTo(p.x + 4, y - 22);
      ctx.fill();
    }
  }

  renderGhostPlayer(ctx, pos) {
    const w = 20;
    const h = 30;
    const x = pos.x - w / 2;
    const y = pos.y;

    // Server Authoritative Ghost Box (Translucent Red Wireframe)
    ctx.strokeStyle = "rgba(239, 68, 68, 0.8)";
    ctx.lineWidth = 2;
    ctx.setLineDash([4, 2]);
    ctx.strokeRect(x, y, w, h);
    ctx.setLineDash([]);

    ctx.fillStyle = "rgba(239, 68, 68, 0.8)";
    ctx.font = "9px monospace";
    ctx.textAlign = "center";
    ctx.fillText("SERVER GHOST", pos.x, y + h + 10);
  }

  renderDrops(ctx, drops) {
    if (!drops) return;
    for (const d of drops) {
      ctx.fillStyle = "#f59e0b";
      ctx.beginPath();
      ctx.arc(d.x, d.y, 6, 0, Math.PI * 2);
      ctx.fill();
      ctx.strokeStyle = "#fff";
      ctx.lineWidth = 1;
      ctx.stroke();
    }
  }

  renderCursor(ctx, cursor, player) {
    const px = cursor.tx * this.tileSize;
    const py = cursor.ty * this.tileSize;

    const inRange = player && Math.hypot((px + 16) - player.x, (py + 16) - (player.y + 15)) <= 180;

    ctx.strokeStyle = inRange ? "#00f0ff" : "#ef4444";
    ctx.lineWidth = 2;
    ctx.strokeRect(px, py, this.tileSize, this.tileSize);
  }

  renderChatBubble(ctx, chat) {
    ctx.fillStyle = "rgba(15, 23, 42, 0.9)";
    ctx.strokeStyle = "#00f0ff";
    ctx.lineWidth = 1;

    const textW = ctx.measureText(chat.text).width + 16;
    ctx.fillRect(chat.x - textW / 2, chat.y - 42, textW, 22);
    ctx.strokeRect(chat.x - textW / 2, chat.y - 42, textW, 22);

    ctx.fillStyle = "#fff";
    ctx.font = "11px monospace";
    ctx.textAlign = "center";
    ctx.fillText(chat.text, chat.x, chat.y - 27);
  }
}

window.GameRenderer = GameRenderer;

