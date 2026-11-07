/**
 * main.js — Main Game Orchestrator & Input Controller
 */

import { BLOCK_SIZE, WORLD_WIDTH, ITEMS, getItemById, INTERACTION_REACH } from "./constants.js";
import { state, addXP } from "./state.js";
import { sound } from "./audio.js";
import { generateWorld } from "./world.js";
import { updatePhysics } from "./physics.js";
import { render } from "./renderer.js";
import { handleSeedPlanting, handleHarvestTree } from "./farming.js";
import { handleLockPlacement, canModifyTile } from "./locks.js";
import { updateDroppedItems, spawnDroppedItem, consumeFromBackpack } from "./items.js";
import { updateParticles, addBlockDebris } from "./particles.js";
import { startTradeWith } from "./trade.js";
import { handleSlashCommand } from "./commands.js";
import { initWebSocket, sendWs } from "./network.js";
import {
  showToast,
  addChat,
  renderBackpackUI,
  updateEquipmentHUD,
  updateHudLockBadge,
  populateShop,
  showModal,
  hideModal,
  hideAllModals,
  closeItemActionModal,
} from "./ui.js";

const canvas = document.getElementById("game-canvas");
const ctx = canvas.getContext("2d");

// ─────────────────────────────────────────────────────────────
// WORLD ENTER & LEAVE
// ─────────────────────────────────────────────────────────────
export function enterWorld(worldName) {
  state.currentWorldName = worldName.toUpperCase().trim() || "MAIN";
  state.world = generateWorld(state.currentWorldName);
  state.trees = [];
  state.worldLock = null;
  state.droppedItems = [];
  state.blockDamage.clear();
  state.remotePlayers.clear();
  state.weather = "DAY";

  // Position player directly at White Door
  const doorX = Math.floor(WORLD_WIDTH / 2);
  state.player.x = doorX * BLOCK_SIZE + 4;
  state.player.y = (24 - 3) * BLOCK_SIZE;
  state.player.vx = 0;
  state.player.vy = 0;

  sound.door();

  // Update HUD
  document.getElementById("hud-world-name").innerText = state.currentWorldName;
  document.getElementById("hud-gems").innerText = state.gems;
  document.getElementById("hud-players").innerText = 1;
  const lvlEl = document.getElementById("hud-level");
  if (lvlEl) lvlEl.innerText = `Lv. ${state.level}`;

  updateHudLockBadge();
  updateEquipmentHUD();

  // Show In-Game HUDs
  document.getElementById("top-hud").style.display = "flex";
  document.getElementById("chat-bar-container").style.display = "flex";
  document.getElementById("backpack-bar").style.display = "flex";
  hideAllModals();

  state.screen = "WORLD_VIEW";
  renderBackpackUI();

  addChat("World", `Welcome to ${state.currentWorldName}! Dual-layer & Tools active.`);

  sendWs({
    type: "JOIN_WORLD",
    world: state.currentWorldName,
    username: state.username,
    x: state.player.x,
    y: state.player.y,
    wearables: state.wearables,
  });
}

export function leaveWorld() {
  sound.door();
  state.screen = "WORLD_SELECT";
  document.getElementById("top-hud").style.display = "none";
  document.getElementById("chat-bar-container").style.display = "none";
  document.getElementById("backpack-bar").style.display = "none";
  showModal("modal-world-select");
}

