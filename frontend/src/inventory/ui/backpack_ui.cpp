#include "../../../include/inventory/ui/backpack_ui.h"
#include "../../../include/inventory/item.h"
#include "../../../include/core/config.h"
#include <iostream>
#include <glad/glad.h> // Header untuk OpenGL
#include <GLFW/glfw3.h> // Header untuk GLFW

// Tambahkan di bagian atas file, setelah include lainnya
#include "../../../include/world/player.h" // Untuk mengakses activeItemId

// ATAU tambahkan deklarasi extern langsung
// extern int activeItemId; // Item aktif yang sedang dipegang player
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Pastikan file ini tersedia di project Anda

// Fungsi untuk memuat texture dari file
unsigned int loadTexture(const char* filename) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << filename << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

// Status backpack UI
bool showBackpack = false;
float backpackPanelHeight = 150.0f; // Tinggi panel backpack
float backpackAnimationProgress = 0.0f; // 0.0f = tersembunyi, 1.0f = terlihat penuh
float backpackAnimationSpeed = 5.0f; // Kecepatan animasi

// Variabel untuk drag backpack
bool isDraggingBackpack = false;
float dragStartY = 0.0f;
float dragStartProgress = 0.0f;

// Texture ID untuk item default
unsigned int punchTextureId = 0; // ID texture untuk punch
unsigned int wrenchTextureId = 0; // ID texture untuk wrench

// Inisialisasi backpack UI
void initBackpackUI() {
    showBackpack = false;
    backpackAnimationProgress = 0.0f;
    isDraggingBackpack = false;
    
    // Muat texture untuk punch dan wrench
    punchTextureId = loadTexture("assets/punch.png");
    wrenchTextureId = loadTexture("assets/wrench.png");
}

// Toggle tampilan backpack
void toggleBackpack() {
    showBackpack = !showBackpack;
}

