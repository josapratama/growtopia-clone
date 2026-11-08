/**
 * ui.js — User Interface, DOM Modals, Hotbars & Notifications
 */

import { ITEMS, getItemById } from "./constants.js";
import { state } from "./state.js";
import { sound } from "./audio.js";
import { addToBackpackById, toggleWearable } from "./items.js";

export function showToast(message) {
  const container = document.getElementById("toast-overlay");
  if (!container) return;
  const toast = document.createElement("div");
  toast.className = "toast-message";
  toast.innerText = message;
  container.appendChild(toast);
  setTimeout(() => {
    toast.style.transition = "opacity 0.4s";
    toast.style.opacity = "0";
    setTimeout(() => toast.remove(), 400);
  }, 2400);
}

export function addChat(user, text) {
  state.chatHistory.push({ user, text });
  if (state.chatHistory.length > 30) state.chatHistory.shift();

  const log = document.getElementById("chat-log");
  if (!log) return;
  log.innerHTML = "";
  state.chatHistory.slice(-5).forEach(c => {
    const el = document.createElement("div");
    el.className = "chat-entry";
    el.innerHTML = `<span class="chat-user">${c.user}: </span><span class="chat-text">${c.text}</span>`;
    log.appendChild(el);
  });
  log.scrollTop = log.scrollHeight;
}

export function renderBackpackUI() {
  const bar = document.getElementById("backpack-bar");
  if (!bar) return;
  bar.innerHTML = "";

  state.backpack.forEach((b, idx) => {
    const slot = document.createElement("div");
    slot.className = `hotbar-slot ${idx === state.selectedSlot ? "selected" : ""}`;
    slot.innerHTML = `
      <span class="slot-hotkey">${idx + 1 <= 9 ? idx + 1 : ""}</span>
      <span class="slot-icon">${b.item.icon}</span>
      <span class="slot-count">${b.item.type === "tool" ? "" : b.count}</span>
    `;

    slot.addEventListener("click", () => {
      if (state.selectedSlot === idx && b.item.type !== "tool") {
        openItemActionModal(idx);
      } else {
        state.selectedSlot = idx;
        renderBackpackUI();
      }
    });

    bar.appendChild(slot);
  });
}

export function openItemActionModal(slotIdx) {
  state.activeItemActionIdx = slotIdx;
  const b = state.backpack[slotIdx];
  if (!b) return;

  const item = b.item;
  document.getElementById("item-action-title").innerText = item.name;
  document.getElementById("item-action-icon").innerText = item.icon;
  document.getElementById("item-action-name").innerText = item.name;
  document.getElementById("item-action-type").innerText = `Category: ${item.type.toUpperCase()}`;
  document.getElementById("item-action-count").innerText = `Available: ${b.count}`;
  document.getElementById("item-action-desc").innerText = item.desc || "Use to build, farm, or customize your character.";

  const slider = document.getElementById("drop-qty-slider");
  const qtyVal = document.getElementById("drop-qty-val");
  slider.max = b.count;
  slider.value = 1;
  qtyVal.innerText = 1;

  slider.oninput = () => {
    qtyVal.innerText = slider.value;
  };

  const equipBtn = document.getElementById("btn-equip-toggle");
  if (item.type === "wearable" || (item.type === "tool" && item.slot === "hand")) {
    equipBtn.style.display = "block";
    const slotKey = item.slot || "hand";
    const isWorn = state.wearables[slotKey] === item.id;
    equipBtn.innerText = isWorn ? "Unwear" : "Wear / Equip";
    equipBtn.onclick = () => {
      toggleWearable(item);
      closeItemActionModal();
    };
  } else {
    equipBtn.style.display = "none";
  }

  showModal("modal-item-action");
}

export function closeItemActionModal() {
  hideModal("modal-item-action");
  state.activeItemActionIdx = null;
}

export function updateEquipmentHUD() {
  const hatSlot = document.querySelector("#equip-hat span");
  const shirtSlot = document.querySelector("#equip-shirt span");
  const wingsSlot = document.querySelector("#equip-wings span");

  const hatItem = getItemById(state.wearables.hat);
  const shirtItem = getItemById(state.wearables.shirt);
  const wingsItem = getItemById(state.wearables.wings);

  if (hatSlot) hatSlot.innerText = hatItem ? hatItem.name : "None";
  if (shirtSlot) shirtSlot.innerText = shirtItem ? shirtItem.name : "None";
  if (wingsSlot) wingsSlot.innerText = wingsItem ? wingsItem.name : "None";
}

export function updateHudLockBadge() {
  const badge = document.getElementById("hud-lock-badge");
  const icon = document.getElementById("hud-lock-icon");
  const owner = document.getElementById("hud-lock-owner");
  if (!badge) return;

  if (state.worldLock) {
    icon.innerText = "🔒";
    owner.innerText = state.worldLock.owner;
    badge.style.borderColor = "#facc15";
    badge.style.color = "#fef08a";
  } else {
    icon.innerText = "🔓";
    owner.innerText = "Public";
    badge.style.borderColor = "#78716c";
    badge.style.color = "#cbd5e1";
  }
}

