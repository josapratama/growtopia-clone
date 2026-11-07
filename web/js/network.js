/**
 * network.js — WebSocket Multiplayer Protocol & Dispatcher
 */

import { state } from "./state.js";
import { sound } from "./audio.js";
import { addToBackpackById } from "./items.js";
import { syncTradeUI, finalizeTrade, closeTradeModal, openTradeModal } from "./trade.js";
import { showToast, addChat, updateHudLockBadge, showModal, hideAllModals } from "./ui.js";

export function initWebSocket() {
  const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
  const host = window.location.host || "localhost:8080";
  const wsUrl = `${protocol}//${host}`;

  try {
    state.ws = new WebSocket(wsUrl);

    state.ws.onopen = () => {
      console.log("[WebSocket] Connected to Growtopia game server!");
      addChat("System", "Connected to multiplayer game server.");
    };

    state.ws.onmessage = e => {
      try {
        const msg = JSON.parse(e.data);
        handleServerMessage(msg);
      } catch (err) {}
    };

    state.ws.onclose = () => {
      console.log("[WebSocket] Running in offline standalone simulation.");
    };

    state.ws.onerror = () => {
      console.log("[WebSocket] Offline fallback active.");
    };
  } catch (e) {
    console.log("[WebSocket] Offline mode active.");
  }
}

export function sendWs(obj) {
  if (state.ws && state.ws.readyState === WebSocket.OPEN) {
    state.ws.send(JSON.stringify(obj));
  }
}

