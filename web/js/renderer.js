/**
 * renderer.js — Canvas Rendering Engine (Weather, Dual-Layer Tiles, Doors, Trees, Drops & Wearables)
 */

import { BLOCK_SIZE, WORLD_WIDTH, WORLD_HEIGHT, ITEMS, getItemById } from "./constants.js";
import { state } from "./state.js";
import { renderParticles } from "./particles.js";

export function render(ctx, canvas) {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  if (state.screen === "WORLD_VIEW") {
    // 1. Weather & Sky Background
    renderSkyAndWeather(ctx, canvas);

    ctx.save();
    ctx.translate(-Math.round(state.camera.x), -Math.round(state.camera.y));

    const startCol = Math.max(0, Math.floor(state.camera.x / BLOCK_SIZE));
    const endCol = Math.min(WORLD_WIDTH - 1, Math.ceil((state.camera.x + canvas.width) / BLOCK_SIZE));
    const startRow = Math.max(0, Math.floor(state.camera.y / BLOCK_SIZE));
    const endRow = Math.min(WORLD_HEIGHT - 1, Math.ceil((state.camera.y + canvas.height) / BLOCK_SIZE));

    // 2. Render Background Layer (Darker Depth Wallpaper)
    renderBackgroundTiles(ctx, startCol, endCol, startRow, endRow);

    // 3. Render Foreground Layer (Solid Blocks)
    renderForegroundTiles(ctx, startCol, endCol, startRow, endRow);

    // 4. Render Signs & Door Labels
    renderSignsAndDoors(ctx, startCol, endCol, startRow, endRow);

    // 5. Render Trees (Sprout, Sapling, Mature)
    renderTrees(ctx, startCol, endCol, startRow, endRow);

    // 6. Render World Lock
    renderWorldLock(ctx);

    // 7. Render Dropped Items
    renderDroppedItems(ctx);

    // 8. Render Particles & Floating Text Popups
    renderParticles(ctx);

    // 9. Render Remote Players
    for (const [_, p] of state.remotePlayers) {
      drawPlayerSprite(ctx, p.x, p.y, p.facingRight, false, p.wearables, p.username);
    }

    // 10. Render Local Player
    const lp = state.player;
    drawPlayerSprite(ctx, lp.x, lp.y, lp.facingRight, lp.punching, state.wearables, state.username, true);

    // 11. Render Speech Bubbles
    renderSpeechBubbles(ctx);

    ctx.restore();

    // 12. Render Rain Foreground Overlay
    if (state.weather === "RAIN") {
      renderRainOverlay(ctx, canvas);
    }
  }
}

function renderSkyAndWeather(ctx, canvas) {
  if (state.weather === "NIGHT") {
    ctx.fillStyle = "#09090b";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Twinkling Stars
    ctx.fillStyle = "rgba(255, 255, 255, 0.85)";
    for (let i = 0; i < 40; i++) {
      const sx = (i * 73 + Date.now() * 0.005) % canvas.width;
      const sy = (i * 37) % (canvas.height * 0.6);
      ctx.fillRect(sx, sy, 2, 2);
    }
    // Glowing Moon
    ctx.fillStyle = "#fef08a";
    ctx.beginPath();
    ctx.arc(canvas.width - 80, 70, 24, 0, Math.PI * 2);
    ctx.fill();
  } else if (state.weather === "SUNSET") {
    const grad = ctx.createLinearGradient(0, 0, 0, canvas.height * 0.65);
    grad.addColorStop(0, "#4c1d95");
    grad.addColorStop(0.5, "#ea580c");
    grad.addColorStop(1, "#facc15");
    ctx.fillStyle = grad;
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Setting Sun
    ctx.fillStyle = "#ffedd5";
    ctx.beginPath();
    ctx.arc(canvas.width * 0.5, canvas.height * 0.45, 36, 0, Math.PI * 2);
    ctx.fill();
  } else if (state.weather === "RAIN") {
    ctx.fillStyle = "#334155";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
  } else {
    // Normal Sunny Day
    ctx.fillStyle = "#38bdf8";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Sun
    ctx.fillStyle = "#fef08a";
    ctx.beginPath();
    ctx.arc(70, 60, 26, 0, Math.PI * 2);
    ctx.fill();
  }
}

