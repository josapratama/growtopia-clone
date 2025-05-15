#pragma once

#include "../../world/constants.h"
#include "../../world/globals.h"
#include "../backpack.h"
#include "imgui.h"

// Status backpack UI
extern bool showBackpack;
extern float backpackPanelHeight;
extern float backpackAnimationProgress;

// Variabel untuk drag backpack
extern bool isDraggingBackpack;
extern float dragStartY;
extern float dragStartProgress;

// Fungsi untuk memuat texture dari file
unsigned int loadTexture(const char* filename);

// Inisialisasi backpack UI
void initBackpackUI();

// Render backpack UI yang bisa ditarik dari bawah
void renderBackpackUI(Backpack& playerBackpack);

// Toggle tampilan backpack
void toggleBackpack();