// ─────────────────────────────────────────────────────────────
// TILE INTERACTION CONTROLLER
// ─────────────────────────────────────────────────────────────
function handleBlockClick(tileX, tileY) {
  if (tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= 60) return;

  const p = state.player;
  const playerCenterX = p.x + p.w / 2;
  const playerCenterY = p.y + p.h / 2;
  const targetX = tileX * BLOCK_SIZE + BLOCK_SIZE / 2;
  const targetY = tileY * BLOCK_SIZE + BLOCK_SIZE / 2;
  const dist = Math.hypot(playerCenterX - targetX, playerCenterY - targetY);

  if (dist > INTERACTION_REACH) {
    showToast("Too far away!");
    return;
  }

  // Check World Lock build permissions
  if (!canModifyTile(tileX, tileY)) {
    showToast(`World is locked by ${state.worldLock.owner}!`);
    sound.punch();
    return;
  }

  const selected = state.backpack[state.selectedSlot];
  if (!selected) return;
  const item = selected.item;

  p.punching = true;
  p.punchTimer = 10;

  // 1. Wrench: Inspect Tile, Tree, Sign, Door, or Player
  if (item.id === ITEMS.WRENCH.id) {
    handleWrenchClick(tileX, tileY);
    return;
  }

  // 2. Fist / Punch / Pickaxe: Break Block or Harvest Tree
  if (item.id === ITEMS.FIST.id || item.id === ITEMS.PICKAXE.id) {
    handlePunchClick(tileX, tileY, item);
    return;
  }

  // 3. Lock Placement
  if (item.type === "lock") {
    handleLockPlacement(tileX, tileY, item);
    return;
  }

  // 4. Seed Planting
  if (item.type === "seed") {
    handleSeedPlanting(tileX, tileY, item);
    return;
  }

  // 5. Background Wallpaper Placement
  if (item.type === "background") {
    handleBackgroundPlacement(tileX, tileY, item);
    return;
  }

  // 6. Block / Door / Platform / Sign / Weather Placement
  if (item.type === "block" || item.type === "door" || item.type === "platform" || item.type === "sign") {
    handleBlockPlacement(tileX, tileY, item);
    return;
  }
}

function handlePunchClick(tileX, tileY, toolItem) {
  // Check tree first
  const treeIdx = state.trees.findIndex(t => t.x === tileX && t.y === tileY);
  if (treeIdx !== -1) {
    handleHarvestTree(tileX, tileY, treeIdx);
    return;
  }

  // Determine hit power bonus (Pickaxe breaks faster)
  const isPickaxe = toolItem?.id === ITEMS.PICKAXE.id || state.wearables.hand === ITEMS.PICKAXE.id;
  const hitDamage = isPickaxe ? 2 : 1;

  // 1. Check Foreground Block
  const fgBlockId = state.world.fg[tileY]?.[tileX];
  if (fgBlockId !== null && fgBlockId !== undefined) {
    if (fgBlockId === ITEMS.BEDROCK.id) {
      showToast("Bedrock cannot be destroyed!");
      sound.punch();
      return;
    }

    const key = `fg_${tileX},${tileY}`;
    const currentHits = (state.blockDamage.get(key) || 0) + hitDamage;
    const blockItem = getItemById(fgBlockId);
    const neededHits = blockItem ? blockItem.hits : 3;

    if (currentHits >= neededHits) {
      // Destroy FG block
      state.world.fg[tileY][tileX] = null;
      state.blockDamage.delete(key);
      sound.breakBlock();
      addBlockDebris(tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16, blockItem ? blockItem.color : "#8b4513");

      spawnDroppedItem(fgBlockId, 1, tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16);
      if (blockItem && blockItem.seedId && Math.random() < 0.35) {
        spawnDroppedItem(blockItem.seedId, 1, tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16);
      }
      const gemVal = Math.floor(1 + Math.random() * 3);
      state.gems += gemVal;
      document.getElementById("hud-gems").innerText = state.gems;
      addXP(blockItem ? blockItem.xp : 5);

      sendWs({
        type: "BLOCK_ACTION",
        tileX,
        tileY,
        action: "break",
        blockId: fgBlockId,
      });
    } else {
      state.blockDamage.set(key, currentHits);
      sound.punch();
    }
    return;
  }

  // 2. Check Background Wallpaper (if FG is empty)
  const bgBlockId = state.world.bg[tileY]?.[tileX];
  if (bgBlockId !== null && bgBlockId !== undefined) {
    const key = `bg_${tileX},${tileY}`;
    const currentHits = (state.blockDamage.get(key) || 0) + hitDamage;
    const bgItem = getItemById(bgBlockId);
    const neededHits = bgItem ? bgItem.hits : 2;

    if (currentHits >= neededHits) {
      // Destroy BG wallpaper
      state.world.bg[tileY][tileX] = null;
      state.blockDamage.delete(key);
      sound.breakBlock();
      addBlockDebris(tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16, bgItem ? bgItem.color : "#542a0c");

      spawnDroppedItem(bgBlockId, 1, tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16);
      addXP(bgItem ? bgItem.xp : 3);

      sendWs({
        type: "BG_BLOCK_ACTION",
        tileX,
        tileY,
        action: "break",
        blockId: bgBlockId,
      });
    } else {
      state.blockDamage.set(key, currentHits);
      sound.punch();
    }
  }
}

