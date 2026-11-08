/**
 * commands.js — Authentic Growtopia Slash Commands Processor
 */

import { BLOCK_SIZE, WORLD_WIDTH } from "./constants.js";
import { state } from "./state.js";
import { sound } from "./audio.js";
import { spawnDroppedItem, consumeFromBackpack } from "./items.js";
import { addChat, showToast } from "./ui.js";
import { sendWs } from "./network.js";

export function handleSlashCommand(commandText) {
  const parts = commandText.trim().split(" ");
  const cmd = parts[0].toLowerCase();
  const arg1 = parts[1];

  switch (cmd) {
    case "/help":
      addChat("Help", "Commands: /help, /me, /respawn, /kill, /trash <qty>, /drop <qty>, /who, /access <name>, /weather <type>, /kick <name>, /rules, /clear");
      break;

    case "/me": {
      const text = parts.slice(1).join(" ");
      if (text) {
        addChat("•", `${state.username} ${text}`);
        sendWs({ type: "CHAT", message: `• ${state.username} ${text}` });
      }
      break;
    }

    case "/respawn":
    case "/kill": {
      const p = state.player;
      const doorX = Math.floor(WORLD_WIDTH / 2);
      p.x = doorX * BLOCK_SIZE + 4;
      p.y = (24 - 3) * BLOCK_SIZE;
      p.vx = 0;
      p.vy = 0;
      sound.respawn();
      showToast("Respawned at White Door.");
      sendWs({
        type: "MOVE",
        x: p.x,
        y: p.y,
        vx: 0,
        vy: 0,
        facingRight: p.facingRight,
      });
      break;
    }

    case "/trash": {
      const qty = parseInt(arg1) || 1;
      const cur = state.backpack[state.selectedSlot];
      if (!cur) {
        showToast("No item selected in backpack to trash!");
        return;
      }
      if (cur.item.type === "tool") {
        showToast("Cannot trash permanent tools!");
        return;
      }
      const actual = Math.min(qty, cur.count);
      consumeFromBackpack(state.selectedSlot, actual);
      const recycledGems = Math.max(1, Math.floor(actual / 4));
      state.gems += recycledGems;
      document.getElementById("hud-gems").innerText = state.gems;
      sound.gem();
      showToast(`Recycled ${cur.item.name} x${actual} into +${recycledGems} Gems!`);
      addChat("Recycle", `Trashed ${cur.item.name} x${actual} and received +${recycledGems} Gems 💎.`);
      break;
    }

    case "/weather": {
      const mode = (arg1 || "day").toUpperCase();
      if (["DAY", "NIGHT", "SUNSET", "RAIN"].includes(mode)) {
        state.weather = mode;
        showToast(`Weather set to ${mode}!`);
        sendWs({ type: "SET_WEATHER", weather: mode });
      } else {
        showToast("Options: DAY, NIGHT, SUNSET, RAIN");
      }
      break;
    }

    case "/who": {
      const list = [state.username, ...Array.from(state.remotePlayers.keys())];
      addChat("World", `Players here (${list.length}): ${list.join(", ")}`);
      break;
    }

    case "/drop": {
      const qty = parseInt(arg1) || 1;
      const cur = state.backpack[state.selectedSlot];
      if (cur && cur.item.type !== "tool") {
        const actual = Math.min(qty, cur.count);
        consumeFromBackpack(state.selectedSlot, actual);
        const throwVx = (state.player.facingRight ? 1 : -1) * 4.5;
        spawnDroppedItem(cur.item.id, actual, state.player.x + 12, state.player.y + 12, throwVx, -4, state.username);
        showToast(`Dropped ${cur.item.name} x${actual}`);
      }
      break;
    }

    case "/access": {
      if (!arg1) {
        showToast("Usage: /access <username>");
        return;
      }
      if (!state.worldLock || state.worldLock.owner !== state.username) {
        showToast("You must be the world owner to grant access!");
        return;
      }
      sendWs({
        type: "EDIT_WORLD_LOCK",
        action: "addAccess",
        targetUser: arg1.trim(),
      });
      showToast(`Granted world access to ${arg1}!`);
      break;
    }

    case "/kick": {
      if (!arg1) {
        showToast("Usage: /kick <username>");
        return;
      }
      if (!state.worldLock || state.worldLock.owner !== state.username) {
        showToast("You must be the world owner to kick!");
        return;
      }
      sendWs({ type: "KICK_PLAYER", target: arg1.trim() });
      showToast(`Kicked ${arg1}!`);
      break;
    }

    case "/rules":
      if (state.worldLock) {
        addChat("Rules", `World owner: ${state.worldLock.owner}. Be respectful and do not scam.`);
      } else {
        addChat("Rules", "Public world. Anyone can farm and build freely!");
      }
      break;

    case "/clear": {
      state.chatHistory = [];
      const log = document.getElementById("chat-log");
      if (log) log.innerHTML = "";
      break;
    }

    default:
      addChat("System", `Unknown command: ${cmd}. Type /help for a list.`);
      break;
  }
}
