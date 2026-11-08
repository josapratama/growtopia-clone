/**
 * items.js — Dropped Items Physics, Backpack & Equipment Management
 */

import { BLOCK_SIZE, GRAVITY, getItemById } from "./constants.js";
import { state } from "./state.js";
import { isSolidBlock } from "./world.js";
import { sound } from "./audio.js";
import { addTextPopup } from "./particles.js";
import { showToast, renderBackpackUI, updateEquipmentHUD } from "./ui.js";
import { sendWs } from "./network.js";

export function updateDroppedItems() {
  if (!state.droppedItems) return;
  const p = state.player;

  for (let i = state.droppedItems.length - 1; i >= 0; i--) {
    const d = state.droppedItems[i];

    // Physics
    d.vy += GRAVITY * 0.5;
    if (d.vy > 8) d.vy = 8;
    d.x += d.vx;
    d.y += d.vy;
    d.vx *= 0.92;

    // Floor tile collision with soft bounce
    const tileX = Math.floor(d.x / BLOCK_SIZE);
    const tileY = Math.floor((d.y + 12) / BLOCK_SIZE);
    if (isSolidBlock(tileX, tileY)) {
      d.y = tileY * BLOCK_SIZE - 12;
      if (Math.abs(d.vy) > 1.5) {
        d.vy = -d.vy * 0.35;
      } else {
        d.vy = 0;
      }
      d.vx *= 0.8;
    }

    // Auto-pickup proximity
    const px = p.x + p.w / 2;
    const py = p.y + p.h / 2;
    const dist = Math.hypot(px - d.x, py - d.y);

    // Dropper has 3s protection so they don't immediately vacuum their own dropped items
    const isDropper = d.droppedBy === state.username;
    const pickupDelay = isDropper ? 3000 : 1000;

    if (dist < 36 && Date.now() - d.droppedAt > pickupDelay) {
      const dropId = d.id;
      state.droppedItems.splice(i, 1);
      sound.gem();
      addToBackpackById(d.itemId, d.count);
      addTextPopup(d.x, d.y - 10, `+${d.count} ${d.itemName}`, "#fef08a");
      showToast(`Picked up ${d.itemName} x${d.count}`);

      sendWs({
        type: "PICKUP_ITEM",
        dropId,
      });
    }
  }
}

export function spawnDroppedItem(itemId, count, x, y, customVx = null, customVy = null, droppedBy = state.username) {
  const item = getItemById(itemId);
  if (!item) return;

  const vx = customVx !== null ? customVx : (Math.random() - 0.5) * 4;
  const vy = customVy !== null ? customVy : -4 - Math.random() * 3;

  const drop = {
    id: "drop_" + Math.random().toString(36).substr(2, 9),
    itemId,
    itemName: item.name,
    icon: item.icon,
    count,
    x,
    y,
    vx,
    vy,
    droppedAt: Date.now(),
    droppedBy: droppedBy,
  };

  state.droppedItems.push(drop);

  sendWs({
    type: "DROP_ITEM",
    itemId,
    count,
    x,
    y,
    vx,
    vy,
    itemName: item.name,
    icon: item.icon,
    droppedBy,
  });
}

export function addToBackpackById(itemId, count) {
  const item = getItemById(itemId);
  if (!item) return;

  const existing = state.backpack.find(b => b.item.id === itemId);
  if (existing) {
    existing.count += count;
  } else {
    state.backpack.push({ item, count });
  }
  renderBackpackUI();
}

export function consumeFromBackpack(slotIdx, count) {
  const slot = state.backpack[slotIdx];
  if (!slot) return;
  slot.count -= count;
  if (slot.count <= 0 && slot.item.type !== "tool") {
    state.backpack.splice(slotIdx, 1);
    if (state.selectedSlot >= state.backpack.length) {
      state.selectedSlot = Math.max(0, state.backpack.length - 1);
    }
  }
  renderBackpackUI();
}

export function toggleWearable(item) {
  const isWorn = state.wearables[item.slot] === item.id;
  if (isWorn) {
    state.wearables[item.slot] = null;
    showToast(`Removed ${item.name}`);
  } else {
    state.wearables[item.slot] = item.id;
    showToast(`Equipped ${item.name}!`);
  }
  sound.splice();
  updateEquipmentHUD();

  sendWs({
    type: "UPDATE_WEARABLES",
    wearables: state.wearables,
  });
}