function handleBackgroundPlacement(tileX, tileY, item) {
  if (state.world.bg[tileY]?.[tileX] !== null) {
    showToast("Background already placed here!");
    return;
  }

  consumeFromBackpack(state.selectedSlot, 1);
  state.world.bg[tileY][tileX] = item.id;
  sound.placeBlock();

  sendWs({
    type: "BG_BLOCK_ACTION",
    tileX,
    tileY,
    action: "place",
    blockId: item.id,
  });
}

function handleBlockPlacement(tileX, tileY, item) {
  if (state.world.fg[tileY]?.[tileX] !== null) {
    showToast("Tile already occupied!");
    return;
  }

  // Prevent placing solid block inside player
  const p = state.player;
  const tilePixelX = tileX * BLOCK_SIZE;
  const tilePixelY = tileY * BLOCK_SIZE;
  const overlap =
    p.x < tilePixelX + BLOCK_SIZE &&
    p.x + p.w > tilePixelX &&
    p.y < tilePixelY + BLOCK_SIZE &&
    p.y + p.h > tilePixelY;

  if (overlap && item.solid) {
    showToast("Cannot place block inside yourself!");
    return;
  }

  consumeFromBackpack(state.selectedSlot, 1);
  state.world.fg[tileY][tileX] = item.id;
  sound.placeBlock();

  // If Weather Machine, activate weather immediately
  if (item.weather) {
    state.weather = item.weather;
    showToast(`Weather changed to ${item.weather}!`);
  }

  // If Sign, initialize default message
  if (item.id === ITEMS.SIGN.id) {
    state.world.signs.set(`${tileX},${tileY}`, {
      text: "Welcome to my world!",
      owner: state.username,
    });
  }

  // If Door, initialize default door
  if (item.id === ITEMS.DOOR.id) {
    state.world.doors.set(`${tileX},${tileY}`, {
      targetWorld: "MAIN",
      label: "Door",
    });
  }

  sendWs({
    type: "BLOCK_ACTION",
    tileX,
    tileY,
    action: "place",
    blockId: item.id,
  });
}

function handleWrenchClick(tileX, tileY) {
  const clickWorldX = tileX * BLOCK_SIZE + 16;
  const clickWorldY = tileY * BLOCK_SIZE + 16;

  // 1. Wrench Player -> Profile & Trade
  for (const [uname, p] of state.remotePlayers.entries()) {
    const dist = Math.hypot(p.x + 12 - clickWorldX, p.y + 15 - clickWorldY);
    if (dist < 40) {
      openPlayerProfileModal(uname);
      return;
    }
  }

  // 2. Wrench World Lock
  if (state.worldLock && state.worldLock.tileX === tileX && state.worldLock.tileY === tileY) {
    openWorldLockModal();
    return;
  }

  // 3. Wrench Door -> Configure Destination & Label
  const blockId = state.world.fg[tileY]?.[tileX];
  if (blockId === ITEMS.DOOR.id) {
    openDoorModal(tileX, tileY);
    return;
  }

  // 4. Wrench Sign -> Edit Message
  if (blockId === ITEMS.SIGN.id) {
    openSignModal(tileX, tileY);
    return;
  }

  // 5. Wrench Tree
  const tree = state.trees.find(t => t.x === tileX && t.y === tileY);
  if (tree) {
    const seedItem = getItemById(tree.seedId);
    const elapsed = (Date.now() - tree.plantedAt) / 1000;
    const remaining = Math.max(0, Math.ceil(tree.growTime - elapsed));
    alert(`🌳 ${seedItem ? seedItem.name : "Tree"}\nStatus: ${remaining > 0 ? `${remaining}s until mature` : "Ready to harvest! Punch to collect loot."}`);
    return;
  }

  if (blockId !== null && blockId !== undefined) {
    const item = getItemById(blockId);
    alert(`🧱 ${item ? item.name : "Block"}\nType: ${item ? item.type : "Unknown"}`);
  }
}

