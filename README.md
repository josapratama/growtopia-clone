# Growtopia Clone (Desktop & Full Web Edition v2.0)

Proyek remake/klon game **Growtopia** lengkap dengan platform multi-layer:
1. **Authoritative 20Hz Netcode Engine (`/netcode-engine`)** — Engine jaringan multiplayer real-time tingkat industri dengan Client-Side Prediction (CSP), Server Reconciliation, dan Entity Linear Interpolation (LERP).
2. **Web Client (HTML5 Canvas + WebSockets)** — Dapat dimainkan langsung di web browser (PC/HP/Tablet) dengan fitur lengkap Growtopia (Farming, Splicing, World Lock, Trade, Wearables, dan Drops).
3. **Desktop Client & Server (C++17/C++20)** — Native desktop client berbasis GLFW, Dear ImGui, SFML, dan PostgreSQL backend server.

---

## ⚡ Authoritative Netcode Engine (`/netcode-engine`)

Sistem sinkronisasi multiplayer kompetitif dengan tick rate server 20 Hz (50 ms):
- **Client-Side Prediction (CSP)**: Respons instan gerakan karakter tanpa menunggu round-trip latency.
- **Server Reconciliation**: Koreksi otomatis saat terdeteksi drift deviasi posisi $> 1.5\text{ px}$.
- **Diagnostic Inspector**: Slider latensi artifisial ($0 - 400\text{ ms}$), packet drop simulator ($0 - 30\%$), dan visualisasi Server Ghost.
- **Jalankan Netcode Sandbox Engine**:
  ```bash
  cd netcode-engine
  npm install
  node server/server.js
  ```
  Akses di browser: `http://localhost:8085`

---

## 🚀 Cara Menjalankan Web Client (HTML5 & Node.js WebSocket Server)

Web Client dapat dijalankan sangat mudah tanpa perlu kompilasi C++ maupun setup vcpkg!

### 1. Jalankan Web Server & WebSocket Bridge:
```bash
cd web
npm install
npm start
```
Buka browser di: **`http://localhost:8080`**

### 2. Atau Buka Langsung (Standalone):
Anda juga dapat langsung membuka file `web/index.html` di Google Chrome / browser apa pun untuk langsung bermain secara instan (offline fallback mode)!

---

## 🎮 Fitur-Fitur Lengkap yang Telah Diimplementasikan

### 1. Sistem Pertanian & Kawin Silang Bibit (*Farming & Splicing System*)
- **Penanaman Bibit:** Tanam Dirt Seed, Rock Seed, Cave Seed, Wood Seed, Lava Seed, Door Seed, Magma Seed, dll. di atas tanah.
- **Fase Pertumbuhan Pohon (3 Tahap Visual):**
  1. *Sprout:* Tunas kecil hijau.
  2. *Sapling:* Pohon muda dengan dedaunan sesuai warna jenis bibit.
  3. *Mature Tree:* Pohon berbuah matang dengan efek kilauan (*sparkle*).
- **Panen (*Harvesting*):** Memukul pohon matang menjatuhkan blok hasil, bibit baru, dan Gems!
- **Splicing (Kawin Silang Bibit):** Tanam bibit B di atas pohon bibit A untuk menghasilkan jenis bibit dan pohon baru:
  - `Dirt Seed` + `Rock Seed` = **`Lava Seed`**
  - `Dirt Seed` + `Cave Seed` = **`Door Seed`** *(Fixed & yields White Door upon harvest)*
  - `Dirt Seed` + `Lava Seed` = **`Wood Seed`**
  - `Wood Seed` + `Dirt Seed` = **`Platform Seed`**
  - `Rock Seed` + `Wood Seed` = **`Brick Seed`**
  - `Lava Seed` + `Brick Seed` = **`Magma Seed`**
  - `Lava Seed` + `Obsidian Seed` = **`Crimson Jetpack`**

### 2. Sistem Kunci Dunia (*World Lock & Area Protection*)
- **World Lock (WL):** Mengklaim kepemilikan seluruh world untuk Anda.
- **Proteksi Anti-Grief:** Hanya pemilik dan pemain yang diberi izin yang dapat merusak/memasang blok di world yang dikunci.
- **Manajemen Akses (`/access <nama>`):**
  - Wrench pada World Lock untuk membuka dialog pengaturan.
  - Memberikan atau mencabut izin bangun untuk pemain lain.
  - Mengambil kembali World Lock (*Break Lock*).
- **Indikator Visual:** Gembok emas dengan animasi kilauan dan banner status kepemilikan di HUD atas (`🔒 Locked by [Owner]` / `🔓 Public`).

