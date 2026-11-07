/**
 * game.js — Modularized into web/js/
 * The engine has been cleanly decoupled into modular ES modules:
 * - web/js/constants.js: Items registry, block definitions, splicing recipes
 * - web/js/audio.js: Procedural 8-bit sound synthesizer
 * - web/js/particles.js: Debris, gem sparkles, text popups, smoke
 * - web/js/state.js: Central game state, XP & leveling system
 * - web/js/world.js: Dual-layer world generator (FG/BG) & collision queries
 * - web/js/physics.js: Player movement, jumping, wings glide, lava hazards
 * - web/js/farming.js: Tree growth, splicing, harvesting & loot drops
 * - web/js/locks.js: World Lock, area protection & permissions
 * - web/js/items.js: Dropped items physics, auto-pickup & backpack
 * - web/js/trade.js: 2-step secure player trading
 * - web/js/commands.js: Authentic Growtopia slash commands (/help, /me, /who, etc.)
 * - web/js/network.js: WebSocket protocol & multiplayer synchronization
 * - web/js/renderer.js: Dual-layer canvas rendering, animations & wearables
 * - web/js/ui.js: Modals, hotbars, toasts, equipment HUD & shop
 * - web/js/main.js: Entry point, input listeners & main game loop
 */

import "./js/main.js";
