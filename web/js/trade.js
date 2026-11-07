/**
 * trade.js — 2-Step Secure Trade System with Item Offering
 */

import { state } from "./state.js";
import { sound } from "./audio.js";
import { getItemById } from "./constants.js";
import { addToBackpackById, consumeFromBackpack } from "./items.js";
import { showToast, showModal, hideModal } from "./ui.js";
import { sendWs } from "./network.js";

export function startTradeWith(targetUsername) {
  sendWs({
    type: "TRADE_REQUEST",
    targetUsername,
  });
  showToast(`Trade request sent to ${targetUsername}...`);
}

export function openTradeModal(tradeId, partnerUsername) {
  state.activeTrade = {
    id: tradeId,
    partner: partnerUsername,
    myOffer: { items: [], gems: 0, accepted: false },
    partnerOffer: { items: [], gems: 0, accepted: false },
  };

  const titleEl = document.getElementById("partner-trade-title");
  if (titleEl) titleEl.innerText = `Partner (${partnerUsername})`;

  renderTradeInventoryPicker();
  renderMyOfferList();
  showModal("modal-trade");
}

export function renderTradeInventoryPicker() {
  const container = document.getElementById("my-trade-items");
  if (!container) return;
  container.innerHTML = "";

  // Render clickable backpack items to add to trade
  const availableItems = state.backpack.filter(b => b.item.type !== "tool");
  if (availableItems.length === 0) {
    container.innerHTML = "<p style='color: #94a3b8; font-size: 8px;'>No tradeable items in backpack.</p>";
    return;
  }

  const list = document.createElement("div");
  list.style.display = "flex";
  list.style.flexWrap = "wrap";
  list.style.gap = "4px";

  availableItems.forEach(b => {
    const itemBtn = document.createElement("button");
    itemBtn.className = "gt-btn";
    itemBtn.style.padding = "4px 6px";
    itemBtn.style.fontSize = "8px";
    itemBtn.innerHTML = `${b.item.icon} ${b.item.name} (${b.count})`;
    itemBtn.onclick = () => {
      const offered = state.activeTrade.myOffer.items.find(i => i.id === b.item.id);
      if (offered) {
        if (offered.count < b.count) offered.count++;
      } else {
        state.activeTrade.myOffer.items.push({ id: b.item.id, count: 1 });
      }
      notifyTradeUpdate();
    };
    list.appendChild(itemBtn);
  });

  container.appendChild(list);
}

export function renderMyOfferList() {
  const statusEl = document.getElementById("my-trade-status");
  if (!statusEl || !state.activeTrade) return;

  const items = state.activeTrade.myOffer.items;
  if (items.length > 0) {
    const summary = items
      .map(i => {
        const it = getItemById(i.id);
        return `${it?.icon || "📦"} x${i.count}`;
      })
      .join(", ");
    statusEl.innerHTML = `Offering: <strong>${summary}</strong>`;
  } else {
    statusEl.innerText = "Status: Drafting Offer";
  }
}

export function notifyTradeUpdate() {
  if (!state.activeTrade) return;
  const gems = parseInt(document.getElementById("my-trade-gems")?.value) || 0;
  state.activeTrade.myOffer.gems = gems;
  renderMyOfferList();

  sendWs({
    type: "TRADE_UPDATE",
    tradeId: state.activeTrade.id,
    items: state.activeTrade.myOffer.items,
    gems,
  });
}

export function syncTradeUI(trade) {
  const isA = trade.userA === state.username;
  const myData = isA ? trade.offerA : trade.offerB;
  const partnerData = isA ? trade.offerB : trade.offerA;

  const partnerGemsEl = document.getElementById("partner-trade-gems");
  const partnerItemsEl = document.getElementById("partner-trade-items");
  const myStatusEl = document.getElementById("my-trade-status");
  const partnerStatusEl = document.getElementById("partner-trade-status");
  const confirmBtn = document.getElementById("btn-trade-confirm");

  if (partnerGemsEl) partnerGemsEl.innerText = partnerData.gems || 0;

  if (partnerItemsEl) {
    if (partnerData.items && partnerData.items.length > 0) {
      partnerItemsEl.innerHTML = partnerData.items
        .map(i => {
          const item = getItemById(i.id);
          return `<div style="padding: 2px 0;">${item?.icon || "📦"} ${item?.name || "Item"} x${i.count}</div>`;
        })
        .join("");
    } else {
      partnerItemsEl.innerHTML = "<p style='color: #94a3b8; font-size: 8px;'>No items offered yet.</p>";
    }
  }

  if (myStatusEl) myStatusEl.innerText = myData.accepted ? "✅ Offer Locked" : "✏️ Editing Offer";
  if (partnerStatusEl) partnerStatusEl.innerText = partnerData.accepted ? "✅ Offer Locked" : "Waiting for partner...";

  if (confirmBtn) {
    if (myData.accepted && partnerData.accepted) {
      confirmBtn.disabled = false;
      confirmBtn.innerText = "Confirm Trade";
    } else {
      confirmBtn.disabled = true;
    }
  }
}

export function finalizeTrade(trade) {
  const isA = trade.userA === state.username;
  const myOffer = isA ? trade.offerA : trade.offerB;
  const partnerOffer = isA ? trade.offerB : trade.offerA;

  // Deduct my offered items
  myOffer.items.forEach(i => {
    const idx = state.backpack.findIndex(b => b.item.id === i.id);
    if (idx !== -1) consumeFromBackpack(idx, i.count);
  });
  state.gems -= myOffer.gems || 0;

  // Add partner's offered items
  partnerOffer.items.forEach(i => {
    addToBackpackById(i.id, i.count);
  });
  state.gems += partnerOffer.gems || 0;

  document.getElementById("hud-gems").innerText = state.gems;

  closeTradeModal();
  sound.trade();
  showToast("🎉 Trade completed successfully!");
}

export function closeTradeModal() {
  hideModal("modal-trade");
  state.activeTrade = null;
}