// ─────────────────────────────────────────────────────────────
// DIALOG HELPERS (Profile, Door, Sign, World Lock)
// ─────────────────────────────────────────────────────────────
function openPlayerProfileModal(uname) {
  const modal = document.getElementById("modal-player-profile");
  if (!modal) return;
  document.getElementById("profile-username").innerText = uname;
  document.getElementById("profile-level").innerText = "Growtopian Explorer";

  document.getElementById("btn-profile-trade").onclick = () => {
    hideModal("modal-player-profile");
    startTradeWith(uname);
  };
  document.getElementById("btn-profile-pull").onclick = () => {
    if (!state.worldLock || state.worldLock.owner !== state.username) {
      showToast("Only world owner can pull players!");
      return;
    }
    sendWs({ type: "PULL_PLAYER", target: uname, x: state.player.x, y: state.player.y });
    showToast(`Pulled ${uname}!`);
    hideModal("modal-player-profile");
  };

  showModal("modal-player-profile");
}

function openDoorModal(tileX, tileY) {
  state.activeDoorTile = { tileX, tileY };
  const key = `${tileX},${tileY}`;
  const door = state.world.doors?.get(key) || { targetWorld: "MAIN", label: "Door" };

  const targetInput = document.getElementById("door-target-input");
  const labelInput = document.getElementById("door-label-input");
  if (targetInput) targetInput.value = door.targetWorld || "MAIN";
  if (labelInput) labelInput.value = door.label || "";

  showModal("modal-door");
}

function openSignModal(tileX, tileY) {
  state.activeSignTile = { tileX, tileY };
  const key = `${tileX},${tileY}`;
  const sign = state.world.signs.get(key) || { text: "", owner: state.username };

  const input = document.getElementById("sign-text-input");
  if (input) input.value = sign.text || "";
  showModal("modal-sign");
}

function openWorldLockModal() {
  if (!state.worldLock) return;
  document.getElementById("wl-owner-name").innerText = state.worldLock.owner;

  const accessListEl = document.getElementById("wl-access-list");
  accessListEl.innerHTML = "";
  if (!state.worldLock.accessList || state.worldLock.accessList.length === 0) {
    accessListEl.innerText = "No other players have access.";
  } else {
    state.worldLock.accessList.forEach(uname => {
      const row = document.createElement("div");
      row.style.display = "flex";
      row.style.justifyContent = "space-between";
      row.style.padding = "2px 0";
      row.innerHTML = `<span>👤 ${uname}</span> <button class="gt-btn red" style="padding: 2px 4px; font-size: 7px;">Remove</button>`;
      row.querySelector("button").onclick = () => {
        sendWs({
          type: "EDIT_WORLD_LOCK",
          action: "removeAccess",
          targetUser: uname,
        });
      };
      accessListEl.appendChild(row);
    });
  }

  showModal("modal-world-lock");
}

