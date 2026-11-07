/**
 * physics.js — Player Physics, Collisions & Lava Hazard Handling
 */

import { BLOCK_SIZE, WORLD_WIDTH, WORLD_HEIGHT, GRAVITY, JUMP_FORCE, MOVE_SPEED, ITEMS } from "./constants.js";
import { state } from "./state.js";
import { isSolidBlock, isHazardBlock } from "./world.js";
import { sound } from "./audio.js";
import { addSmoke, addTextPopup } from "./particles.js";
import { sendWs } from "./network.js";

export function updatePhysics(canvas) {
  const p = state.player;
  const hasAngelWings = state.wearables.wings === ITEMS.ANGEL_WINGS.id;

  // Invulnerability timer
  if (p.invulnTimer > 0) p.invulnTimer--;

  // Horizontal acceleration
  if (state.keys.left) {
    p.vx = -MOVE_SPEED;
    p.facingRight = false;
    p.animTime += 0.25;
  } else if (state.keys.right) {
    p.vx = MOVE_SPEED;
    p.facingRight = true;
    p.animTime += 0.25;
  } else {
    p.vx *= 0.75;
    if (Math.abs(p.vx) < 0.1) p.vx = 0;
  }

  // Jump logic
  if (state.keys.jump) {
    if (p.grounded) {
      p.vy = JUMP_FORCE;
      p.grounded = false;
      p.jumpsLeft = hasAngelWings ? 1 : 0;
      sound.jump();
    } else if (hasAngelWings && p.jumpsLeft > 0 && p.vy > -2) {
      // Double jump with Angel Wings
      p.vy = JUMP_FORCE * 0.9;
      p.jumpsLeft--;
      sound.jump();
    } else if (hasAngelWings && p.vy > 0) {
      // Slow gliding while holding jump
      p.vy = Math.min(p.vy, 1.8);
      p.isGliding = true;
    }
  } else {
    p.isGliding = false;
  }

  // Gravity
  p.vy += GRAVITY;
  if (p.vy > 12) p.vy = 12;

  // Move X & Check Horizontal Tile Collisions
  p.x += p.vx;
  checkCollisionX();

  // Move Y & Check Vertical Tile Collisions
  p.y += p.vy;
  p.grounded = false;
  checkCollisionY();

  // Check Hazard (Lava)
  checkLavaHazard();

  // Punch animation timer
  if (p.punching) {
    p.punchTimer--;
    if (p.punchTimer <= 0) p.punching = false;
  }

  // Camera smooth follow
  const targetCamX = p.x + p.w / 2 - canvas.width / 2;
  const targetCamY = p.y + p.h / 2 - canvas.height / 2;
  state.camera.x += (targetCamX - state.camera.x) * 0.12;
  state.camera.y += (targetCamY - state.camera.y) * 0.12;

  // Clamp camera
  const maxCamX = WORLD_WIDTH * BLOCK_SIZE - canvas.width;
  const maxCamY = WORLD_HEIGHT * BLOCK_SIZE - canvas.height;
  state.camera.x = Math.max(0, Math.min(state.camera.x, Math.max(0, maxCamX)));
  state.camera.y = Math.max(0, Math.min(state.camera.y, Math.max(0, maxCamY)));

  // Broadcast position to server
  if (Math.abs(p.vx) > 0.05 || Math.abs(p.vy) > 0.05) {
    sendWs({
      type: "MOVE",
      x: Math.round(p.x),
      y: Math.round(p.y),
      facingRight: p.facingRight,
    });
  }
}

function checkCollisionX() {
  const p = state.player;
  const leftTile = Math.floor(p.x / BLOCK_SIZE);
  const rightTile = Math.floor((p.x + p.w) / BLOCK_SIZE);
  const topTile = Math.floor(p.y / BLOCK_SIZE);
  const bottomTile = Math.floor((p.y + p.h - 1) / BLOCK_SIZE);

  for (let y = topTile; y <= bottomTile; y++) {
    for (let x = leftTile; x <= rightTile; x++) {
      if (isSolidBlock(x, y)) {
        if (p.vx > 0) {
          p.x = x * BLOCK_SIZE - p.w - 0.01;
          p.vx = 0;
        } else if (p.vx < 0) {
          p.x = (x + 1) * BLOCK_SIZE + 0.01;
          p.vx = 0;
        }
      }
    }
  }
}

function checkCollisionY() {
  const p = state.player;
  const leftTile = Math.floor((p.x + 2) / BLOCK_SIZE);
  const rightTile = Math.floor((p.x + p.w - 2) / BLOCK_SIZE);
  const topTile = Math.floor(p.y / BLOCK_SIZE);
  const bottomTile = Math.floor((p.y + p.h) / BLOCK_SIZE);

  for (let y = topTile; y <= bottomTile; y++) {
    for (let x = leftTile; x <= rightTile; x++) {
      if (isSolidBlock(x, y)) {
        if (p.vy > 0) {
          p.y = y * BLOCK_SIZE - p.h;
          p.vy = 0;
          p.grounded = true;
          p.jumpsLeft = state.wearables.wings === ITEMS.ANGEL_WINGS.id ? 1 : 0;
        } else if (p.vy < 0) {
          p.y = (y + 1) * BLOCK_SIZE;
          p.vy = 0;
        }
      }
    }
  }
}

function checkLavaHazard() {
  const p = state.player;
  if (p.invulnTimer > 0) return;

  const centerTileX = Math.floor((p.x + p.w / 2) / BLOCK_SIZE);
  const centerTileY = Math.floor((p.y + p.h / 2) / BLOCK_SIZE);

  if (isHazardBlock(centerTileX, centerTileY)) {
    // Burned by lava!
    sound.hurt();
    addSmoke(p.x + p.w / 2, p.y + p.h / 2);
    addTextPopup(p.x, p.y - 12, "🔥 OUCH! BURNED!", "#ef4444");

    // Respawn at White Door
    const doorX = Math.floor(WORLD_WIDTH / 2);
    p.x = doorX * BLOCK_SIZE + 4;
    p.y = (24 - 3) * BLOCK_SIZE;
    p.vx = 0;
    p.vy = 0;
    p.invulnTimer = 60; // 1s invulnerability
    sound.respawn();
  }
}

