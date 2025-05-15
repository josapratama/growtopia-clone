#include "../../include/core/pages.h"
#include "../../include/core/config.h"
#include "imgui.h"
#include <iostream>

void renderOptionPage(AppPage& currentPage) {
    // Mengatur posisi dan ukuran jendela
    ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH/2 - 200, SCR_HEIGHT/2 - 150), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Options", NULL, ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Game Options");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Contoh opsi-opsi game
    static bool fullscreen = false;
    static int volume = 50;
    static int quality = 1;
    static const char* qualityItems[] = { "Low", "Medium", "High" };
    
    ImGui::Checkbox("Fullscreen", &fullscreen);
    ImGui::SliderInt("Volume", &volume, 0, 100, "%d%%");
    ImGui::Combo("Graphics Quality", &quality, qualityItems, IM_ARRAYSIZE(qualityItems));
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Tombol Back
    if (ImGui::Button("Back to Main Menu", ImVec2(ImGui::GetWindowWidth() - 30, 40))) {
        currentPage = MAIN_MENU;
    }
    
    ImGui::End();
}

void renderAboutPage(AppPage& currentPage) {
    // Mengatur posisi dan ukuran jendela
    ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH/2 - 200, SCR_HEIGHT/2 - 150), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("About Growtopia", NULL, ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Growtopia Clone");
    ImGui::Text("Version 0.1.0");
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::TextWrapped(
        "This is a Growtopia clone project created for educational purposes. "
        "The project demonstrates the use of OpenGL, GLFW, and Dear ImGui for "
        "creating a game interface."
    );
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Tombol Back
    if (ImGui::Button("Back to Main Menu", ImVec2(ImGui::GetWindowWidth() - 30, 40))) {
        currentPage = MAIN_MENU;
    }
    
    ImGui::End();
}