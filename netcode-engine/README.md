# 2D Sandbox MMO Netcode Engine ⛏️🌐

An authoritative 20 Hz tick-rate 2D multiplayer sandbox engine designed to demonstrate industrial-grade real-time game networking patterns directly in the browser over WebSockets.

---

## 🚀 Key Architectural Features

1. **Authoritative Server Physics (20 Hz Tick Loop)**:
   - The server maintains 100% authority over player movement, collisions (AABB with tile grid), velocity, inventory, and tile HP.
   - Prevents speed hacks, wall-passing, and duplicate item exploits.

2. **Client-Side Prediction (CSP)**:
   - The local client simulates character physics instantly upon key press without waiting for round-trip latency (RTT).
   - Zero input lag even on high latency or erratic network connections.

3. **Server Reconciliation**:
   - Each input packet includes an incremental sequence number (`seq`).
   - The server acknowledges the last processed input (`lastAckSeq`).
   - If the client's predicted position drifts beyond tolerance (1.5 px) from the authoritative snapshot, the client snaps back and replays all pending unacknowledged inputs seamlessly.

4. **Remote Entity Linear Interpolation (LERP)**:
   - Remote players are rendered using a 100 ms historical interpolation buffer with cubic ease / linear transitions, eliminating jitter caused by network jitter or packet arrival gaps.

5. **Destructible Voxel Tilemap & Physics Drops**:
   - Procedural heightmap terrain generation with dirt, stone, wood, brick, glass, gold, and bedrock layers.
   - Mining reduces tile HP and spawns physics-enabled floating item entities that fall with gravity and are picked up by players.

6. **Interactive Netcode Diagnostic Inspector**:
   - Built-in sliders to simulate artificial latency (0 - 400 ms) and simulated packet drop rate (0 - 30%).
   - "Server Ghost" debug visualization to see the exact delta between local client prediction and authoritative server state.

7. **Zero-Dependency Web Audio 8-bit Synthesizer**:
   - Procedural 8-bit sound effects for jump, mining hit, block placement, and item collection using native browser Web Audio API.

---

## 📦 Getting Started

### Run with Node.js
```bash
cd project/sandbox-mmo
npm install
node server/server.js
```
Open [http://localhost:8085](http://localhost:8085) in your web browser.

### Run with Docker Compose
```bash
docker compose up -d --build
```

---

## 🎮 Game Controls

| Action | Control |
|---|---|
| **Move Left / Right** | `A` / `D` or `Arrow Left` / `Arrow Right` |
| **Jump** | `W`, `Space`, or `Arrow Up` |
| **Mine / Break Tile** | `Left Click` (or hold) |
| **Place Tile** | `Right Click` (or hold) |
| **Select Material** | `1` to `8` keys or click hotbar |
| **Open Chat** | `Enter` key |

