/**
 * locks.js — World Lock, Area Protection & Permissions
 */

import { ITEMS } from "./constants.js";
import { state } from "./state.js";
import { sound } from "./audio.js";
import { consumeFromBackpack } from "./items.js";
import { showToast, updateHudLockBadge } from "./ui.js";
import { sendWs } from "./network.js";

export function handleLockPlacement(tileX, tileY, lockItem) {
  if (state.worldLock) {
    showToast(`This world is already locked by ${state.worldLock.owner}!`);
    return;
  }
  if (state.world.fg[tileY]?.[tileX] !== null) {
    showToast("Clear the tile before placing lock!");
    return;
  }

  consumeFromBackpack(state.selectedSlot, 1);
  state.world.fg[tileY][tileX] = lockItem.id;
  state.worldLock = {
    owner: state.username,
    type: lockItem.name,
    tileX,
    tileY,
    accessList: [],
    isPublic: false,
  };
  updateHudLockBadge();
  sound.lock();
  showToast(`🔒 World claimed by ${state.username}!`);

  sendWs({
    type: "CLAIM_WORLD_LOCK",
    tileX,
    tileY,
    lockType: lockItem.name,
  });
}

export function canModifyTile(tileX, tileY) {
  if (!state.worldLock) return true;
  if (state.worldLock.owner === state.username) return true;
  if (state.worldLock.isPublic) return true;
  return (state.worldLock.accessList || []).includes(state.username);
}