// ─────────────────────────────────────────────────────────────
// INPUT LISTENERS & DOM EVENT HOOKS
// ─────────────────────────────────────────────────────────────
function initInputListeners() {
  window.addEventListener("keydown", e => {
    sound.init(); // Activate Web Audio Context on gesture
    if (e.target.tagName === "INPUT" || e.target.tagName === "TEXTAREA") return;

    if (e.key === "ArrowLeft" || e.key === "a" || e.key === "A") state.keys.left = true;
    if (e.key === "ArrowRight" || e.key === "d" || e.key === "D") state.keys.right = true;
    if (e.key === "ArrowUp" || e.key === "w" || e.key === "W" || e.key === " ") state.keys.jump = true;

    // Number hotkeys 1-9
    const num = parseInt(e.key);
    if (!isNaN(num) && num >= 1 && num <= state.backpack.length) {
      state.selectedSlot = num - 1;
      renderBackpackUI();
    }

    // Door Entry on Up / Enter
    if (e.key === "Enter" || e.key === "w" || e.key === "ArrowUp") {
      const playerTileX = Math.floor((state.player.x + 12) / BLOCK_SIZE);
      const playerTileY = Math.floor((state.player.y + 15) / BLOCK_SIZE);
      const doorKey = `${playerTileX},${playerTileY}`;

      if (state.world.fg[playerTileY]?.[playerTileX] === ITEMS.DOOR.id) {
        const door = state.world.doors?.get(doorKey) || { targetWorld: "MAIN" };
        if (door.targetWorld === "EXIT") {
          leaveWorld();
        } else {
          enterWorld(door.targetWorld);
        }
        return;
      }
    }

    if (e.key === "Enter") {
      const chatInput = document.getElementById("chat-input");
      if (chatInput && document.activeElement !== chatInput) {
        chatInput.focus();
        e.preventDefault();
      }
    }
  });

  window.addEventListener("keyup", e => {
    if (e.target.tagName === "INPUT" || e.target.tagName === "TEXTAREA") return;
    if (e.key === "ArrowLeft" || e.key === "a" || e.key === "A") state.keys.left = false;
    if (e.key === "ArrowRight" || e.key === "d" || e.key === "D") state.keys.right = false;
    if (e.key === "ArrowUp" || e.key === "w" || e.key === "W" || e.key === " ") state.keys.jump = false;
  });

  canvas.addEventListener("mousedown", e => {
    sound.init();
    if (state.screen !== "WORLD_VIEW") return;
    const rect = canvas.getBoundingClientRect();
    const scaleX = canvas.width / rect.width;
    const scaleY = canvas.height / rect.height;

    const canvasX = (e.clientX - rect.left) * scaleX;
    const canvasY = (e.clientY - rect.top) * scaleY;

    const worldX = canvasX + state.camera.x;
    const worldY = canvasY + state.camera.y;

    const tileX = Math.floor(worldX / BLOCK_SIZE);
    const tileY = Math.floor(worldY / BLOCK_SIZE);

    handleBlockClick(tileX, tileY);
  });

  // Chat Input Listener
  const chatInput = document.getElementById("chat-input");
  chatInput.addEventListener("keydown", e => {
    if (e.key === "Enter") {
      const text = chatInput.value.trim();
      if (text) {
        if (text.startsWith("/")) {
          handleSlashCommand(text);
        } else {
          addChat(state.username, text);
          state.chatBubbles.set(state.username, { text, timer: 240 });
          sendWs({
            type: "CHAT",
            message: text,
          });
        }
        chatInput.value = "";
      }
      chatInput.blur();
    }
  });

  // DOM Modals Event Hooks
  document.getElementById("menu-btn-play").onclick = () => {
    hideAllModals();
    showModal("modal-login");
  };
  document.getElementById("menu-btn-guest").onclick = () => {
    hideAllModals();
    showModal("modal-world-select");
  };
  document.getElementById("login-close").onclick = () => {
    hideAllModals();
    showModal("modal-main-menu");
  };
  document.getElementById("login-form").onsubmit = e => {
    e.preventDefault();
    state.username = document.getElementById("login-username").value.trim() || state.username;
    hideAllModals();
    showModal("modal-world-select");
  };
  document.getElementById("world-search-form").onsubmit = e => {
    e.preventDefault();
    const wName = document.getElementById("world-search-input").value.trim();
    enterWorld(wName);
  };
  document.getElementById("btn-exit-world").onclick = leaveWorld;
  document.getElementById("btn-shop").onclick = () => {
    populateShop();
    showModal("modal-shop");
  };
  document.getElementById("shop-close").onclick = () => hideModal("modal-shop");

  // Door Modal
  const doorClose = document.getElementById("door-close");
  if (doorClose) doorClose.onclick = () => hideModal("modal-door");
  const doorSave = document.getElementById("door-save-btn");
  if (doorSave) {
    doorSave.onclick = () => {
      if (state.activeDoorTile) {
        const targetWorld = document.getElementById("door-target-input").value.trim() || "MAIN";
        const label = document.getElementById("door-label-input").value.trim();
        const { tileX, tileY } = state.activeDoorTile;
        state.world.doors.set(`${tileX},${tileY}`, { targetWorld, label });
        sendWs({ type: "UPDATE_DOOR", tileX, tileY, targetWorld, label });
        showToast(`Door leads to ${targetWorld}!`);
      }
      hideModal("modal-door");
    };
  }

  // Sign Modal
  const signClose = document.getElementById("sign-close");
  if (signClose) signClose.onclick = () => hideModal("modal-sign");
  const signSave = document.getElementById("sign-save-btn");
  if (signSave) {
    signSave.onclick = () => {
      if (state.activeSignTile) {
        const text = document.getElementById("sign-text-input").value.trim();
        const { tileX, tileY } = state.activeSignTile;
        state.world.signs.set(`${tileX},${tileY}`, { text, owner: state.username });
        sendWs({ type: "UPDATE_SIGN", tileX, tileY, text });
        showToast("Sign updated!");
      }
      hideModal("modal-sign");
    };
  }

  // Profile Modal Close
  const profileClose = document.getElementById("profile-close");
  if (profileClose) profileClose.onclick = () => hideModal("modal-player-profile");

  // World Lock Actions
  document.getElementById("world-lock-close").onclick = () => hideModal("modal-world-lock");
  document.getElementById("wl-done-btn").onclick = () => hideModal("modal-world-lock");
  document.getElementById("wl-add-access-btn").onclick = () => {
    const target = document.getElementById("wl-add-access-input").value.trim();
    if (target) {
      sendWs({
        type: "EDIT_WORLD_LOCK",
        action: "addAccess",
        targetUser: target,
      });
      document.getElementById("wl-add-access-input").value = "";
      hideModal("modal-world-lock");
    }
  };
  document.getElementById("wl-break-btn").onclick = () => {
    if (confirm("Are you sure you want to break and reclaim this World Lock?")) {
      sendWs({
        type: "EDIT_WORLD_LOCK",
        action: "breakLock",
      });
      consumeFromBackpack(state.selectedSlot, 0); // Refresh
      hideModal("modal-world-lock");
    }
  };

  // Item Action Modal
  document.getElementById("item-action-close").onclick = closeItemActionModal;
  document.getElementById("btn-item-cancel").onclick = closeItemActionModal;
  document.getElementById("btn-drop-item").onclick = () => {
    if (state.activeItemActionIdx !== null) {
      const b = state.backpack[state.activeItemActionIdx];
      const slider = document.getElementById("drop-qty-slider");
      const dropCount = parseInt(slider.value) || 1;
      const actual = Math.min(dropCount, b.count);

      consumeFromBackpack(state.activeItemActionIdx, actual);
      spawnDroppedItem(b.item.id, actual, state.player.x + 12, state.player.y + 12);
      closeItemActionModal();
    }
  };

  // Trade Modal
  document.getElementById("trade-close").onclick = () => {
    if (state.activeTrade) sendWs({ type: "TRADE_CANCEL", tradeId: state.activeTrade.id });
    hideModal("modal-trade");
  };
  document.getElementById("btn-trade-cancel").onclick = () => {
    if (state.activeTrade) sendWs({ type: "TRADE_CANCEL", tradeId: state.activeTrade.id });
    hideModal("modal-trade");
  };

  // Mobile Touch Controls
  const setupTouch = (id, key) => {
    const el = document.getElementById(id);
    if (!el) return;
    el.addEventListener("touchstart", e => {
      e.preventDefault();
      sound.init();
      state.keys[key] = true;
    });
    el.addEventListener("touchend", e => {
      e.preventDefault();
      state.keys[key] = false;
    });
  };
  setupTouch("touch-left", "left");
  setupTouch("touch-right", "right");
  setupTouch("touch-jump", "jump");

  // Popular Worlds Grid
  const popularWorlds = ["MAIN", "START", "TRADE", "FARM", "PARKOUR", "CASINO"];
  const popGrid = document.getElementById("popular-worlds-grid");
  if (popGrid) {
    popGrid.innerHTML = "";
    popularWorlds.forEach(name => {
      const card = document.createElement("div");
      card.className = "world-card";
      card.innerHTML = `<strong>🌍 ${name}</strong><br><span style="color: #4ade80;">Active</span>`;
      card.onclick = () => enterWorld(name);
      popGrid.appendChild(card);
    });
  }
}

// ─────────────────────────────────────────────────────────────
// MAIN GAME LOOP
// ─────────────────────────────────────────────────────────────
function gameLoop() {
  if (state.screen === "WORLD_VIEW") {
    updatePhysics(canvas);
    updateDroppedItems();
    updateParticles();
  }
  render(ctx, canvas);
  requestAnimationFrame(gameLoop);
}

// Start Engine
initWebSocket();
initInputListeners();
showModal("modal-main-menu");
requestAnimationFrame(gameLoop);