### 3. Sistem Drop Barang & Fisika Lemparan (*Item Drops & Physics*)
- **Directional Throw Physics:** Item yang dijatuhkan via `/drop <qty>` atau modal tas memiliki kecepatan lempar horizontal ke arah hadap pemain beserta pantulan lantai natural.
- **Dropper Pickup Delay:** Perlindungan anti auto-sedot selama 3 detik bagi pelempar agar tidak langsung tersedot kembali secara instan, sementara pemain lain dapat mengambilnya setelah 1 detik.
- **Trash & Gem Recycling (`/trash <qty>`):** Daur ulang item sampah menjadi Gems langsung dari tombol modal tas atau perintah `/trash`.

### 4. Sistem Perdagangan Aman 2-Langkah (*2-Step Secure Trade*)
- **Anti Duplication & Inventory Validation:** Menjamin ketersediaan item di tas sebelum *Accept* dan *Confirm*.
- **Kontrol Antarmuka Lengkap:** Tombol *Accept Offer* dan *Confirm Trade* terintegrasi penuh secara realtime antar pemain.

### 5. Perlengkapan & Mobilitas Baru
- **Crimson Jetpack:** Perlengkapan punggung dengan tenaga roket ganda untuk lompatan tinggi, partikel asap, dan *sustained gliding*.
- **Golden Pickaxe:** Alat tambang emas dengan kecepatan menghancurkan blok 3x lipat dibanding pukulan tinju.
- **White Door & Respawn Sync:** Perbaikan sinkronisasi warp pintu world dan perintah `/respawn` / `/kill`.
- **Auto-Pickup:** Pemain yang mendekati item otomatis mengambilnya dengan efek suara lonceng.

### 4. Sistem Perdagangan Aman (*Secure 2-Step Trade System*)
- Klik Wrench ke pemain lain untuk membuka jendela **Secure Trade**.
- **Dua Sisi Penawaran:** Masukkan item dan jumlah Gems yang ingin ditukar.
- **Keamanan 2 Langkah:**
  1. *Accept Offer:* Kedua belah pihak mengunci penawaran (tidak bisa diubah mendadak).
  2. *Confirm Trade (Hitung mundur 3 detik):* Mencegah trik penipuan (*scamming*).

### 5. Karakter & Perlengkapan Kosmetik (*Wearables & Equipment*)
- **Angel Wings (Sayap Malaikat):**
  - Renders sayap malaikat putih bercahaya dengan animasi kepakan di punggung karakter.
  - **Kemampuan Khusus:** Memberikan *Double Jump* dan efek *Slow Gliding* (meluncur perlahan saat menahan tombol Lompat).
- **Royal Crown:** Mahkota emas megah bertabur permata merah di atas kepala.
- **Royal Robe:** Jubah kerajaan berwarna ungu beludru dengan aksen emas.

### 6. Toko Gems (*In-Game Store*)
- Beli berbagai paket item menggunakan Gems hasil menambang:
  - 🔒 **World Lock** (2,000 Gems)
  - 🔐 **Small Lock** (50 Gems) & **Big Lock** (200 Gems)
  - 🌱 **Basic Splicing Pack** (100 Gems)
  - 🪽 **Angel Wings** (500 Gems)
  - 👑 **Royal Outfit Pack** (600 Gems)

### 7. Penyimpanan Dunia Permanen (*World Persistence*)
- Setiap perubahan blok, penanaman pohon, kunci dunia, dan item yang dijatuhkan otomatis disimpan ke file `web/data/worlds.json` di server. Saat server direstart atau pemain masuk kembali, dunia tetap tersimpan utuh!

---

## ⌨️ Kontrol Permainan
- `A` / `D` atau `Panah Kiri` / `Panah Kanan`: Bergerak kiri/kanan.
- `W` / `Space` / `Panah Atas`: Melompat (atau meluncur perlahan jika memakai Angel Wings).
- `1` s/d `9`: Memilih slot item tas / hotbar.
- `Klik Kiri Mouse`: Memukul blok, menanam bibit, memasang gembok, atau menggunakan Wrench.
- `Klik Slot Tas yang Terpilih`: Membuka menu aksi item (Drop / Wear / Info).
- `Enter`: Masuk/keluar White Door atau mengetik chat.
- Touch Controls: D-Pad virtual dan tombol aksi otomatis aktif pada layar sentuh.

---

## 🛠️ Native Desktop Client & Server (C++)

### Backend Server (C++20):
```bash
cd backend
docker compose up -d           # Menjalankan PostgreSQL
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
./build/Release/GrowtopiaServer.exe
```

### Frontend Desktop (C++17):
```bash
cd frontend
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
./build/Release/GrowtopiaApp.exe
```
