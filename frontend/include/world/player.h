#pragma once
#include <string>
#include "imgui.h"

// Proses input keyboard untuk pergerakan pemain
void processPlayerMovement();

// Update posisi kamera berdasarkan posisi pemain
void updateCameraPosition();

// Inisialisasi posisi player saat masuk world
void initPlayerPosition();

// Render nama player di atas kepala
void renderPlayerName(ImDrawList* drawList, float playerScreenX);

// Proses klik mouse untuk memukul block
void processBlockPunch();

// Variabel eksternal
extern std::string playerName;
extern bool isTyping;

// Tambahkan di bagian bawah file, setelah deklarasi variabel eksternal lainnya
extern int activeItemId; // Item aktif yang sedang dipegang player

// Deklarasi variabel untuk status world
extern bool isInsideWorld;

// Tambahkan deklarasi untuk velocity player
extern float playerVelocityX;
extern float playerVelocityY;

// Fungsi untuk mendeteksi dan memproses interaksi dengan door
void processDoorInteraction();