// ─────────────────────────────────────────────────────────────
// SHOP ITEMS CATALOG
// ─────────────────────────────────────────────────────────────
const SHOP_ITEMS = [
  { name: "World Lock", price: 2000, itemId: ITEMS.WORLD_LOCK.id, count: 1, icon: "🔒", desc: "Locks an entire world for you and your friends!" },
  { name: "Small Lock", price: 50, itemId: ITEMS.SMALL_LOCK.id, count: 1, icon: "🔐", desc: "Locks 10 tiles area." },
  { name: "Big Lock", price: 200, itemId: ITEMS.BIG_LOCK.id, count: 1, icon: "🔏", desc: "Locks 48 tiles area." },
  { name: "Pickaxe", price: 150, itemId: ITEMS.PICKAXE.id, count: 1, icon: "⛏️", desc: "Breaks rock and stone blocks with fewer hits!" },
  { name: "Golden Pickaxe", price: 350, itemId: ITEMS.GOLDEN_PICKAXE.id, count: 1, icon: "⛏️✨", desc: "Forged in pure gold! Breaks blocks 3x faster than fists." },
  { name: "Basic Splicing Pack", price: 100, itemId: ITEMS.SEED_DIRT.id, count: 20, icon: "🌱", desc: "Contains 20 Dirt Seeds & 10 Rock Seeds for splicing." },
  { name: "White Door Seeds", price: 80, itemId: ITEMS.SEED_DOOR.id, count: 5, icon: "🌱🚪", desc: "Seeds that blossom into White Doors." },
  { name: "Wallpaper Pack", price: 80, itemId: ITEMS.DIRT_BG.id, count: 50, icon: "🟫", desc: "Contains 50 Dirt BG & 20 Cave BG wallpapers." },
  { name: "Angel Wings", price: 500, itemId: ITEMS.ANGEL_WINGS.id, count: 1, icon: "🪽", desc: "Majestic wings that allow gliding and double jumps!" },
  { name: "Crimson Jetpack", price: 650, itemId: ITEMS.JETPACK.id, count: 1, icon: "🚀", desc: "High-power jet thruster allowing double jump and sustained glide!" },
  { name: "Royal Outfit Pack", price: 600, itemId: ITEMS.ROYAL_CROWN.id, count: 1, icon: "👑", desc: "Contains Royal Crown and Royal Robe!" },
  { name: "Weather - Night", price: 400, itemId: ITEMS.WEATHER_NIGHT.id, count: 1, icon: "🌙", desc: "Starry dark night with glowing moon." },
  { name: "Weather - Rain", price: 400, itemId: ITEMS.WEATHER_RAIN.id, count: 1, icon: "🌧️", desc: "Atmospheric rainfall across the world." },
  { name: "Devil Horns", price: 300, itemId: ITEMS.DEVIL_HORNS.id, count: 1, icon: "😈", desc: "Spooky fiery red horns!" },
  { name: "Ninja Hood", price: 350, itemId: ITEMS.NINJA_HOOD.id, count: 1, icon: "🥷", desc: "Stealthy ninja disguise!" },
];

export function populateShop() {
  const container = document.getElementById("shop-items-list");
  if (!container) return;
  container.innerHTML = "";

  document.getElementById("shop-gems-count").innerText = state.gems;

  SHOP_ITEMS.forEach(pack => {
    const card = document.createElement("div");
    card.style.background = "#1c1917";
    card.style.border = "2px solid #57534e";
    card.style.borderRadius = "8px";
    card.style.padding = "8px";
    card.style.display = "flex";
    card.style.flexDirection = "column";
    card.style.gap = "4px";

    card.innerHTML = `
      <div style="display: flex; align-items: center; gap: 8px;">
        <span style="font-size: 20px;">${pack.icon}</span>
        <div>
          <strong style="color: #f8fafc; font-size: 9px;">${pack.name}</strong>
          <p style="color: #facc15; font-size: 8px; margin: 2px 0 0 0;">💎 ${pack.price} Gems</p>
        </div>
      </div>
      <p style="color: #94a3b8; font-size: 7px; margin-top: 4px; line-height: 1.4;">${pack.desc}</p>
      <button class="gt-btn green" style="margin-top: auto; padding: 4px 8px; font-size: 8px;">Buy Pack</button>
    `;

    card.querySelector("button").onclick = () => {
      if (state.gems >= pack.price) {
        state.gems -= pack.price;
        document.getElementById("hud-gems").innerText = state.gems;
        document.getElementById("shop-gems-count").innerText = state.gems;

        addToBackpackById(pack.itemId, pack.count);
        if (pack.name === "Basic Splicing Pack") {
          addToBackpackById(ITEMS.SEED_ROCK.id, 10);
        } else if (pack.name === "Wallpaper Pack") {
          addToBackpackById(ITEMS.CAVE_BG.id, 20);
        } else if (pack.name === "Royal Outfit Pack") {
          addToBackpackById(ITEMS.ROYAL_ROBE.id, 1);
        }

        sound.splice();
        showToast(`Purchased ${pack.name}!`);
      } else {
        showToast("Not enough gems!");
        sound.punch();
      }
    };

    container.appendChild(card);
  });
}

export function showModal(id) {
  const el = document.getElementById(id);
  if (el) el.style.display = "flex";
}

export function hideModal(id) {
  const el = document.getElementById(id);
  if (el) el.style.display = "none";
}

export function hideAllModals() {
  document.querySelectorAll(".modal-overlay").forEach(m => (m.style.display = "none"));
}
