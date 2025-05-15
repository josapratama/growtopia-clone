#include "../../include/menu/main.h"
#include "../../include/core/config.h"
#include <iostream>

bool renderMainMenu(AppPage& currentPage, bool& shouldClose) {
    // Dapatkan ukuran jendela aktual dari ImGui
    ImVec2 actualWindowSize = ImGui::GetIO().DisplaySize;
    
    // Mengatur posisi dan ukuran jendela untuk mengisi seluruh layar
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(actualWindowSize, ImGuiCond_Always);
    
    // Hapus border dan judul window
    ImGui::Begin("##MainMenuScreen", NULL, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBackground
    );
    
    // Dapatkan ukuran jendela saat ini (penting untuk responsivitas)
    ImVec2 windowSize = ImGui::GetWindowSize();
    
    // Hitung ukuran teks dan posisi elemen
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::SetWindowFontScale(5.0f);
    
    float titleWidth = ImGui::CalcTextSize("Growtopia").x;
    float titleHeight = ImGui::GetTextLineHeightWithSpacing() * 1.5f;
    
    // Ukuran area konten (tanpa kotak hitam)
    float contentWidth = titleWidth + 100;
    float contentHeight = titleHeight + 350;
    
    // Posisi awal konten di tengah layar
    float startX = (windowSize.x - contentWidth) * 0.5f;
    float startY = (windowSize.y - contentHeight) * 0.5f;
    
    // Posisikan cursor untuk judul
    ImGui::SetCursorPos(ImVec2(startX + (contentWidth - titleWidth) * 0.5f, startY + 20));
    
    // Tambahkan gaya tebal untuk teks
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Warna putih terang
    ImGui::Text("Growtopia");
    ImGui::PopStyleColor(); // Pop untuk warna teks judul
    ImGui::PopFont();
    
    // Tambahkan separator
    float separatorY = startY + titleHeight + 40;
    ImGui::SetCursorPosX(startX);
    ImGui::SetCursorPosY(separatorY);
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::SetNextItemWidth(contentWidth);
    ImGui::Separator();
    ImGui::PopStyleColor();
    
    // Reset font scale untuk tombol
    ImGui::SetWindowFontScale(1.5f);
    
    // Ukuran tombol
    float buttonWidth = contentWidth - 50;
    float buttonHeight = 70;
    float buttonX = startX + (contentWidth - buttonWidth) * 0.5f;
    float buttonY = separatorY + 20;
    
    // Gaya tombol dengan teks lebih besar
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20)); // Padding lebih besar
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f)); // Warna hijau
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
    
    // Tombol Play Online
    ImGui::SetCursorPos(ImVec2(buttonX, buttonY));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Warna teks putih terang
    if (ImGui::Button("PLAY ONLINE", ImVec2(buttonWidth, buttonHeight))) {
        std::cout << "Play Online diklik!" << std::endl;
        currentPage = LOGIN_PAGE;
    }
    ImGui::PopStyleColor(); // Pop untuk warna teks tombol
    
    // Tombol Option
    buttonY += buttonHeight + 20;
    ImGui::SetCursorPos(ImVec2(buttonX, buttonY));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Warna teks putih terang
    if (ImGui::Button("OPTION", ImVec2(buttonWidth, buttonHeight))) {
        std::cout << "Option diklik!" << std::endl;
        currentPage = OPTION_PAGE;
    }
    ImGui::PopStyleColor(); // Pop untuk warna teks tombol
    
    // Tombol About
    buttonY += buttonHeight + 20;
    ImGui::SetCursorPos(ImVec2(buttonX, buttonY));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Warna teks putih terang
    if (ImGui::Button("ABOUT", ImVec2(buttonWidth, buttonHeight))) {
        std::cout << "About diklik!" << std::endl;
        currentPage = ABOUT_PAGE;
    }
    ImGui::PopStyleColor(); // Pop untuk warna teks tombol
    
    // Tombol Quit dengan warna berbeda
    ImGui::PopStyleColor(3);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f)); // Warna merah
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
    
    buttonY += buttonHeight + 20;
    ImGui::SetCursorPos(ImVec2(buttonX, buttonY));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Warna teks putih terang
    if (ImGui::Button("QUIT", ImVec2(buttonWidth, buttonHeight))) {
        std::cout << "Quit diklik!" << std::endl;
        shouldClose = true;
    }
    ImGui::PopStyleColor(); // Pop untuk warna teks tombol
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    // Reset font scale
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::End();
    
    return true;
}