#include "../../../include/world/ui/shop.h"
#include "../../../include/world/globals.h"
#include "../../../include/core/config.h"
#include "imgui.h"

// Render tombol dan dialog Shop
void renderShop() {
    if (showShop) {
        ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH - 310, 170), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);
        ImGui::Begin("Shop", &showShop, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        // Tampilkan jumlah gems dan growtokens
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "Gems: 1000");
        ImGui::SameLine(150);
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Growtokens: 5");
        ImGui::Separator();
        
        // Daftar item yang bisa dibeli
        ImGui::Text("Items for Sale:");
        
        ImGui::BeginChild("ShopItems", ImVec2(0, 200), true);
        
        // Item 1
        ImGui::Text("Pickaxe");
        ImGui::SameLine(150);
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "200 Gems");
        ImGui::SameLine();
        if (ImGui::Button("Buy##1", ImVec2(60, 0))) {
            // Proses pembelian
        }
        
        // Item 2
        ImGui::Text("Seed Pack");
        ImGui::SameLine(150);
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "50 Gems");
        ImGui::SameLine();
        if (ImGui::Button("Buy##2", ImVec2(60, 0))) {
            // Proses pembelian
        }
        
        // Item 3
        ImGui::Text("World Lock");
        ImGui::SameLine(150);
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "2000 Gems");
        ImGui::SameLine();
        if (ImGui::Button("Buy##3", ImVec2(60, 0))) {
            // Proses pembelian
        }
        
        // Item 4
        ImGui::Text("Rare Item");
        ImGui::SameLine(150);
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "2 Growtokens");
        ImGui::SameLine();
        if (ImGui::Button("Buy##4", ImVec2(60, 0))) {
            // Proses pembelian
        }
        
        ImGui::EndChild();
        
        if (ImGui::Button("Close", ImVec2(280, 30))) {
            showShop = false;
        }
        
        ImGui::End();
    }
}