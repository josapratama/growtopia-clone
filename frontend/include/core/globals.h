#pragma once

#include <string>
#include "imgui.h"

// Deklarasi variabel global
extern bool inWorldView;
extern float playerX;
extern float playerY;
extern std::string currentWorldName;

// Warna untuk tampilan world
extern ImVec4 skyColor;
extern ImVec4 grassColor;
extern ImVec4 blockColor;

#define BLOCK_SIZE 32