function handleServerMessage(msg) {
  switch (msg.type) {
    case "WORLD_JOINED": {
      if (Array.isArray(msg.tiles)) {
        state.world.fg = msg.tiles;
        state.world.bg = msg.tiles.map(row => row.map(() => null));
      } else {
        state.world.fg = msg.tiles.fg || [];
        state.world.bg = msg.tiles.bg || [];
      }
      state.world.signs = new Map(Object.entries(msg.signs || {}));
      state.world.doors = new Map(Object.entries(msg.doors || {}));
      state.weather = msg.weather || "DAY";

      state.trees = msg.trees || [];
      state.worldLock = msg.lock || null;
      state.droppedItems = msg.droppedItems || [];
      state.remotePlayers.clear();

      (msg.players || []).forEach(p => {
        if (p.username !== state.username) {
          state.remotePlayers.set(p.username, p);
        }
      });

      updateHudLockBadge();
      const pCountEl = document.getElementById("hud-players");
      if (pCountEl) pCountEl.innerText = state.remotePlayers.size + 1;
      break;
    }

    case "PLAYER_JOINED": {
      if (msg.username !== state.username) {
        state.remotePlayers.set(msg.username, {
          username: msg.username,
          x: msg.x,
          y: msg.y,
          facingRight: true,
          wearables: msg.wearables || {},
        });
        addChat("System", `${msg.username} entered the world.`);
        const pCountEl = document.getElementById("hud-players");
        if (pCountEl) pCountEl.innerText = state.remotePlayers.size + 1;
      }
      break;
    }

    case "PLAYER_MOVED": {
      if (msg.username !== state.username) {
        const p = state.remotePlayers.get(msg.username);
        if (p) {
          p.x = msg.x;
          p.y = msg.y;
          p.facingRight = msg.facingRight;
        }
      }
      break;
    }

    case "TELEPORT": {
      state.player.x = msg.x;
      state.player.y = msg.y;
      state.player.vx = 0;
      state.player.vy = 0;
      sound.door();
      showToast(msg.reason || "Teleported!");
      break;
    }

    case "KICKED": {
      sound.hurt();
      state.screen = "WORLD_SELECT";
      document.getElementById("top-hud").style.display = "none";
      document.getElementById("chat-bar-container").style.display = "none";
      document.getElementById("backpack-bar").style.display = "none";
      hideAllModals();
      showModal("modal-world-select");
      showToast("You were kicked from the world!");
      break;
    }

    case "WEATHER_CHANGED": {
      state.weather = msg.weather;
      showToast(`Weather is now ${msg.weather}`);
      break;
    }

    case "PLAYER_WEARABLES_UPDATED": {
      const p = state.remotePlayers.get(msg.username);
      if (p) p.wearables = msg.wearables;
      break;
    }

    case "PLAYER_LEFT": {
      state.remotePlayers.delete(msg.username);
      addChat("System", `${msg.username} left.`);
      const pCountEl = document.getElementById("hud-players");
      if (pCountEl) pCountEl.innerText = state.remotePlayers.size + 1;
      break;
    }

    case "BLOCK_ACTION": {
      if (!state.world.fg) return;
      if (msg.action === "break") {
        state.world.fg[msg.tileY][msg.tileX] = null;
        sound.breakBlock();
      } else if (msg.action === "place") {
        state.world.fg[msg.tileY][msg.tileX] = msg.blockId;
        sound.placeBlock();
      }
      break;
    }

    case "BG_BLOCK_ACTION": {
      if (!state.world.bg) return;
      if (msg.action === "break") {
        state.world.bg[msg.tileY][msg.tileX] = null;
        sound.breakBlock();
      } else if (msg.action === "place") {
        state.world.bg[msg.tileY][msg.tileX] = msg.blockId;
        sound.placeBlock();
      }
      break;
    }

    case "SIGN_UPDATED": {
      state.world.signs.set(`${msg.tileX},${msg.tileY}`, {
        text: msg.text,
        owner: msg.owner,
      });
      break;
    }

    case "DOOR_UPDATED": {
      state.world.doors.set(`${msg.tileX},${msg.tileY}`, {
        targetWorld: msg.targetWorld,
        label: msg.label,
      });
      break;
    }

    case "TREE_PLANTED": {
      state.trees = (state.trees || []).filter(t => !(t.x === msg.tree.x && t.y === msg.tree.y));
      state.trees.push(msg.tree);
      sound.placeBlock();
      break;
    }

    case "TREE_HARVESTED": {
      state.trees = (state.trees || []).filter(t => !(t.x === msg.tileX && t.y === msg.tileY));
      sound.breakBlock();
      break;
    }

    case "WORLD_LOCK_UPDATED": {
      state.worldLock = msg.lock;
      updateHudLockBadge();
      sound.lock();
      showToast(msg.lock ? `World locked by ${msg.lock.owner}!` : "World lock was removed!");
      break;
    }

    case "ITEM_DROPPED": {
      state.droppedItems = state.droppedItems || [];
      state.droppedItems.push(msg.item);
      sound.drop();
      break;
    }

    case "ITEM_PICKED_UP": {
      state.droppedItems = (state.droppedItems || []).filter(d => d.id !== msg.dropId);
      break;
    }

    case "ITEM_COLLECTED_SELF": {
      sound.gem();
      addToBackpackById(msg.item.itemId, msg.item.count);
      showToast(`Collected ${msg.item.itemName} x${msg.item.count}`);
      break;
    }

    case "WORLD_PROTECTION_ALERT": {
      showToast(msg.message);
      sound.punch();
      break;
    }

    case "CHAT_MESSAGE": {
      addChat(msg.username, msg.message);
      state.chatBubbles.set(msg.username, { text: msg.message, timer: 240 });
      break;
    }

    case "TRADE_INVITE": {
      if (confirm(`${msg.from} wants to trade with you! Accept?`)) {
        openTradeModal(msg.tradeId, msg.from);
      }
      break;
    }

    case "TRADE_STATE_SYNC": {
      syncTradeUI(msg.trade);
      break;
    }

    case "TRADE_COMPLETED": {
      finalizeTrade(msg.trade);
      break;
    }

    case "TRADE_CANCELLED": {
      closeTradeModal();
      showToast(`Trade cancelled by ${msg.by}.`);
      break;
    }
  }
}
