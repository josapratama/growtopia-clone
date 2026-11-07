/**
 * constants.js — Game Constants, Items Registry & Splicing Matrix
 */

export const BLOCK_SIZE = 32;
export const WORLD_WIDTH = 100;
export const WORLD_HEIGHT = 60;
export const GRAVITY = 0.55;
export const JUMP_FORCE = -10.5;
export const MOVE_SPEED = 3.6;
export const INTERACTION_REACH = 6 * BLOCK_SIZE;

export const ITEMS = {
  // Tools
  FIST: { id: 0, name: "Punch", type: "tool", icon: "👊", color: "#f87171" },
  WRENCH: { id: 1, name: "Wrench", type: "tool", icon: "🔧", color: "#60a5fa" },
  PICKAXE: { id: 50, name: "Pickaxe", type: "tool", slot: "hand", icon: "⛏️", color: "#94a3b8", desc: "Breaks rock and stone blocks with fewer hits!" },

  // Foreground Blocks
  DIRT: { id: 2, name: "Dirt", type: "block", icon: "🟫", color: "#8b4513", hits: 3, solid: true, seedId: 3, xp: 5 },
  CAVE: { id: 4, name: "Cave Block", type: "block", icon: "⬛", color: "#3e2723", hits: 4, solid: true, seedId: 15, xp: 8 },
  PLATFORM: { id: 5, name: "Wood Platform", type: "platform", icon: "🪵", color: "#b45309", hits: 2, solid: true, isPlatform: true, seedId: 19, xp: 6 },
  LAVA: { id: 6, name: "Lava", type: "block", icon: "🔥", color: "#ef4444", hits: 5, solid: true, isHazard: true, seedId: 17, xp: 12 },
  ROCK: { id: 7, name: "Rock", type: "block", icon: "🪨", color: "#78716c", hits: 6, solid: true, seedId: 14, xp: 10 },
  DOOR: { id: 8, name: "White Door", type: "door", icon: "🚪", color: "#f8fafc", hits: 10, solid: false, xp: 15 },
  BEDROCK: { id: 10, name: "Bedrock", type: "block", icon: "🧱", color: "#1c1917", hits: 99999, solid: true, xp: 0 },
  BRICK: { id: 11, name: "Brick", type: "block", icon: "🧱", color: "#b91c1c", hits: 4, solid: true, seedId: 18, xp: 10 },
  WOOD: { id: 12, name: "Wood Block", type: "block", icon: "🪵", color: "#78350f", hits: 3, solid: true, seedId: 16, xp: 7 },
  STONE_WALL: { id: 13, name: "Stone Wall", type: "block", icon: "🏛️", color: "#64748b", hits: 5, solid: true, xp: 12 },
  SIGN: { id: 22, name: "Bulletin Sign", type: "sign", icon: "🪧", color: "#d97706", hits: 3, solid: false, seedId: 26, xp: 8 },
  OBSIDIAN: { id: 23, name: "Obsidian", type: "block", icon: "🟣", color: "#1e1b4b", hits: 8, solid: true, seedId: 27, xp: 20 },
  GLASS: { id: 24, name: "Glass Window", type: "block", icon: "🪟", color: "#a5f3fc", hits: 2, solid: true, seedId: 28, xp: 6 },
  MUSHROOM: { id: 25, name: "Mushroom Block", type: "block", icon: "🍄", color: "#f43f5e", hits: 3, solid: true, seedId: 29, xp: 9 },

  // Background Wallpapers
  DIRT_BG: { id: 40, name: "Dirt Background", type: "background", icon: "🟫", color: "#542a0c", hits: 2, solid: false, xp: 4 },
  CAVE_BG: { id: 41, name: "Cave Background", type: "background", icon: "⬛", color: "#221714", hits: 3, solid: false, xp: 5 },
  WOOD_BG: { id: 42, name: "Wood Background", type: "background", icon: "🪵", color: "#451a03", hits: 2, solid: false, xp: 5 },
  BRICK_BG: { id: 43, name: "Brick Background", type: "background", icon: "🧱", color: "#7f1d1d", hits: 3, solid: false, xp: 7 },

  // Weather Machines
  WEATHER_NIGHT: { id: 51, name: "Weather - Night", type: "block", icon: "🌙", color: "#1e1b4b", hits: 3, weather: "NIGHT", desc: "Changes world weather to starry night!" },
  WEATHER_SUNSET: { id: 52, name: "Weather - Sunset", type: "block", icon: "🌅", color: "#ea580c", hits: 3, weather: "SUNSET", desc: "Changes world weather to golden sunset!" },
  WEATHER_RAIN: { id: 53, name: "Weather - Rain", type: "block", icon: "🌧️", color: "#0284c7", hits: 3, weather: "RAIN", desc: "Causes refreshing rain across the world!" },

  // Seeds
  SEED_DIRT: { id: 3, name: "Dirt Seed", type: "seed", icon: "🌱", color: "#84cc16", growsInto: 2, growTime: 12 },
  SEED_ROCK: { id: 14, name: "Rock Seed", type: "seed", icon: "🪨", color: "#a8a29e", growsInto: 7, growTime: 20 },
  SEED_CAVE: { id: 15, name: "Cave Seed", type: "seed", icon: "🌰", color: "#713f12", growsInto: 4, growTime: 16 },
  SEED_WOOD: { id: 16, name: "Wood Seed", type: "seed", icon: "🌿", color: "#65a30d", growsInto: 12, growTime: 25 },
  SEED_LAVA: { id: 17, name: "Lava Seed", type: "seed", icon: "🔥", color: "#f97316", growsInto: 6, growTime: 35 },
  SEED_BRICK: { id: 18, name: "Brick Seed", type: "seed", icon: "🍁", color: "#ea580c", growsInto: 11, growTime: 30 },
  SEED_PLATFORM: { id: 19, name: "Platform Seed", type: "seed", icon: "🎋", color: "#d97706", growsInto: 5, growTime: 20 },
  SEED_SIGN: { id: 26, name: "Sign Seed", type: "seed", icon: "📜", color: "#fef08a", growsInto: 22, growTime: 22 },
  SEED_OBSIDIAN: { id: 27, name: "Obsidian Seed", type: "seed", icon: "🔮", color: "#6366f1", growsInto: 23, growTime: 45 },
  SEED_GLASS: { id: 28, name: "Glass Seed", type: "seed", icon: "💎", color: "#38bdf8", growsInto: 24, growTime: 18 },
  SEED_MUSHROOM: { id: 29, name: "Mushroom Seed", type: "seed", icon: "🍄", color: "#fda4af", growsInto: 25, growTime: 24 },

  // Locks
  WORLD_LOCK: { id: 9, name: "World Lock", type: "lock", icon: "🔒", color: "#facc15", desc: "Locks the entire world for you and your friends!" },
  SMALL_LOCK: { id: 20, name: "Small Lock", type: "lock", icon: "🔐", color: "#e2e8f0", desc: "Locks 10 tiles area." },
  BIG_LOCK: { id: 21, name: "Big Lock", type: "lock", icon: "🔏", color: "#38bdf8", desc: "Locks 48 tiles area." },

  // Wearables & Equipment
  ANGEL_WINGS: { id: 30, name: "Angel Wings", type: "wearable", slot: "wings", icon: "🪽", color: "#f8fafc", desc: "Allows double jumping and slow gliding!" },
  ROYAL_CROWN: { id: 31, name: "Royal Crown", type: "wearable", slot: "hat", icon: "👑", color: "#facc15", desc: "A prestigious golden crown." },
  ROYAL_ROBE: { id: 32, name: "Royal Robe", type: "wearable", slot: "shirt", icon: "🥋", color: "#7c3aed", desc: "A royal velvet robe." },
  DEVIL_HORNS: { id: 33, name: "Devil Horns", type: "wearable", slot: "hat", icon: "😈", color: "#dc2626", desc: "Spooky fiery red horns!" },
  NINJA_HOOD: { id: 34, name: "Ninja Hood", type: "wearable", slot: "hat", icon: "🥷", color: "#18181b", desc: "Mysterious assassin ninja mask." },
};

// Splicing Matrix
export const SPLICING_RECIPES = {
  "3,14": 17, // Dirt Seed + Rock Seed = Lava Seed
  "14,3": 17,
  "3,15": 8,  // Dirt Seed + Cave Seed = Door
  "15,3": 8,
  "3,17": 16, // Dirt Seed + Lava Seed = Wood Seed
  "17,3": 16,
  "14,15": 26,// Rock Seed + Cave Seed = Sign Seed
  "15,14": 26,
  "16,3": 19, // Wood Seed + Dirt Seed = Platform Seed
  "3,16": 19,
  "14,16": 18,// Rock Seed + Wood Seed = Brick Seed
  "16,14": 18,
  "17,14": 27,// Lava Seed + Rock Seed = Obsidian Seed
  "14,17": 27,
  "14,19": 28,// Rock Seed + Platform Seed = Glass Seed
  "19,14": 28,
  "16,15": 29,// Wood Seed + Cave Seed = Mushroom Seed
  "15,16": 29,
};

export function getItemById(id) {
  for (const k in ITEMS) {
    if (ITEMS[k].id === id) return ITEMS[k];
  }
  return null;
}