function renderRainOverlay(ctx, canvas) {
  ctx.strokeStyle = "rgba(186, 230, 253, 0.5)";
  ctx.lineWidth = 1.5;
  const time = Date.now() * 0.7;

  ctx.beginPath();
  for (let i = 0; i < 60; i++) {
    const rx = (i * 24 + time * 0.8) % canvas.width;
    const ry = (i * 32 + time * 1.5) % canvas.height;
    ctx.moveTo(rx, ry);
    ctx.lineTo(rx - 4, ry + 12);
  }
  ctx.stroke();
}

function renderBackgroundTiles(ctx, startCol, endCol, startRow, endRow) {
  if (!state.world.bg) return;
  for (let y = startRow; y <= endRow; y++) {
    for (let x = startCol; x <= endCol; x++) {
      const id = state.world.bg[y]?.[x];
      if (id !== null && id !== undefined) {
        const item = getItemById(id);
        if (item) {
          ctx.fillStyle = item.color;
          ctx.fillRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
          ctx.fillStyle = "rgba(0, 0, 0, 0.45)";
          ctx.fillRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
        }
      }
    }
  }
}

function renderForegroundTiles(ctx, startCol, endCol, startRow, endRow) {
  if (!state.world.fg) return;
  for (let y = startRow; y <= endRow; y++) {
    for (let x = startCol; x <= endCol; x++) {
      const id = state.world.fg[y]?.[x];
      if (id !== null && id !== undefined) {
        drawFgTile(ctx, x, y, id);
      }
    }
  }
}

function drawFgTile(ctx, x, y, id) {
  const px = x * BLOCK_SIZE;
  const py = y * BLOCK_SIZE;
  const item = getItemById(id);
  if (!item) return;

  if (id === ITEMS.DIRT.id) {
    ctx.fillStyle = item.color;
    ctx.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE);
    if (y === 24 || (y > 0 && state.world.fg[y - 1]?.[x] === null)) {
      ctx.fillStyle = "#22c55e";
      ctx.fillRect(px, py, BLOCK_SIZE, 6);
    }
  } else if (id === ITEMS.DOOR.id) {
    ctx.fillStyle = "#f8fafc";
    ctx.fillRect(px + 4, py, BLOCK_SIZE - 8, BLOCK_SIZE);
    ctx.fillStyle = "#e2e8f0";
    ctx.fillRect(px + 6, py + 2, BLOCK_SIZE - 12, BLOCK_SIZE - 4);
    ctx.fillStyle = "#334155";
    ctx.fillRect(px + BLOCK_SIZE - 12, py + BLOCK_SIZE / 2, 3, 3);
  } else if (id === ITEMS.PLATFORM.id) {
    ctx.fillStyle = "#b45309";
    ctx.fillRect(px, py, BLOCK_SIZE, 8);
    ctx.fillStyle = "#78350f";
    ctx.fillRect(px + 4, py + 8, 4, BLOCK_SIZE - 8);
    ctx.fillRect(px + BLOCK_SIZE - 8, py + 8, 4, BLOCK_SIZE - 8);
  } else if (id === ITEMS.GLASS.id) {
    ctx.fillStyle = "rgba(165, 243, 252, 0.65)";
    ctx.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE);
    ctx.strokeStyle = "#38bdf8";
    ctx.lineWidth = 1;
    ctx.strokeRect(px + 1, py + 1, BLOCK_SIZE - 2, BLOCK_SIZE - 2);
  } else {
    ctx.fillStyle = item.color;
    ctx.fillRect(px, py, BLOCK_SIZE, BLOCK_SIZE);
    ctx.strokeStyle = "rgba(0,0,0,0.25)";
    ctx.strokeRect(px + 0.5, py + 0.5, BLOCK_SIZE - 1, BLOCK_SIZE - 1);
  }

  // Draw Block Cracks if damaged
  const damage = state.blockDamage.get(`${x},${y}`);
  if (damage) {
    ctx.strokeStyle = "#fff";
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(px + 4, py + 4);
    ctx.lineTo(px + 16, py + 16);
    ctx.lineTo(px + 28, py + 8);
    ctx.stroke();
  }
}

