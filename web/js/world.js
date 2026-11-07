/**
 * world.js — Dual-Layer World Generator & Tile Helpers
 */

import { WORLD_WIDTH, WORLD_HEIGHT, ITEMS, getItemById } from "./constants.js";
import { state } from "./state.js";

export function generateWorld(name) {
  const fg = [];
  const bg = [];
  const surfaceY = 24;

  for (let y = 0; y < WORLD_HEIGHT; y++) {
    const fgRow = [];
    const bgRow = [];

    for (let x = 0; x < WORLD_WIDTH; x++) {
      if (y < surfaceY) {
        fgRow.push(null);
        bgRow.push(null);
      } else if (y === surfaceY) {
        fgRow.push(ITEMS.DIRT.id);
        bgRow.push(ITEMS.DIRT_BG.id);
      } else if (y < 55) {
        const rand = Math.random();
        if (rand < 0.15) {
          fgRow.push(ITEMS.CAVE.id);
          bgRow.push(ITEMS.CAVE_BG.id);
        } else if (rand < 0.20 && y > 35) {
          fgRow.push(ITEMS.ROCK.id);
          bgRow.push(ITEMS.CAVE_BG.id);
        } else if (rand < 0.22 && y > 40) {
          fgRow.push(ITEMS.LAVA.id);
          bgRow.push(ITEMS.CAVE_BG.id);
        } else {
          fgRow.push(ITEMS.DIRT.id);
          bgRow.push(ITEMS.DIRT_BG.id);
        }
      } else {
        fgRow.push(ITEMS.BEDROCK.id);
        bgRow.push(ITEMS.CAVE_BG.id);
      }
    }
    fg.push(fgRow);
    bg.push(bgRow);
  }

  // Place White Door at center
  const doorX = Math.floor(WORLD_WIDTH / 2);
  fg[surfaceY - 1][doorX] = ITEMS.DOOR.id;
  fg[surfaceY - 2][doorX] = ITEMS.DOOR.id;

  const doors = new Map();
  doors.set(`${doorX},${surfaceY - 1}`, { targetWorld: "EXIT", label: "Exit Door" });

  return {
    fg,
    bg,
    signs: new Map(),
    doors,
  };
}

export function isSolidBlock(x, y) {
  if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) return true;
  if (!state.world || !state.world.fg) return false;
  const id = state.world.fg[y]?.[x];
  if (id === null || id === undefined) return false;
  const item = getItemById(id);
  return item && item.solid && !item.isHazard;
}

export function isHazardBlock(x, y) {
  if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) return false;
  if (!state.world || !state.world.fg) return false;
  const id = state.world.fg[y]?.[x];
  if (id === null || id === undefined) return false;
  const item = getItemById(id);
  return item && item.isHazard;
}

export function isPlatformBlock(x, y) {
  if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) return false;
  if (!state.world || !state.world.fg) return false;
  const id = state.world.fg[y]?.[x];
  if (id === null || id === undefined) return false;
  const item = getItemById(id);
  return item && item.isPlatform;
}
