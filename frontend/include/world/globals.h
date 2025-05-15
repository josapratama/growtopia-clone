#pragma once
#include <string>
#include <map>
#include <utility>
#include "imgui.h"

// Variabel untuk fitur UI
extern bool showSettingsMenu;
extern bool showSocialPortal;
extern bool showShop;
extern bool hideUI;
extern char chatInput[128];
extern bool showChatInput;
extern std::string currentChatMessage;
extern float chatMessageTimer;
extern float cameraX;

// Variabel global yang dideklarasikan di header
extern bool inWorldView;
extern float playerX;
extern float playerY;
extern std::string currentWorldName;

// Warna untuk elemen dunia
extern ImVec4 skyColor;
extern ImVec4 grassColor;
extern ImVec4 blockColor;

// Struktur data untuk menyimpan health block
// Key: pair<x, y> koordinat block, Value: health yang tersisa
extern std::map<std::pair<int, int>, int> blockHealth;