function renderSignsAndDoors(ctx, startCol, endCol, startRow, endRow) {
  // Signs
  if (state.world.signs) {
    state.world.signs.forEach((sign, key) => {
      const [tx, ty] = key.split(",").map(Number);
      if (tx < startCol - 1 || tx > endCol + 1 || ty < startRow - 1 || ty > endRow + 1) return;

      const px = tx * BLOCK_SIZE;
      const py = ty * BLOCK_SIZE;

      ctx.fillStyle = "#78350f";
      ctx.fillRect(px + 14, py + 16, 4, 16);
      ctx.fillStyle = "#d97706";
      ctx.fillRect(px + 4, py + 4, 24, 16);
      ctx.strokeStyle = "#92400e";
      ctx.lineWidth = 1.5;
      ctx.strokeRect(px + 4, py + 4, 24, 16);

      if (sign.text) {
        ctx.fillStyle = "rgba(0,0,0,0.7)";
        ctx.fillRect(px - 12, py - 8, 56, 10);
        ctx.fillStyle = "#fef08a";
        ctx.font = "6px 'Press Start 2P', monospace";
        ctx.textAlign = "center";
        const displayTxt = sign.text.length > 7 ? sign.text.slice(0, 6) + "…" : sign.text;
        ctx.fillText(displayTxt, px + 16, py - 1);
      }
    });
  }

  // Doors
  if (state.world.doors) {
    state.world.doors.forEach((door, key) => {
      const [tx, ty] = key.split(",").map(Number);
      if (tx < startCol - 1 || tx > endCol + 1 || ty < startRow - 1 || ty > endRow + 1) return;

      const px = tx * BLOCK_SIZE;
      const py = ty * BLOCK_SIZE;

      if (door.label || door.targetWorld) {
        ctx.fillStyle = "rgba(15, 23, 42, 0.85)";
        ctx.fillRect(px - 14, py - 12, 60, 11);
        ctx.fillStyle = "#38bdf8";
        ctx.font = "6px 'Press Start 2P', monospace";
        ctx.textAlign = "center";
        ctx.fillText(`🚪 ${door.targetWorld || "EXIT"}`, px + 16, py - 4);
      }
    });
  }
}

function renderTrees(ctx, startCol, endCol, startRow, endRow) {
  if (!state.trees) return;

  state.trees.forEach(t => {
    if (t.x < startCol - 1 || t.x > endCol + 1 || t.y < startRow - 2 || t.y > endRow + 1) return;

    const px = t.x * BLOCK_SIZE;
    const py = t.y * BLOCK_SIZE;
    const seedItem = getItemById(t.seedId);
    const fruitItem = seedItem ? getItemById(seedItem.growsInto) : null;
    const elapsedSec = (Date.now() - t.plantedAt) / 1000;
    const progress = Math.min(1, elapsedSec / t.growTime);

    if (progress < 0.35) {
      ctx.fillStyle = "#15803d";
      ctx.fillRect(px + 14, py + 22, 4, 10);
      ctx.fillStyle = "#84cc16";
      ctx.beginPath();
      ctx.arc(px + 16, py + 20, 5, 0, Math.PI * 2);
      ctx.fill();
    } else if (progress < 1) {
      ctx.fillStyle = "#78350f";
      ctx.fillRect(px + 13, py + 10, 6, 22);
      ctx.fillStyle = seedItem ? seedItem.color : "#22c55e";
      ctx.beginPath();
      ctx.arc(px + 16, py + 8, 10, 0, Math.PI * 2);
      ctx.fill();
    } else {
      ctx.fillStyle = "#78350f";
      ctx.fillRect(px + 12, py + 4, 8, 28);
      ctx.fillStyle = fruitItem ? fruitItem.color : "#16a34a";
      ctx.beginPath();
      ctx.arc(px + 16, py + 2, 14, 0, Math.PI * 2);
      ctx.fill();

      ctx.fillStyle = fruitItem ? fruitItem.color : "#facc15";
      ctx.fillRect(px + 4, py - 4, 8, 8);
      ctx.fillRect(px + 20, py - 2, 8, 8);

      const sparkle = Math.sin(Date.now() * 0.008) * 3;
      ctx.fillStyle = "rgba(255, 255, 255, 0.85)";
      ctx.beginPath();
      ctx.arc(px + 16, py - 12 + sparkle, 3, 0, Math.PI * 2);
      ctx.fill();
    }
  });
}

