#pragma once
#include "../core/pages.h"
#include <string>
#include <vector>
#include "imgui.h"

// Struktur untuk menyimpan informasi world
struct WorldInfo {
    std::string name;
    std::string owner;
    int players;
    bool isLocked;
};

// Variabel global untuk menyimpan daftar world
extern std::vector<WorldInfo> worldList;
extern std::vector<std::string> myWorlds; // Daftar world milik user
extern std::vector<std::string> recentWorlds; // Daftar world yang baru dikunjungi
extern std::string currentWorldName;
extern bool inWorldView;

// Warna untuk tampilan world
extern ImVec4 skyColor;
extern ImVec4 grassColor;
extern ImVec4 blockColor;
extern ImVec4 playerColor;

// Fungsi untuk merender halaman daftar world
bool renderWorldListPage(AppPage& currentPage);