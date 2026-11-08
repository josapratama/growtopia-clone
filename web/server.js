import http from "http";
import fs from "fs";
import path from "path";
import net from "net";
import { WebSocketServer, WebSocket } from "ws";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const HTTP_PORT = process.env.PORT || 8080;
const CPP_BACKEND_PORT = 5000;
const CPP_BACKEND_HOST = "127.0.0.1";
const DATA_DIR = path.join(__dirname, "data");
const WORLDS_FILE = path.join(DATA_DIR, "worlds.json");

if (!fs.existsSync(DATA_DIR)) {
  fs.mkdirSync(DATA_DIR, { recursive: true });
}

// ─────────────────────────────────────────────────────────────
// 1. STATIC HTTP SERVER
// ─────────────────────────────────────────────────────────────
const server = http.createServer((req, res) => {
  let reqPath = req.url.split("?")[0];
  if (reqPath === "/") reqPath = "/index.html";

  const filePath = path.join(__dirname, reqPath);
  const ext = path.extname(filePath).toLowerCase();

  const mimeTypes = {
    ".html": "text/html",
    ".js": "text/javascript",
    ".css": "text/css",
    ".png": "image/png",
    ".jpg": "image/jpeg",
    ".json": "application/json",
    ".ico": "image/x-icon",
  };

  const contentType = mimeTypes[ext] || "application/octet-stream";

  fs.readFile(filePath, (err, content) => {
    if (err) {
      if (err.code === "ENOENT") {
        res.writeHead(404, { "Content-Type": "text/plain" });
        res.end("404 Not Found");
      } else {
        res.writeHead(500, { "Content-Type": "text/plain" });
        res.end(`500 Server Error: ${err.code}`);
      }
    } else {
      res.writeHead(200, {
        "Content-Type": contentType,
        "Access-Control-Allow-Origin": "*",
      });
      res.end(content);
    }
  });
});

// ─────────────────────────────────────────────────────────────
// 2. WORLD PERSISTENCE (Dual-Layer FG/BG + Doors + Signs + Weather)
// ─────────────────────────────────────────────────────────────
const WORLD_WIDTH = 100;
const WORLD_HEIGHT = 60;
const persistentWorlds = new Map();

function loadWorldsFromDisk() {
  if (fs.existsSync(WORLDS_FILE)) {
    try {
      const data = JSON.parse(fs.readFileSync(WORLDS_FILE, "utf-8"));
      for (const [name, w] of Object.entries(data)) {
        let tiles = w.tiles;
        if (Array.isArray(tiles)) {
          tiles = {
            fg: tiles,
            bg: tiles.map(row => row.map(() => null)),
          };
        }
        persistentWorlds.set(name, {
          name: w.name,
          tiles,
          trees: w.trees || [],
          lock: w.lock || null,
          signs: w.signs || {},
          doors: w.doors || {},
          weather: w.weather || "DAY",
          bans: w.bans || [],
          droppedItems: (w.droppedItems || []).slice(0, 100),
        });
      }
      console.log(`[Persistence] Loaded ${persistentWorlds.size} worlds from disk.`);
    } catch (e) {
      console.error("[Persistence] Error loading worlds:", e.message);
    }
  }
}

let saveDebounceTimer = null;
function saveWorldsToDisk() {
  if (saveDebounceTimer) clearTimeout(saveDebounceTimer);
  saveDebounceTimer = setTimeout(() => {
    try {
      const obj = {};
      for (const [name, w] of persistentWorlds.entries()) {
        obj[name] = {
          name: w.name,
          tiles: w.tiles,
          trees: w.trees || [],
          lock: w.lock || null,
          signs: w.signs || {},
          doors: w.doors || {},
          weather: w.weather || "DAY",
          bans: w.bans || [],
          droppedItems: (w.droppedItems || []).slice(0, 100),
        };
      }
      fs.writeFileSync(WORLDS_FILE, JSON.stringify(obj, null, 2), "utf-8");
      console.log(`[Persistence] Saved ${persistentWorlds.size} worlds to disk.`);
    } catch (e) {
      console.error("[Persistence] Error saving worlds:", e.message);
    }
  }, 1000);
}

loadWorldsFromDisk();