function renderWorldLock(ctx) {
  if (!state.worldLock) return;
  const { tileX, tileY, owner } = state.worldLock;
  const px = tileX * BLOCK_SIZE;
  const py = tileY * BLOCK_SIZE;

  ctx.fillStyle = "#facc15";
  ctx.fillRect(px + 6, py + 10, 20, 18);
  ctx.strokeStyle = "#ca8a04";
  ctx.lineWidth = 3;
  ctx.beginPath();
  ctx.arc(px + 16, py + 10, 7, Math.PI, 0);
  ctx.stroke();

  ctx.fillStyle = "#fff";
  ctx.fillRect(px + 14, py + 16, 4, 6);

  ctx.fillStyle = "rgba(0, 0, 0, 0.75)";
  ctx.fillRect(px - 14, py - 14, 60, 12);
  ctx.fillStyle = "#fef08a";
  ctx.font = "7px 'Press Start 2P', monospace";
  ctx.textAlign = "center";
  ctx.fillText(owner, px + 16, py - 5);
}

function renderDroppedItems(ctx) {
  if (!state.droppedItems) return;
  const time = Date.now() * 0.005;

  state.droppedItems.forEach(d => {
    const bob = Math.sin(time + d.x) * 3;
    const px = d.x;
    const py = d.y + bob;

    ctx.font = "14px serif";
    ctx.textAlign = "center";
    ctx.fillText(d.icon, px, py);

    ctx.fillStyle = "rgba(0, 0, 0, 0.65)";
    ctx.fillRect(px - 18, py - 18, 36, 9);
    ctx.fillStyle = "#fef08a";
    ctx.font = "6px 'Press Start 2P', monospace";
    ctx.fillText(`x${d.count}`, px, py - 11);
  });
}

