// Tambahkan include ulang secara eksplisit di bagian atas file
#include "../../include/world/renderer.h"
#include "../../include/world/constants.h"
#include "../../include/world/globals.h"
#include "../../include/world/terrain.h"
#include "../../include/world/player.h"
#include "../../include/world/ui/chat.h" // Pastikan include ini benar
#include "../../include/world/ui/settings.h"
#include "../../include/world/ui/social.h"
#include "../../include/world/ui/shop.h"
#include "../../include/world/ui/info.h"
#include "../../include/world/utils/drawing.h"
#include "../../include/core/config.h"
#include "imgui.h"

// Fungsi untuk merender tampilan world
// Tambahkan include untuk backpack UI
#include "../../include/inventory/ui/backpack_ui.h"
#include "../../include/inventory/backpack.h"

// Deklarasikan backpack player sebagai variabel global
Backpack playerBackpack(13); // 13 slot seperti di Growtopia

// Modifikasi fungsi renderWorldView
void renderWorldView(AppPage currentPage) {
    // Atur jendela untuk mengisi seluruh layar
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(SCR_WIDTH, SCR_HEIGHT), ImGuiCond_Always);
    
    // Di dalam fungsi renderWorldView, tambahkan flag untuk menghindari flickering
    ImGui::Begin("World View", NULL, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus
        // Hapus flag yang tidak didukung:
        // ImGuiWindowFlags_NoDocking |
        // ImGuiWindowFlags_NoNav
    );
    
    // Dapatkan ImDrawList untuk menggambar
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Render elemen dunia
    renderSky(drawList);
    renderGroundTerrain(drawList);
    renderFloatingPlatforms(drawList);
    
    // Di dalam fungsi renderWorldView, setelah processPlayerMovement()
    
    // Proses input dan pergerakan pemain
    processPlayerMovement();
    updateCameraPosition();
    
    // Proses klik mouse untuk memukul block
    processBlockPunch();
    
    // Proses interaksi dengan door
    processDoorInteraction();
    
    // Gambar karakter pemain
    float playerScreenX = playerX - cameraX;
    drawPlayer(drawList, playerScreenX, playerY, BLOCK_SIZE);
    
    // Render nama player di atas kepala
    renderPlayerName(drawList, playerScreenX);
    
    // Render chat bubble
    renderChatBubble(drawList, playerScreenX);
    
    // Proses input chat
    processChatInput(); // Hapus deklarasi fungsi di dalam fungsi
    
    // Render UI
    renderWorldInfo();
    
    // Jika UI tidak disembunyikan, tampilkan semua elemen UI
    if (!hideUI) {
        renderUIButtons();
        renderSettingsMenu(currentPage);
        renderSocialPortal();
        renderShop();
        
        // Render backpack UI
        renderBackpackUI(playerBackpack);
    } else {
        // Tombol untuk menampilkan UI jika disembunyikan
        ImGui::SetCursorPos(ImVec2(SCR_WIDTH - 110, 10));
        if (ImGui::Button("Show UI", ImVec2(100, 30))) {
            hideUI = false;
        }
    }
    
    // Tambahkan tombol untuk toggle backpack di bagian bawah layar
    if (!showBackpack && backpackAnimationProgress <= 0.0f) {
        ImGui::SetCursorPos(ImVec2((SCR_WIDTH - 100) / 2, SCR_HEIGHT - 30));
        if (ImGui::Button("Backpack", ImVec2(100, 25))) {
            toggleBackpack();
        }
    }
    
    // Tambahkan di bagian bawah fungsi renderWorldView, sebelum ImGui::End()
    
    // Tampilkan item aktif di sudut kiri atas
    if (!hideUI) {
        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::BeginGroup();
        
        // Background untuk item aktif
        ImVec2 activeItemSize(50, 50);
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            cursorPos,
            ImVec2(cursorPos.x + activeItemSize.x, cursorPos.y + activeItemSize.y),
            IM_COL32(50, 50, 50, 200),
            4.0f
        );
        
        // Tampilkan item aktif
        std::string activeItemLabel;
        if (activeItemId == 1) {
            activeItemLabel = "Punch";
        } else if (activeItemId == 50) {
            activeItemLabel = "Wrench";
        } else {
            activeItemLabel = std::to_string(activeItemId);
        }
        
        // Posisikan teks di tengah kotak
        float textWidth = ImGui::CalcTextSize(activeItemLabel.c_str()).x;
        ImGui::SetCursorPos(ImVec2(10 + (activeItemSize.x - textWidth) / 2, 10 + activeItemSize.y / 3));
        ImGui::Text("%s", activeItemLabel.c_str());
        
        ImGui::EndGroup();
    }
    
    ImGui::End();
}

// Tambahkan fungsi untuk inisialisasi world dan player
void initWorldAndPlayer() {
    // Inisialisasi posisi player
    initPlayerPosition();
    
    // Inisialisasi backpack player dengan item default
    playerBackpack.initNewPlayerBackpack();
    
    // Inisialisasi backpack UI
    initBackpackUI();
}