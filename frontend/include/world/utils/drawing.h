#pragma once
#include "imgui.h"

// Fungsi untuk menggambar blok
void drawBlock(ImDrawList* drawList, float x, float y, float size, ImVec4 color);

// Fungsi untuk menggambar karakter pemain
void drawPlayer(ImDrawList* drawList, float x, float y, float size);