function drawPlayerSprite(ctx, x, y, facingRight, punching, wearables, name, isLocal = false) {
  ctx.save();
  ctx.translate(Math.round(x + 12), Math.round(y + 15));
  if (!facingRight) ctx.scale(-1, 1);

  // 1. Wearable: Angel Wings
  if (wearables && wearables.wings === ITEMS.ANGEL_WINGS.id) {
    const flap = Math.sin(Date.now() * 0.012) * 4;
    ctx.fillStyle = "#f8fafc";
    ctx.strokeStyle = "#93c5fd";
    ctx.lineWidth = 1.5;

    ctx.beginPath();
    ctx.ellipse(-14, -6 + flap, 12, 6, -0.4, 0, Math.PI * 2);
    ctx.fill();
    ctx.stroke();
  }

  // 2. Torso & Shirt
  ctx.fillStyle = (wearables && wearables.shirt === ITEMS.ROYAL_ROBE.id) ? "#7c3aed" : state.player.shirtColor;
  ctx.fillRect(-8, -4, 16, 18);

  if (wearables && wearables.shirt === ITEMS.ROYAL_ROBE.id) {
    ctx.fillStyle = "#facc15";
    ctx.fillRect(-8, 12, 16, 3);
    ctx.fillRect(-2, -4, 4, 16);
  }

  // 3. Legs
  ctx.fillStyle = "#1e3a8a";
  ctx.fillRect(-7, 14, 6, 8);
  ctx.fillRect(1, 14, 6, 8);

  // 4. Head & Face
  ctx.fillStyle = state.player.skinColor;
  ctx.fillRect(-7, -18, 14, 14);

  // Hair
  ctx.fillStyle = state.player.hairColor;
  ctx.fillRect(-8, -21, 16, 6);

  // Eyes
  ctx.fillStyle = "#0f172a";
  ctx.fillRect(2, -13, 3, 3);

  // 5. Wearables: Hat Slot
  if (wearables) {
    if (wearables.hat === ITEMS.ROYAL_CROWN.id) {
      ctx.fillStyle = "#facc15";
      ctx.beginPath();
      ctx.moveTo(-9, -21);
      ctx.lineTo(-9, -29);
      ctx.lineTo(-4, -24);
      ctx.lineTo(0, -30);
      ctx.lineTo(4, -24);
      ctx.lineTo(9, -29);
      ctx.lineTo(9, -21);
      ctx.closePath();
      ctx.fill();
      ctx.fillStyle = "#ef4444";
      ctx.fillRect(-2, -25, 4, 3);
    } else if (wearables.hat === ITEMS.DEVIL_HORNS.id) {
      ctx.fillStyle = "#dc2626";
      ctx.beginPath();
      ctx.moveTo(-8, -20);
      ctx.lineTo(-12, -30);
      ctx.lineTo(-4, -22);
      ctx.fill();
      ctx.beginPath();
      ctx.moveTo(8, -20);
      ctx.lineTo(12, -30);
      ctx.lineTo(4, -22);
      ctx.fill();
    } else if (wearables.hat === ITEMS.NINJA_HOOD.id) {
      ctx.fillStyle = "#18181b";
      ctx.fillRect(-8, -21, 16, 12);
      ctx.fillRect(-8, -9, 16, 6);
    }
  }

  // 6. Arm / Punch & Hand Tool (Pickaxe)
  ctx.fillStyle = state.player.skinColor;
  if (punching) {
    ctx.fillRect(6, -2, 12, 6);
    // Draw Pickaxe in hand if equipped
    if (wearables && wearables.hand === ITEMS.PICKAXE.id) {
      ctx.fillStyle = "#78350f";
      ctx.fillRect(14, -8, 3, 14);
      ctx.fillStyle = "#94a3b8";
      ctx.fillRect(11, -11, 9, 3);
    }
  } else {
    ctx.fillRect(3, -2, 5, 10);
    if (wearables && wearables.hand === ITEMS.PICKAXE.id) {
      ctx.fillStyle = "#78350f";
      ctx.fillRect(4, 2, 3, 12);
      ctx.fillStyle = "#94a3b8";
      ctx.fillRect(1, 0, 9, 3);
    }
  }

  ctx.restore();

  // Nameplate & Level
  ctx.fillStyle = "rgba(0, 0, 0, 0.7)";
  ctx.fillRect(x - 12, y - 20, 48, 11);
  ctx.fillStyle = isLocal ? "#38bdf8" : "#f1f5f9";
  ctx.font = "7px 'Press Start 2P', monospace";
  ctx.textAlign = "center";
  ctx.fillText(name, x + 12, y - 11);
}

function renderSpeechBubbles(ctx) {
  for (const [uname, bubble] of state.chatBubbles) {
    bubble.timer--;
    if (bubble.timer <= 0) {
      state.chatBubbles.delete(uname);
      continue;
    }

    let px = state.player.x + 12;
    let py = state.player.y - 24;

    if (uname !== state.username) {
      const rp = state.remotePlayers.get(uname);
      if (rp) {
        px = rp.x + 12;
        py = rp.y - 24;
      }
    }

    ctx.font = "7px 'Press Start 2P', monospace";
    const textWidth = ctx.measureText(bubble.text).width;
    const bw = textWidth + 14;
    const bh = 18;

    ctx.fillStyle = "rgba(255, 255, 255, 0.95)";
    ctx.strokeStyle = "#0f172a";
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.roundRect(px - bw / 2, py - bh, bw, bh, 6);
    ctx.fill();
    ctx.stroke();

    ctx.fillStyle = "#0f172a";
    ctx.textAlign = "center";
    ctx.fillText(bubble.text, px, py - 6);
  }
}