function getOrCreateWorld(worldName) {
  const normName = worldName.toUpperCase().trim() || "MAIN";
  if (!persistentWorlds.has(normName)) {
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
          fgRow.push(2); // Dirt with grass
          bgRow.push(40); // Dirt BG
        } else if (y < 55) {
          const rand = Math.random();
          if (rand < 0.15) {
            fgRow.push(4); // Cave
            bgRow.push(41); // Cave BG
          } else if (rand < 0.20 && y > 35) {
            fgRow.push(7); // Rock
            bgRow.push(41);
          } else if (rand < 0.22 && y > 40) {
            fgRow.push(6); // Lava
            bgRow.push(41);
          } else {
            fgRow.push(2); // Dirt
            bgRow.push(40);
          }
        } else {
          fgRow.push(10); // Bedrock
          bgRow.push(41);
        }
      }
      fg.push(fgRow);
      bg.push(bgRow);
    }

    const doorX = Math.floor(WORLD_WIDTH / 2);
    fg[surfaceY - 1][doorX] = 8;
    fg[surfaceY - 2][doorX] = 8;

    const doors = {};
    doors[`${doorX},${surfaceY - 1}`] = { targetWorld: "EXIT", label: "Exit Door" };

    const newWorld = {
      name: normName,
      tiles: { fg, bg },
      trees: [],
      lock: null,
      signs: {},
      doors,
      weather: "DAY",
      bans: [],
      droppedItems: [],
    };
    persistentWorlds.set(normName, newWorld);
    saveWorldsToDisk();
  }
  return persistentWorlds.get(normName);
}

// ─────────────────────────────────────────────────────────────
// 3. WEBSOCKET MULTIPLAYER & REALTIME SYNC
// ─────────────────────────────────────────────────────────────
const wss = new WebSocketServer({ server });
const activeWorlds = new Map(); // worldName -> Map(playerId, playerInfo)
const activeTrades = new Map(); // tradeId -> tradeObject

function broadcastToWorld(worldName, message, senderWs = null) {
  const players = activeWorlds.get(worldName);
  if (!players) return;
  const raw = JSON.stringify(message);
  for (const [_, client] of players) {
    if (client.ws.readyState === WebSocket.OPEN && client.ws !== senderWs) {
      client.ws.send(raw);
    }
  }
}

