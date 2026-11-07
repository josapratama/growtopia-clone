/**
 * state.js — Centralized Game State & XP Leveling System
 */

import { ITEMS } from "./constants.js";
import { sound } from "./audio.js";
import { addTextPopup } from "./particles.js";

export const state = {
  screen: "MAIN_MENU", // "MAIN_MENU" | "WORLD_SELECT" | "WORLD_VIEW"
  username: "Guest_" + Math.floor(1000 + Math.random() * 9000),
  level: 1,
  xp: 0,
  xpNeeded: 100,
  gems: 250,
  currentWorldName: "MAIN",
  weather: "DAY", // "DAY" | "SUNSET" | "NIGHT" | "RAIN"
  world: {
    fg: [], // Foreground 2D array [y][x]
    bg: [], // Background 2D array [y][x]
    signs: new Map(), // "x,y" -> { text, owner }
    doors: new Map(), // "x,y" -> { targetWorld, label }
  },
  trees: [], // Array of { id, x, y, seedId, plantedAt, growTime, spliceWith }
  worldLock: null, // { owner, type, tileX, tileY, accessList, isPublic }
  droppedItems: [], // Array of { id, itemId, count, x, y, vx, vy, droppedAt, droppedBy }
  blockDamage: new Map(), // "x,y" -> hitsTaken
  remotePlayers: new Map(), // username -> player
  chatBubbles: new Map(), // username -> { text, timer }
  chatHistory: [],
  selectedSlot: 0,
  wearables: {
    hat: null,
    shirt: null,
    wings: null,
    hand: null,
  },
  backpack: [
    { item: ITEMS.FIST, count: 1 },
    { item: ITEMS.WRENCH, count: 1 },
    { item: ITEMS.PICKAXE, count: 1 },
    { item: ITEMS.DIRT, count: 50 },
    { item: ITEMS.DIRT_BG, count: 30 },
    { item: ITEMS.SEED_DIRT, count: 15 },
    { item: ITEMS.ROCK, count: 20 },
    { item: ITEMS.SEED_ROCK, count: 10 },
    { item: ITEMS.PLATFORM, count: 30 },
    { item: ITEMS.SIGN, count: 5 },
    { item: ITEMS.WORLD_LOCK, count: 1 },
    { item: ITEMS.ANGEL_WINGS, count: 1 },
    { item: ITEMS.ROYAL_CROWN, count: 1 },
    { item: ITEMS.ROYAL_ROBE, count: 1 },
  ],
  camera: { x: 0, y: 0 },
  player: {
    x: 0,
    y: 0,
    w: 24,
    h: 30,
    vx: 0,
    vy: 0,
    facingRight: true,
    grounded: false,
    punching: false,
    punchTimer: 0,
    animTime: 0,
    skinColor: "#ffdbac",
    hairColor: "#f59e0b",
    shirtColor: "#3b82f6",
    jumpsLeft: 1,
    isGliding: false,
    invulnTimer: 0,
  },
  keys: {
    left: false,
    right: false,
    jump: false,
  },
  mouse: {
    worldX: 0,
    worldY: 0,
    tileX: 0,
    tileY: 0,
    down: false,
  },
  activeTrade: null,
  activeItemActionIdx: null,
  activeSignTile: null,
  activeDoorTile: null,
  ws: null,
};

export function addXP(amount) {
  state.xp += amount;
  addTextPopup(state.player.x, state.player.y - 10, `+${amount} XP`, "#60a5fa");

  while (state.xp >= state.xpNeeded) {
    state.xp -= state.xpNeeded;
    state.level++;
    state.xpNeeded = Math.floor(state.xpNeeded * 1.5);
    sound.levelup();
    addTextPopup(state.player.x, state.player.y - 25, `LEVEL UP! (Lv. ${state.level})`, "#facc15");
  }

  const xpEl = document.getElementById("hud-level");
  if (xpEl) xpEl.innerText = `Lv. ${state.level}`;
}
