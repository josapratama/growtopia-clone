/**
 * farming.js — Tree Growth, Splicing & Harvesting Mechanics
 */

import { BLOCK_SIZE, ITEMS, SPLICING_RECIPES, getItemById } from "./constants.js";
import { state, addXP } from "./state.js";
import { isSolidBlock } from "./world.js";
import { sound } from "./audio.js";
import { addBlockDebris, addTextPopup } from "./particles.js";
import { spawnDroppedItem, consumeFromBackpack } from "./items.js";
import { showToast } from "./ui.js";
import { sendWs } from "./network.js";

export function handleSeedPlanting(tileX, tileY, seedItem) {
  // Check if tile has an existing tree for Splicing!
  const existingTree = state.trees.find(t => t.x === tileX && t.y === tileY);
  if (existingTree) {
    const spliceKey = `${existingTree.seedId},${seedItem.id}`;
    const resultSeedId = SPLICING_RECIPES[spliceKey];

    if (resultSeedId) {
      // SUCCESSFUL SPLICE!
      consumeFromBackpack(state.selectedSlot, 1);
      state.trees = state.trees.filter(t => !(t.x === tileX && t.y === tileY));

      const newSeedItem = getItemById(resultSeedId);
      const splicedTree = {
        id: `tree_${tileX}_${tileY}_${Date.now()}`,
        x: tileX,
        y: tileY,
        seedId: resultSeedId,
        plantedAt: Date.now(),
        growTime: newSeedItem ? newSeedItem.growTime : 30,
        spliceWith: seedItem.name,
      };
      state.trees.push(splicedTree);
      sound.splice();
      addTextPopup(tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE, "🌟 SPLICE SUCCESS!", "#38bdf8");
      showToast(`🌟 Spliced into ${newSeedItem.name}!`);
      addXP(15);

      sendWs({
        type: "PLANT_TREE",
        tileX,
        tileY,
        seedId: resultSeedId,
        growTime: splicedTree.growTime,
        spliceWith: seedItem.name,
      });
    } else {
      showToast("These seeds cannot be spliced together!");
      sound.punch();
    }
    return;
  }

  // Must have a solid block below to plant a tree
  const belowSolid = isSolidBlock(tileX, tileY + 1);
  if (!belowSolid) {
    showToast("Trees must be planted on solid ground!");
    return;
  }
  if (state.world.fg[tileY]?.[tileX] !== null) {
    showToast("Tile is occupied!");
    return;
  }

  consumeFromBackpack(state.selectedSlot, 1);
  const newTree = {
    id: `tree_${tileX}_${tileY}_${Date.now()}`,
    x: tileX,
    y: tileY,
    seedId: seedItem.id,
    plantedAt: Date.now(),
    growTime: seedItem.growTime || 20,
    spliceWith: null,
  };
  state.trees.push(newTree);
  sound.placeBlock();

  sendWs({
    type: "PLANT_TREE",
    tileX,
    tileY,
    seedId: seedItem.id,
    growTime: newTree.growTime,
  });
}

export function handleHarvestTree(tileX, tileY, treeIdx) {
  const tree = state.trees[treeIdx];
  const elapsedSec = (Date.now() - tree.plantedAt) / 1000;
  const isMature = elapsedSec >= tree.growTime;

  if (isMature) {
    // Harvest mature tree!
    state.trees.splice(treeIdx, 1);
    sound.splice();
    addBlockDebris(tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16, "#22c55e");

    const seedItem = getItemById(tree.seedId);
    const produceId = seedItem ? seedItem.growsInto : ITEMS.DIRT.id;
    const produceItem = getItemById(produceId) || ITEMS.DIRT;

    // Drop harvest loot: 1-3 blocks, 1-2 seeds, 2-6 gems!
    const blockDrop = Math.floor(1 + Math.random() * 3);
    const seedDrop = Math.floor(1 + Math.random() * 2);
    const gemDrop = Math.floor(2 + Math.random() * 5);

    spawnDroppedItem(produceId, blockDrop, tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16);
    spawnDroppedItem(tree.seedId, seedDrop, tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE + 16);

    state.gems += gemDrop;
    document.getElementById("hud-gems").innerText = state.gems;
    addTextPopup(tileX * BLOCK_SIZE + 16, tileY * BLOCK_SIZE - 12, `+${gemDrop} 💎`, "#facc15");
    showToast(`Harvested ${produceItem.name} x${blockDrop} & +${gemDrop} 💎`);
    addXP(20);

    sendWs({
      type: "HARVEST_TREE",
      tileX,
      tileY,
    });
  } else {
    showToast(`Tree is still growing... (${Math.ceil(tree.growTime - elapsedSec)}s left)`);
    sound.punch();
  }
}

