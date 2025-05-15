#include "../../../include/world/ui/info.h"
#include "../../../include/world/globals.h"
#include "../../../include/core/config.h"
#include <cstring>
#include "imgui.h"

// Render informasi world di bagian atas
void renderWorldInfo() {
    ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::BeginChild("WorldInfo", ImVec2(180, 70), true);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "World: %s", currentWorldName.c_str());
    ImGui::Text("Gunakan WASD untuk bergerak");
    ImGui::Text("Klik tombol Chat untuk chat"); // Ubah petunjuk
    ImGui::EndChild();
}

// Render semua tombol UI dengan ukuran yang lebih kecil
void renderUIButtons() {
    // Tombol menu di pojok kanan atas (lebih kecil)
    ImGui::SetCursorPos(ImVec2(SCR_WIDTH - 70, 10));
    if (ImGui::Button("Menu", ImVec2(60, 25))) {
        showSettingsMenu = !showSettingsMenu;
        showSocialPortal = false;
        showShop = false;
    }
    
    // Tombol Chat (lebih kecil)
    ImGui::SetCursorPos(ImVec2(SCR_WIDTH - 200, 10));
    if (ImGui::Button("Chat", ImVec2(60, 25))) {
        showChatInput = true;
        memset(chatInput, 0, sizeof(chatInput));
    }
    
    // Tombol Social Portal (lebih kecil)
    ImGui::SetCursorPos(ImVec2(SCR_WIDTH - 130, 10));
    if (ImGui::Button("Social", ImVec2(60, 25))) {
        showSocialPortal = !showSocialPortal;
        showSettingsMenu = false;
        showShop = false;
    }
    
    // Tombol Shop (lebih kecil)
    ImGui::SetCursorPos(ImVec2(SCR_WIDTH - 200, 40));
    if (ImGui::Button("Shop", ImVec2(60, 25))) {
        showShop = !showShop;
        showSettingsMenu = false;
        showSocialPortal = false;
    }
}