#include "../../../include/world/ui/settings.h"
#include "../../../include/world/globals.h"
#include "../../../include/core/config.h"
#include "imgui.h"

// Render tombol menu dan dialog settings
void renderSettingsMenu(AppPage& currentPage) {
    if (showSettingsMenu) {
        ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH - 220, 50), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_Always);
        ImGui::Begin("Settings Menu", &showSettingsMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        if (ImGui::Button("Exit World", ImVec2(180, 30))) {
            inWorldView = false;
            showSettingsMenu = false;
        }
        
        if (ImGui::Button("Respawn", ImVec2(180, 30))) {
            playerX = 50.0f;
            playerY = 50.0f;
            cameraX = 0.0f;
        }
        
        if (ImGui::Button("Options", ImVec2(180, 30))) {
            // Implementasi opsi nanti
        }
        
        if (ImGui::Button("Store", ImVec2(180, 30))) {
            showShop = true;
            showSettingsMenu = false;
        }
        
        if (ImGui::Button("Hide UI", ImVec2(180, 30))) {
            hideUI = true;
            showSettingsMenu = false;
        }
        
        if (ImGui::Button("Support", ImVec2(180, 30))) {
            // Implementasi support nanti
        }
        
        if (ImGui::Button("Logout", ImVec2(180, 30))) {
            currentPage = LOGIN_PAGE;
            inWorldView = false;
            showSettingsMenu = false;
        }
        
        if (ImGui::Button("Back", ImVec2(180, 30))) {
            showSettingsMenu = false;
        }
        
        ImGui::End();
    }
}