// Render backpack UI yang bisa ditarik dari bawah
void renderBackpackUI(Backpack& playerBackpack) {
    // Pastikan backpack selalu memiliki punch dan wrench
    if (playerBackpack.getAllItems().empty()) {
        playerBackpack.initNewPlayerBackpack();
    }
    
    // Update animasi
    float deltaTime = ImGui::GetIO().DeltaTime;
    
    // Cek apakah sedang drag dengan mouse
    if (isDraggingBackpack) {
        // Hitung progress berdasarkan posisi mouse
        float mouseY = ImGui::GetIO().MousePos.y;
        float dragDelta = dragStartY - mouseY;
        float newProgress = dragStartProgress + (dragDelta / backpackPanelHeight);
        
        // Batasi nilai progress antara 0.0 dan 1.0
        backpackAnimationProgress = std::max(0.0f, std::min(1.0f, newProgress));
        
        // Update status showBackpack berdasarkan progress
        showBackpack = (backpackAnimationProgress > 0.5f);
        
        // Lepaskan drag jika tombol mouse dilepas
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            isDraggingBackpack = false;
            
            // Animasikan ke posisi terbuka atau tertutup penuh
            if (backpackAnimationProgress > 0.5f) {
                showBackpack = true;
            } else {
                showBackpack = false;
            }
        }
    } else {
        // Animasi normal saat tidak di-drag
        if (showBackpack) {
            backpackAnimationProgress += deltaTime * backpackAnimationSpeed;
            if (backpackAnimationProgress > 1.0f) backpackAnimationProgress = 1.0f;
        } else {
            backpackAnimationProgress -= deltaTime * backpackAnimationSpeed;
            if (backpackAnimationProgress < 0.0f) backpackAnimationProgress = 0.0f;
        }
    }
    
    // Jika tidak ada animasi dan backpack tersembunyi, tidak perlu render
    if (backpackAnimationProgress <= 0.0f) return;
    
    // Posisi panel backpack (animasi dari bawah layar)
    float yPos = SCR_HEIGHT - (backpackPanelHeight * backpackAnimationProgress);
    
    // Set posisi dan ukuran window
    ImGui::SetNextWindowPos(ImVec2(0, yPos), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(SCR_WIDTH, backpackPanelHeight), ImGuiCond_Always);
    
    // Buat window backpack
    ImGui::Begin("Backpack", NULL, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar
    );
    
    // Tampilkan handle untuk menarik backpack
    ImGui::SetCursorPosX((SCR_WIDTH - 100) / 2);
    ImGui::SetCursorPosY(0);
    
    // Area handle untuk drag
    ImVec2 handlePos = ImGui::GetCursorScreenPos();
    ImVec2 handleSize(100, 20);
    ImGui::InvisibleButton("##backpackHandle", handleSize);
    
    // Gambar handle dengan visual yang lebih jelas
    ImGui::GetWindowDrawList()->AddRectFilled(
        handlePos,
        ImVec2(handlePos.x + handleSize.x, handlePos.y + handleSize.y),
        IM_COL32(70, 70, 70, 255),
        5.0f
    );
    
    // Tambahkan teks "Backpack" di handle
    ImGui::GetWindowDrawList()->AddText(
        ImVec2(handlePos.x + (handleSize.x - ImGui::CalcTextSize("Backpack").x) / 2, 
               handlePos.y + (handleSize.y - ImGui::GetFontSize()) / 2),
        IM_COL32(255, 255, 255, 255),
        "Backpack"
    );
    
    // Tambahkan indikator drag (3 garis horizontal)
    float lineWidth = 30.0f;
    float lineSpacing = 4.0f;
    float lineY = handlePos.y + 5.0f;
    float lineX = handlePos.x + (handleSize.x - lineWidth) / 2;
    
    for (int i = 0; i < 3; i++) {
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(lineX, lineY + i * lineSpacing),
            ImVec2(lineX + lineWidth, lineY + i * lineSpacing),
            IM_COL32(200, 200, 200, 255),
            2.0f
        );
    }
    
    // Deteksi drag pada handle
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        isDraggingBackpack = true;
        dragStartY = ImGui::GetIO().MousePos.y;
        dragStartProgress = backpackAnimationProgress;
    }
    
    // Tampilkan item dalam backpack
    ImGui::Separator();
    ImGui::Columns(13, NULL, false); // 13 slot seperti di Growtopia
    
    const auto& items = playerBackpack.getAllItems();
    const ItemManager& itemManager = ItemManager::getInstance();
    
    // Render slot kosong jika belum ada item
    for (int i = 0; i < playerBackpack.getMaxSlots(); i++) {
        // Tentukan item untuk slot ini
        int itemId = 0;
        int itemCount = 0;
        
        if (i < items.size()) {
            itemId = items[i].itemId;
            itemCount = items[i].count;
        }
        
        // Buat ID unik untuk setiap slot
        ImGui::PushID(i);
        
        // Render slot item
        ImGui::BeginGroup();
        
        // Background slot
        ImVec2 slotSize(40, 40);
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            cursorPos,
            ImVec2(cursorPos.x + slotSize.x, cursorPos.y + slotSize.y),
            IM_COL32(50, 50, 50, 200),
            4.0f
        );
        
        // Jika ada item di slot ini
        if (itemId > 0) {
            // Dapatkan informasi item
            Item item = itemManager.getItem(itemId);
            
            // Untuk semua item, tampilkan button dengan label yang sesuai
            std::string buttonLabel;
            if (itemId == 1) {
                buttonLabel = "Punch";
            } else if (itemId == 50) {
                buttonLabel = "Wrench";
            } else {
                buttonLabel = std::to_string(itemId);
            }
            
            // Modifikasi bagian ini untuk mengubah item aktif saat diklik
            if (ImGui::Button(buttonLabel.c_str(), slotSize)) {
                // Set item ini sebagai item aktif
                extern int activeItemId; // Deklarasi variabel eksternal
                activeItemId = itemId;
                std::cout << "Item aktif diubah ke: " << itemId << std::endl;
            }
            
            // Tambahkan indikator visual untuk item aktif
            if (itemId == activeItemId) {
                // Gambar border untuk menandai item aktif
                ImGui::GetWindowDrawList()->AddRect(
                    cursorPos,
                    ImVec2(cursorPos.x + slotSize.x, cursorPos.y + slotSize.y),
                    IM_COL32(255, 255, 0, 255), // Warna kuning
                    4.0f,
                    0,
                    2.0f // Ketebalan garis
                );
            }
            
            // Tampilkan tooltip saat hover
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                if (itemId == 1) {
                    ImGui::Text("Punch");
                    ImGui::Text("Your fist. Good for punching things.");
                } else if (itemId == 50) {
                    ImGui::Text("Wrench");
                    ImGui::Text("A wrench for adjusting things.");
                } else {
                    ImGui::Text("%s", item.name.c_str());
                    ImGui::Text("Rarity: %d", item.rarity);
                    ImGui::Text("%s", item.description.c_str());
                }
                ImGui::EndTooltip();
            }
        }
        
        // Tampilkan jumlah item jika lebih dari 1
        if (itemCount > 1) {
            ImGui::SameLine(0, 0);
            ImGui::SetCursorPosX(cursorPos.x + slotSize.x - 15);
            ImGui::SetCursorPosY(cursorPos.y + slotSize.y - 15);
            ImGui::Text("%d", itemCount);
        }
        
        // Tutup group dan pop ID untuk slot ini
        ImGui::EndGroup();
        ImGui::PopID();
        
        // Pindah ke kolom berikutnya
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    ImGui::End();
}