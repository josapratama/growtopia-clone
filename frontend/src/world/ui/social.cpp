#include "../../../include/world/ui/social.h"
#include "../../../include/world/globals.h"
#include "../../../include/core/config.h"
#include "imgui.h"

// Render tombol dan dialog Social Portal
void renderSocialPortal() {
    if (showSocialPortal) {
        ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH - 310, 130), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always);
        ImGui::Begin("Social Portal", &showSocialPortal, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        if (ImGui::Button("Show Friends", ImVec2(280, 30))) {
            // Implementasi tampilkan teman
        }
        
        if (ImGui::Button("Community Hub", ImVec2(280, 30))) {
            // Implementasi community hub
        }
        
        if (ImGui::Button("Show Apprentices", ImVec2(280, 30))) {
            // Implementasi tampilkan apprentices
        }
        
        if (ImGui::Button("Create Guild", ImVec2(280, 30))) {
            // Implementasi buat guild
        }
        
        if (ImGui::Button("Trade History", ImVec2(280, 30))) {
            // Implementasi riwayat perdagangan
        }
        
        if (ImGui::Button("Back", ImVec2(280, 30))) {
            showSocialPortal = false;
        }
        
        ImGui::End();
    }
}