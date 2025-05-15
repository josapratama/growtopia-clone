#include "../../../include/world/utils/drawing.h"

// Fungsi untuk menggambar blok
void drawBlock(ImDrawList* drawList, float x, float y, float size, ImVec4 color) {
    // Gambar blok utama
    drawList->AddRectFilled(
        ImVec2(x, y),
        ImVec2(x + size, y + size),
        ImColor(color)
    );
    
    // Tambahkan garis tepi untuk efek 3D
    drawList->AddRect(
        ImVec2(x, y),
        ImVec2(x + size, y + size),
        IM_COL32(0, 0, 0, 100),
        0.0f,
        0,
        2.0f
    );
}

// Fungsi untuk menggambar karakter pemain
void drawPlayer(ImDrawList* drawList, float x, float y, float size) {
    // Gambar kepala
    drawList->AddCircleFilled(
        ImVec2(x + size/2, y - size/2),
        size/3,
        IM_COL32(255, 220, 180, 255)
    );
    
    // Gambar badan
    drawList->AddRectFilled(
        ImVec2(x + size/3, y),
        ImVec2(x + size*2/3, y + size),
        IM_COL32(255, 0, 0, 255)
    );
    
    // Gambar tangan
    drawList->AddRectFilled(
        ImVec2(x, y),
        ImVec2(x + size/3, y + size*2/3),
        IM_COL32(255, 220, 180, 255)
    );
    
    drawList->AddRectFilled(
        ImVec2(x + size*2/3, y),
        ImVec2(x + size, y + size*2/3),
        IM_COL32(255, 220, 180, 255)
    );
    
    // Gambar kaki
    drawList->AddRectFilled(
        ImVec2(x + size/3, y + size),
        ImVec2(x + size/2, y + size*1.5f),
        IM_COL32(0, 0, 255, 255)
    );
    
    drawList->AddRectFilled(
        ImVec2(x + size/2, y + size),
        ImVec2(x + size*2/3, y + size*1.5f),
        IM_COL32(0, 0, 255, 255)
    );
}