wss.on("connection", ws => {
  let playerId = "P_" + Math.random().toString(36).substr(2, 9);
  let username = `Guest_${Math.floor(1000 + Math.random() * 9000)}`;
  let currentWorld = null;
  let playerWearables = { hat: null, shirt: null, wings: null, hand: null };

  ws.on("message", raw => {
    try {
      const data = JSON.parse(raw);

      switch (data.type) {
        // ── WORLD JOIN ──────────────────────────────────────
        case "JOIN_WORLD": {
          const targetWorld = (data.world || "MAIN").toUpperCase().trim();
          const worldData = getOrCreateWorld(targetWorld);

          // Check if player is banned
          if ((worldData.bans || []).includes(username)) {
            ws.send(JSON.stringify({ type: "WORLD_PROTECTION_ALERT", message: "You are banned from this world!" }));
            return;
          }

          if (currentWorld && activeWorlds.has(currentWorld)) {
            const oldWorldPlayers = activeWorlds.get(currentWorld);
            oldWorldPlayers.delete(playerId);
            broadcastToWorld(currentWorld, {
              type: "PLAYER_LEFT",
              username,
              playerId,
            });
            if (oldWorldPlayers.size === 0) activeWorlds.delete(currentWorld);
          }

          currentWorld = targetWorld;

          if (!activeWorlds.has(targetWorld)) {
            activeWorlds.set(targetWorld, new Map());
          }
          const worldPlayers = activeWorlds.get(targetWorld);

          const playerInfo = {
            ws,
            playerId,
            username: data.username || username,
            x: data.x || 1600,
            y: data.y || 700,
            facingRight: true,
            wearables: data.wearables || playerWearables,
          };
          username = playerInfo.username;
          worldPlayers.set(playerId, playerInfo);

          const playerList = [];
          for (const [id, p] of worldPlayers.entries()) {
            playerList.push({
              playerId: id,
              username: p.username,
              x: p.x,
              y: p.y,
              facingRight: p.facingRight,
              wearables: p.wearables,
            });
          }

          ws.send(
            JSON.stringify({
              type: "WORLD_JOINED",
              world: targetWorld,
              tiles: worldData.tiles,
              trees: worldData.trees || [],
              lock: worldData.lock || null,
              signs: worldData.signs || {},
              doors: worldData.doors || {},
              weather: worldData.weather || "DAY",
              droppedItems: worldData.droppedItems || [],
              players: playerList,
            })
          );

          broadcastToWorld(
            targetWorld,
            {
              type: "PLAYER_JOINED",
              playerId,
              username,
              x: playerInfo.x,
              y: playerInfo.y,
              wearables: playerInfo.wearables,
            },
            ws
          );
          break;
        }

        // ── PLAYER MOVEMENT ─────────────────────────────────
        case "MOVE": {
          if (!currentWorld || !activeWorlds.has(currentWorld)) return;
          const p = activeWorlds.get(currentWorld).get(playerId);
          if (p) {
            p.x = data.x;
            p.y = data.y;
            p.facingRight = data.facingRight;
            broadcastToWorld(
              currentWorld,
              {
                type: "PLAYER_MOVED",
                playerId,
                username,
                x: data.x,
                y: data.y,
                facingRight: data.facingRight,
              },
              ws
            );
          }
          break;
        }

        // ── WEARABLES SYNC ──────────────────────────────────
        case "UPDATE_WEARABLES": {
          playerWearables = data.wearables || playerWearables;
          if (currentWorld && activeWorlds.has(currentWorld)) {
            const p = activeWorlds.get(currentWorld).get(playerId);
            if (p) p.wearables = playerWearables;
            broadcastToWorld(currentWorld, {
              type: "PLAYER_WEARABLES_UPDATED",
              playerId,
              username,
              wearables: playerWearables,
            });
          }
          break;
        }

        // ── FOREGROUND BLOCK ACTION ─────────────────────────
        case "BLOCK_ACTION": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { tileX, tileY, action, blockId } = data;

          if (tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= WORLD_HEIGHT) return;

          // World Lock Check
          if (worldData.lock && worldData.lock.owner !== username && !worldData.lock.isPublic) {
            const hasAccess = (worldData.lock.accessList || []).includes(username);
            if (!hasAccess) {
              ws.send(
                JSON.stringify({
                  type: "WORLD_PROTECTION_ALERT",
                  owner: worldData.lock.owner,
                  message: `World is locked by ${worldData.lock.owner}!`,
                })
              );
              return;
            }
          }

          if (action === "break") {
            worldData.tiles.fg[tileY][tileX] = null;
          } else if (action === "place") {
            worldData.tiles.fg[tileY][tileX] = blockId;
          }

          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "BLOCK_ACTION",
            tileX,
            tileY,
            action,
            blockId,
            by: username,
          });
          break;
        }

        // ── BACKGROUND WALLPAPER ACTION ─────────────────────
        case "BG_BLOCK_ACTION": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { tileX, tileY, action, blockId } = data;

          if (tileX < 0 || tileX >= WORLD_WIDTH || tileY < 0 || tileY >= WORLD_HEIGHT) return;

          if (worldData.lock && worldData.lock.owner !== username && !worldData.lock.isPublic) {
            const hasAccess = (worldData.lock.accessList || []).includes(username);
            if (!hasAccess) return;
          }

          if (action === "break") {
            worldData.tiles.bg[tileY][tileX] = null;
          } else if (action === "place") {
            worldData.tiles.bg[tileY][tileX] = blockId;
          }

          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "BG_BLOCK_ACTION",
            tileX,
            tileY,
            action,
            blockId,
            by: username,
          });
          break;
        }

        // ── SIGN UPDATE ─────────────────────────────────────
        case "UPDATE_SIGN": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { tileX, tileY, text } = data;
          worldData.signs = worldData.signs || {};
          worldData.signs[`${tileX},${tileY}`] = { text, owner: username };
          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "SIGN_UPDATED",
            tileX,
            tileY,
            text,
            owner: username,
          });
          break;
        }

        // ── DOOR UPDATE ─────────────────────────────────────
        case "UPDATE_DOOR": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { tileX, tileY, targetWorld, label } = data;
          worldData.doors = worldData.doors || {};
          worldData.doors[`${tileX},${tileY}`] = { targetWorld, label };
          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "DOOR_UPDATED",
            tileX,
            tileY,
            targetWorld,
            label,
          });
          break;
        }

        // ── WEATHER CHANGE ──────────────────────────────────
        case "SET_WEATHER": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          worldData.weather = data.weather || "DAY";
          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "WEATHER_CHANGED",
            weather: worldData.weather,
          });
          break;
        }

        // ── PULL PLAYER (ACTUAL TELEPORTATION) ───────────────
        case "PULL_PLAYER": {
          if (!currentWorld) return;
          const players = activeWorlds.get(currentWorld);
          if (players) {
            for (const [_, p] of players) {
              if (p.username === data.target) {
                p.ws.send(
                  JSON.stringify({
                    type: "TELEPORT",
                    x: data.x,
                    y: data.y,
                    reason: `${username} pulled you!`,
                  })
                );
                break;
              }
            }
          }
          break;
        }

        // ── KICK PLAYER ─────────────────────────────────────
        case "KICK_PLAYER": {
          if (!currentWorld) return;
          const players = activeWorlds.get(currentWorld);
          if (players) {
            for (const [id, p] of players) {
              if (p.username === data.target) {
                p.ws.send(JSON.stringify({ type: "KICKED" }));
                players.delete(id);
                broadcastToWorld(currentWorld, {
                  type: "PLAYER_LEFT",
                  username: p.username,
                  playerId: id,
                });
                break;
              }
            }
          }
          break;
        }

        // ── FARMING: PLANT & HARVEST ────────────────────────
        case "PLANT_TREE": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { tileX, tileY, seedId, growTime, spliceWith } = data;

          worldData.trees = (worldData.trees || []).filter(
            t => !(t.x === tileX && t.y === tileY)
          );

          const newTree = {
            id: `tree_${tileX}_${tileY}_${Date.now()}`,
            x: tileX,
            y: tileY,
            seedId,
            plantedAt: Date.now(),
            growTime: growTime || 30,
            spliceWith: spliceWith || null,
          };
          worldData.trees.push(newTree);
          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "TREE_PLANTED",
            tree: newTree,
            by: username,
          });
          break;
        }

        case "HARVEST_TREE": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { tileX, tileY } = data;

          const idx = (worldData.trees || []).findIndex(
            t => t.x === tileX && t.y === tileY
          );
          if (idx !== -1) {
            const harvested = worldData.trees.splice(idx, 1)[0];
            saveWorldsToDisk();

            broadcastToWorld(currentWorld, {
              type: "TREE_HARVESTED",
              tileX,
              tileY,
              tree: harvested,
              by: username,
            });
          }
          break;
        }

        // ── WORLD LOCK CLAIM / EDIT ─────────────────────────
        case "CLAIM_WORLD_LOCK": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { tileX, tileY, lockType } = data;

          if (worldData.lock) return;

          worldData.lock = {
            owner: username,
            type: lockType || "WORLD_LOCK",
            tileX,
            tileY,
            claimedAt: Date.now(),
            accessList: [],
            isPublic: false,
          };
          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "WORLD_LOCK_UPDATED",
            lock: worldData.lock,
            by: username,
          });
          break;
        }

        case "EDIT_WORLD_LOCK": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          if (!worldData.lock || worldData.lock.owner !== username) return;

          if (data.action === "addAccess") {
            const target = data.targetUser.trim();
            if (target && !worldData.lock.accessList.includes(target)) {
              worldData.lock.accessList.push(target);
            }
          } else if (data.action === "removeAccess") {
            worldData.lock.accessList = worldData.lock.accessList.filter(
              u => u !== data.targetUser
            );
          } else if (data.action === "togglePublic") {
            worldData.lock.isPublic = !worldData.lock.isPublic;
          } else if (data.action === "breakLock") {
            worldData.lock = null;
          }

          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "WORLD_LOCK_UPDATED",
            lock: worldData.lock,
            by: username,
          });
          break;
        }

        // ── ITEM DROPPING & PICKUP ──────────────────────────
        case "DROP_ITEM": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { itemId, count, x, y, itemName, icon } = data;

          const droppedItem = {
            id: "drop_" + Math.random().toString(36).substr(2, 9),
            itemId,
            itemName: itemName || "Item",
            icon: icon || "📦",
            count: count || 1,
            x: x || 1600,
            y: y || 700,
            vx: typeof data.vx === "number" ? data.vx : (Math.random() - 0.5) * 4,
            vy: typeof data.vy === "number" ? data.vy : -4 - Math.random() * 3,
            droppedBy: data.droppedBy || username,
            droppedAt: Date.now(),
          };

          worldData.droppedItems = worldData.droppedItems || [];
          worldData.droppedItems.push(droppedItem);
          saveWorldsToDisk();

          broadcastToWorld(currentWorld, {
            type: "ITEM_DROPPED",
            item: droppedItem,
            by: username,
          });
          break;
        }

        case "PICKUP_ITEM": {
          if (!currentWorld) return;
          const worldData = getOrCreateWorld(currentWorld);
          const { dropId } = data;

          const idx = (worldData.droppedItems || []).findIndex(d => d.id === dropId);
          if (idx !== -1) {
            const picked = worldData.droppedItems.splice(idx, 1)[0];
            saveWorldsToDisk();

            ws.send(
              JSON.stringify({
                type: "ITEM_COLLECTED_SELF",
                item: picked,
              })
            );

            broadcastToWorld(currentWorld, {
              type: "ITEM_PICKED_UP",
              dropId,
              by: username,
            });
          }
          break;
        }

        // ── SECURE TRADE SYSTEM ─────────────────────────────
        case "TRADE_REQUEST": {
          const targetPlayerName = data.targetUsername;
          const players = activeWorlds.get(currentWorld);
          let targetWs = null;
          if (players) {
            for (const [_, p] of players) {
              if (p.username === targetPlayerName) {
                targetWs = p.ws;
                break;
              }
            }
          }

          if (targetWs && targetWs.readyState === WebSocket.OPEN) {
            const tradeId = "trade_" + Date.now();
            activeTrades.set(tradeId, {
              id: tradeId,
              userA: username,
              userB: targetPlayerName,
              offerA: { items: [], gems: 0, accepted: false, confirmed: false },
              offerB: { items: [], gems: 0, accepted: false, confirmed: false },
            });

            targetWs.send(
              JSON.stringify({
                type: "TRADE_INVITE",
                from: username,
                tradeId,
              })
            );
          }
          break;
        }

        case "TRADE_UPDATE": {
          const { tradeId, items, gems } = data;
          const trade = activeTrades.get(tradeId);
          if (!trade) return;

          const isUserA = trade.userA === username;
          const myOffer = isUserA ? trade.offerA : trade.offerB;
          myOffer.items = items || [];
          myOffer.gems = gems || 0;
          trade.offerA.accepted = false;
          trade.offerB.accepted = false;

          broadcastToWorld(currentWorld, {
            type: "TRADE_STATE_SYNC",
            tradeId,
            trade,
          });
          break;
        }

        case "TRADE_ACCEPT": {
          const { tradeId } = data;
          const trade = activeTrades.get(tradeId);
          if (!trade) return;

          if (trade.userA === username) trade.offerA.accepted = true;
          if (trade.userB === username) trade.offerB.accepted = true;

          broadcastToWorld(currentWorld, {
            type: "TRADE_STATE_SYNC",
            tradeId,
            trade,
          });
          break;
        }

        case "TRADE_CONFIRM": {
          const { tradeId } = data;
          const trade = activeTrades.get(tradeId);
          if (!trade) return;

          if (trade.userA === username) trade.offerA.confirmed = true;
          if (trade.userB === username) trade.offerB.confirmed = true;

          if (trade.offerA.confirmed && trade.offerB.confirmed) {
            broadcastToWorld(currentWorld, {
              type: "TRADE_COMPLETED",
              tradeId,
              trade,
            });
            activeTrades.delete(tradeId);
          } else {
            broadcastToWorld(currentWorld, {
              type: "TRADE_STATE_SYNC",
              tradeId,
              trade,
            });
          }
          break;
        }

        case "TRADE_CANCEL": {
          const { tradeId } = data;
          if (activeTrades.has(tradeId)) {
            activeTrades.delete(tradeId);
            broadcastToWorld(currentWorld, {
              type: "TRADE_CANCELLED",
              tradeId,
              by: username,
            });
          }
          break;
        }

        // ── CHAT ────────────────────────────────────────────
        case "CHAT": {
          if (!currentWorld) return;
          broadcastToWorld(currentWorld, {
            type: "CHAT_MESSAGE",
            username,
            message: (data.message || "").slice(0, 100),
          });
          break;
        }
      }
    } catch (e) {
      console.error("[WebSocket Error]", e.message);
    }
  });

  ws.on("close", () => {
    if (currentWorld && activeWorlds.has(currentWorld)) {
      const players = activeWorlds.get(currentWorld);
      players.delete(playerId);
      broadcastToWorld(currentWorld, {
        type: "PLAYER_LEFT",
        username,
        playerId,
      });
      if (players.size === 0) activeWorlds.delete(currentWorld);
    }
  });
});

server.listen(HTTP_PORT, () => {
  console.log(`\n=================================================`);
  console.log(`🚀 Growtopia Clone Modular Web Engine Running!`);
  console.log(`🌐 URL: http://localhost:${HTTP_PORT}`);
  console.log(`💾 World Persistence: ${WORLDS_FILE}`);
  console.log(`=================================================\n`);
});
