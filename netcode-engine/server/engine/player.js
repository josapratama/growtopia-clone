/**
 * 2D Sandbox MMO — Player Entity & AABB Physics
 */

const { TILE_SIZE, TILE_PROPERTIES } = require("./protocol");

class Player {
  constructor(id, name, x = 200, y = 200) {
    this.id = id;
    this.name = name || `Guest_${id.substring(0, 4)}`;
    this.x = x;
    this.y = y;
    this.vx = 0;
    this.vy = 0;
    this.width = 20;
    this.height = 30;

    // Movement Physics Constants
    this.moveSpeed = 180;  // px/s
    this.jumpForce = -360; // px/s
    this.gravity = 950;    // px/s^2

    // State
    this.isGrounded = false;
    this.facing = 1; // 1 = right, -1 = left
    this.lastProcessedSeq = 0;
    this.color = this.generateColor(name);

    // Inventory: itemType -> count
    this.inventory = {
      1: 64, // Dirt
      2: 32, // Grass
      3: 32, // Stone
      4: 32, // Wood
      5: 16, // Brick
      6: 5   // Gold Block
    };
  }

  generateColor(seed) {
    const colors = ["#00f0ff", "#3b82f6", "#10b981", "#f59e0b", "#ec4899", "#8b5cf6"];
    let hash = 0;
    for (let i = 0; i < (seed || "").length; i++) {
      hash = seed.charCodeAt(i) + ((hash << 5) - hash);
    }
    return colors[Math.abs(hash) % colors.length];
  }

  /**
   * Deterministic movement step.
   * Can be executed identically on both Server and Client!
   */
  applyInput(input, tilemap, dt) {
    // 1. Horizontal movement
    let moveDir = 0;
    if (input.left) moveDir -= 1;
    if (input.right) moveDir += 1;

    this.vx = moveDir * this.moveSpeed;
    if (moveDir !== 0) {
      this.facing = moveDir;
    }

    // 2. Jump
    if (input.jump && this.isGrounded) {
      this.vy = this.jumpForce;
      this.isGrounded = false;
    }

    // 3. Gravity
    this.vy += this.gravity * dt;
    if (this.vy > 650) this.vy = 650; // terminal velocity

    // 4. AABB Collision & Integration
    this.integratePosition(tilemap, dt);

    if (input.seq) {
      this.lastProcessedSeq = input.seq;
    }
  }

  integratePosition(tilemap, dt) {
    // Move X first and check horizontal collision
    this.x += this.vx * dt;
    this.resolveCollisionX(tilemap);

    // Move Y next and check vertical collision
    this.y += this.vy * dt;
    this.resolveCollisionY(tilemap);
  }

  resolveCollisionX(tilemap) {
    const box = this.getBounds();
    const minTileX = Math.floor(box.left / TILE_SIZE);
    const maxTileX = Math.floor(box.right / TILE_SIZE);
    const minTileY = Math.floor(box.top / TILE_SIZE);
    const maxTileY = Math.floor(box.bottom / TILE_SIZE);

    for (let ty = minTileY; ty <= maxTileY; ty++) {
      for (let tx = minTileX; tx <= maxTileX; tx++) {
        if (tilemap.isSolid(tx, ty)) {
          if (this.vx > 0) {
            this.x = tx * TILE_SIZE - this.width / 2;
            this.vx = 0;
          } else if (this.vx < 0) {
            this.x = (tx + 1) * TILE_SIZE + this.width / 2;
            this.vx = 0;
          }
        }
      }
    }
  }

  resolveCollisionY(tilemap) {
    this.isGrounded = false;
    const box = this.getBounds();
    const minTileX = Math.floor(box.left / TILE_SIZE);
    const maxTileX = Math.floor(box.right / TILE_SIZE);
    const minTileY = Math.floor(box.top / TILE_SIZE);
    const maxTileY = Math.floor(box.bottom / TILE_SIZE);

    for (let ty = minTileY; ty <= maxTileY; ty++) {
      for (let tx = minTileX; tx <= maxTileX; tx++) {
        if (tilemap.isSolid(tx, ty)) {
          if (this.vy > 0) {
            // Landed on ground
            this.y = ty * TILE_SIZE - this.height;
            this.vy = 0;
            this.isGrounded = true;
          } else if (this.vy < 0) {
            // Hit ceiling
            this.y = (ty + 1) * TILE_SIZE;
            this.vy = 0;
          }
        }
      }
    }
  }

  getBounds() {
    return {
      left: this.x - this.width / 2,
      right: this.x + this.width / 2,
      top: this.y,
      bottom: this.y + this.height
    };
  }

  serialize(includePrivate = false) {
    const data = {
      id: this.id,
      name: this.name,
      x: Math.round(this.x * 10) / 10,
      y: Math.round(this.y * 10) / 10,
      vx: Math.round(this.vx),
      vy: Math.round(this.vy),
      facing: this.facing,
      isGrounded: this.isGrounded,
      color: this.color
    };
    if (includePrivate) {
      data.inventory = this.inventory;
      data.lastProcessedSeq = this.lastProcessedSeq;
    }
    return data;
  }
}

module.exports = { Player };

