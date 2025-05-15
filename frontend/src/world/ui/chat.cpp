#include "../../../include/world/ui/chat.h"
#include "../../../include/world/constants.h"
#include "../../../include/world/globals.h"
#include "../../../include/core/config.h"
#include <cstring>

// Deklarasi variabel eksternal dari player.cpp
extern std::string playerName;
extern bool isTyping;

// Proses input chat
// Proses input chat
void processChatInput() {
    // Hapus deteksi tombol T untuk membuka chat
    // Sekarang chat hanya bisa dibuka dengan tombol Chat di UI
    
    // Tampilkan input chat jika aktif
    if (showChatInput) {
        ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH/2 - 150, SCR_HEIGHT - 100), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300, 50), ImGuiCond_Always);
        ImGui::Begin("ChatInput", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        ImGui::Text("Chat:");
        ImGui::SameLine();
        ImGui::PushItemWidth(230);
        
        // Set keyboard focus ke input text
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        
        // Tetap menggunakan Enter untuk mengirim chat
        bool enterPressed = ImGui::InputText("##chatinput", chatInput, IM_ARRAYSIZE(chatInput), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopItemWidth();
        
        if (enterPressed && strlen(chatInput) > 0) {
            // Simpan pesan chat dan tampilkan di atas kepala pemain
            currentChatMessage = chatInput;
            chatMessageTimer = CHAT_DISPLAY_TIME;
            
            // Reset input chat dan tutup window
            memset(chatInput, 0, sizeof(chatInput));
            showChatInput = false;
            isTyping = false;
        }
        
        // Tutup chat input jika Escape ditekan
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            showChatInput = false;
            isTyping = false;
        }
        
        ImGui::End();
    }
}

// Render chat bubble di atas kepala pemain
void renderChatBubble(ImDrawList* drawList, float playerScreenX) {
    if (chatMessageTimer > 0.0f) {
        ImFont* font = ImGui::GetFont();
        float textWidth = font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, currentChatMessage.c_str()).x;
        float textX = playerScreenX - textWidth / 2;
        float textY = playerY - BLOCK_SIZE - 20.0f;
        
        // Latar belakang chat bubble
        drawList->AddRectFilled(
            ImVec2(textX - 5, textY - 5),
            ImVec2(textX + textWidth + 5, textY + ImGui::GetFontSize() + 5),
            IM_COL32(255, 255, 255, 200),
            5.0f
        );
        
        // Teks chat
        drawList->AddText(
            ImVec2(textX, textY),
            IM_COL32(0, 0, 0, 255),
            currentChatMessage.c_str()
        );
        
        // Kurangi timer chat
        chatMessageTimer -= ImGui::GetIO().DeltaTime;
    }
}