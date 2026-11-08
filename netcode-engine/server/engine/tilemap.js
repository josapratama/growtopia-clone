/**
 * 2D Sandbox MMO — Procedural Tilemap & Block Physics
 */

const { TILE_TYPES, TILE_PROPERTIES, TILE_SIZE } = require("./protocol");

class Tilemap {
  constructor(width = 100, height = 60) {
    this.width = width;
    this.height = height;
    this.tiles = new Uint8Array(width * height);
    this.tileHp = new Uint8Array(width * height);
    this.dirtyTiles = []; // [{ x, y, type, hp }]
    this.itemDrops = [];  // [{ id, x, y, type, vx, vy }]
    this.dropIdCounter = 1;

    this.generateWorld();
  }

  getIndex(x, y) {
    return y * this.width + x;
  }

  getTile(x, y) {
    if (x < 0 || x >= this.width || y < 0 || y >= this.height) {
      return TILE_TYPES.BEDROCK; // Boundary is solid
    }
    return this.tiles[this.getIndex(x, y)];
  }

  setTile(x, y, type) {
    if (x < 0 || x >= this.width || y < 0 || y >= this.height) return false;
    const idx = this.getIndex(x, y);
    this.tiles[idx] = type;
    const prop = TILE_PROPERTIES[type] || {};
    this.tileHp[idx] = prop.maxHp || 0;
    this.dirtyTiles.push({ x, y, type, hp: this.tileHp[idx] });
    return true;
  }

  generateWorld() {
    // Generate terrain profile
    const surfaceY = Math.floor(this.height * 0.45);

    for (let x = 0; x < this.width; x++) {
      // Sine wave hills
      const hillHeight = Math.floor(Math.sin(x * 0.1) * 3 + Math.cos(x * 0.05) * 2);
      const groundLevel = surfaceY + hillHeight;

      for (let y = 0; y < this.height; y++) {
        let tile = TILE_TYPES.AIR;

        if (y === this.height - 1) {
          tile = TILE_TYPES.BEDROCK;
        } else if (y > groundLevel + 6) {
          // Deep stone with random gold ore
          tile = Math.random() < 0.06 ? TILE_TYPES.GOLD : TILE_TYPES.STONE;
        } else if (y > groundLevel) {
          tile = TILE_TYPES.DIRT;
        } else if (y === groundLevel) {
          tile = TILE_TYPES.GRASS;
        }

        const idx = this.getIndex(x, y);
        this.tiles[idx] = tile;
        const prop = TILE_PROPERTIES[tile] || {};
        this.tileHp[idx] = prop.maxHp || 0;
      }
    }
  }

  hitBlock(x, y, damage = 1) {
    if (x < 0 || x >= this.width || y < 0 || y >= this.height) return null;
    const idx = this.getIndex(x, y);
    const tile = this.tiles[idx];
    if (tile === TILE_TYPES.AIR || tile === TILE_TYPES.BEDROCK) return null;

    if (this.tileHp[idx] <= damage) {
      // Block destroyed!
      this.tiles[idx] = TILE_TYPES.AIR;
      this.tileHp[idx] = 0;
      this.dirtyTiles.push({ x, y, type: TILE_TYPES.AIR, hp: 0 });

      // Spawn item drop
      const prop = TILE_PROPERTIES[tile];
      if (prop && prop.drop !== null) {
        this.spawnDrop(x * TILE_SIZE + 16, y * TILE_SIZE + 16, prop.drop);
      }
      return { destroyed: true, type: tile };
    } else {
      this.tileHp[idx] -= damage;
      this.dirtyTiles.push({ x, y, type: tile, hp: this.tileHp[idx] });
      return { destroyed: false, hpRemaining: this.tileHp[idx] };
    }
  }

  placeBlock(x, y, type) {
    if (x < 0 || x >= this.width || y < 0 || y >= this.height) return false;
    const current = this.getTile(x, y);
    if (current !== TILE_TYPES.AIR) return false;

    // Check adjacency (must be placed next to solid block)
    const hasNeighbor =
      this.isSolid(x - 1, y) ||
      this.isSolid(x + 1, y) ||
      this.isSolid(x, y - 1) ||
      this.isSolid(x, y + 1);

    if (!hasNeighbor) return false;

    this.setTile(x, y, type);
    return true;
  }

  isSolid(x, y) {
    const tile = this.getTile(x, y);
    return TILE_PROPERTIES[tile]?.solid || false;
  }

  spawnDrop(px, py, itemType) {
    const drop = {
      id: this.dropIdCounter++,
      x: px,
      y: py,
      type: itemType,
      vx: (Math.random() - 0.5) * 60,
      vy: -120 - Math.random() * 60
    };
    this.itemDrops.push(drop);
  }

  updateDrops(dt) {
    const gravity = 400; // px/s^2
    for (let i = this.itemDrops.length - 1; i >= 0; i--) {
      const drop = this.itemDrops[i];
      drop.vy += gravity * dt;
      drop.x += drop.vx * dt;
      drop.y += drop.vy * dt;

      // Simple floor collision
      const tx = Math.floor(drop.x / TILE_SIZE);
      const ty = Math.floor((drop.y + 6) / TILE_SIZE);
      if (this.isSolid(tx, ty)) {
        drop.y = ty * TILE_SIZE - 6;
        drop.vy = 0;
        drop.vx *= 0.7; // friction
      }
    }
  }

  drainDirtyTiles() {
    const dirty = this.dirtyTiles;
    this.dirtyTiles = [];
    return dirty;
  }
}

module.exports = { Tilemap };

