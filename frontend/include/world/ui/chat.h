#pragma once
#include "imgui.h"

// Proses input chat
void processChatInput();

// Render chat bubble di atas kepala pemain
void renderChatBubble(ImDrawList* drawList, float playerScreenX);