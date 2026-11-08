/**
 * 2D Sandbox MMO — Network Protocol & Tile Definitions
 */

const MSG_TYPES = {
  JOIN: "JOIN",
  INIT_WORLD: "INIT_WORLD",
  INPUT: "INPUT",
  SNAPSHOT: "SNAPSHOT",
  TILE_INTERACT: "TILE_INTERACT",
  TILE_UPDATE: "TILE_UPDATE",
  ITEM_DROP: "ITEM_DROP",
  ITEM_PICKUP: "ITEM_PICKUP",
  CHAT: "CHAT",
  PING: "PING",
  PONG: "PONG"
};

const TILE_TYPES = {
  AIR: 0,
  DIRT: 1,
  GRASS: 2,
  STONE: 3,
  WOOD: 4,
  BRICK: 5,
  GOLD: 6,
  LAVA: 7,
  BEDROCK: 8
};

const TILE_PROPERTIES = {
  [TILE_TYPES.AIR]: { name: "Air", solid: false, maxHp: 0 },
  [TILE_TYPES.DIRT]: { name: "Dirt", solid: true, maxHp: 3, color: "#8b5a2b", drop: TILE_TYPES.DIRT },
  [TILE_TYPES.GRASS]: { name: "Grass", solid: true, maxHp: 3, color: "#22c55e", drop: TILE_TYPES.DIRT },
  [TILE_TYPES.STONE]: { name: "Stone", solid: true, maxHp: 6, color: "#64748b", drop: TILE_TYPES.STONE },
  [TILE_TYPES.WOOD]: { name: "Wood", solid: true, maxHp: 4, color: "#b45309", drop: TILE_TYPES.WOOD },
  [TILE_TYPES.BRICK]: { name: "Brick", solid: true, maxHp: 5, color: "#dc2626", drop: TILE_TYPES.BRICK },
  [TILE_TYPES.GOLD]: { name: "Gold Block", solid: true, maxHp: 8, color: "#eab308", drop: TILE_TYPES.GOLD },
  [TILE_TYPES.LAVA]: { name: "Lava", solid: false, damage: 25, color: "#f97316", drop: null },
  [TILE_TYPES.BEDROCK]: { name: "Bedrock", solid: true, maxHp: 999999, color: "#0f172a", drop: null }
};

module.exports = {
  MSG_TYPES,
  TILE_TYPES,
  TILE_PROPERTIES,
  TILE_SIZE: 32 // 32x32 pixels per tile
};

