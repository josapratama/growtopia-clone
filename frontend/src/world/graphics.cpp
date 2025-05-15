#include "../../include/world/graphics.h"
#include "../../include/inventory/item.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "imgui.h"

// Ukuran tile dalam pixel
const int TILE_SIZE = 32;

// Konstruktor
WorldGraphics::WorldGraphics() : width(100), height(60) {
    // Inisialisasi world dengan ukuran default
    initWorld();
    
    // Muat tekstur item
    loadTextures();
}
void WorldGraphics::initWorld() {
    // Inisialisasi array 2D untuk foreground dan background
    foreground.resize(width, std::vector<int>(height, 0));
    background.resize(width, std::vector<int>(height, 0));
    
    // Buat dasar dunia (tanah di bagian bawah)
    for (int x = 0; x < width; x++) {
        for (int y = height - 10; y < height; y++) {
            foreground[x][y] = 1; // ID untuk Dirt
        }
    }
    
    // Tambahkan beberapa background
    for (int x = 0; x < width; x++) {
        for (int y = height - 10; y < height; y++) {
            background[x][y] = 12; // ID untuk Dirt Background
        }
    }
}

// Muat tekstur untuk semua item
void WorldGraphics::loadTextures() {
    // Dapatkan semua item dari ItemManager
    const auto& allItems = ItemManager::getInstance().getAllItems();
    
    // Iterasi melalui semua item dan muat teksturnya
    for (const auto& pair : allItems) {
        int textureId = pair.second.textureId;
        
        // Buat path ke file tekstur
        std::string texturePath = "assets/textures/items/" + std::to_string(textureId) + ".png";
        
        // Muat tekstur
        sf::Texture texture;
        if (texture.loadFromFile(texturePath)) {
            textures[textureId] = texture;
        } else {
            std::cerr << "Gagal memuat tekstur: " << texturePath << std::endl;
        }
    }
}

// Render world ke window
void WorldGraphics::render(sf::RenderWindow& window, int cameraX, int cameraY) {
    // Hitung tile yang terlihat di layar
    int startX = std::max(0, cameraX / TILE_SIZE);
    int startY = std::max(0, cameraY / TILE_SIZE);
    
    // Perbaikan: Konversi window.getSize().x ke int untuk menghindari error tipe
    int endX = std::min(width, startX + (static_cast<int>(window.getSize().x) / TILE_SIZE) + 1);
    int endY = std::min(height, startY + (static_cast<int>(window.getSize().y) / TILE_SIZE) + 1);
    
    // Render background terlebih dahulu
    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            int bgId = background[x][y];
            if (bgId > 0) {
                // Dapatkan tekstur untuk background
                auto it = textures.find(bgId);
                if (it != textures.end()) {
                    sf::Sprite sprite(it->second);
                    sprite.setPosition(sf::Vector2f((x * TILE_SIZE) - cameraX, (y * TILE_SIZE) - cameraY));
                    window.draw(sprite);
                }
            }
        }
    }
    
    // Kemudian render foreground
    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            int fgId = foreground[x][y];
            if (fgId > 0) {
                // Dapatkan tekstur untuk foreground
                auto it = textures.find(fgId);
                if (it != textures.end()) {
                    sf::Sprite sprite(it->second);
                    sprite.setPosition(sf::Vector2f((x * TILE_SIZE) - cameraX, (y * TILE_SIZE) - cameraY));
                    window.draw(sprite);
                }
            }
        }
    }
}

// Pindahkan fungsi drawChatBubble ke luar fungsi render
void drawChatBubble(ImDrawList* drawList, float x, float y, const char* message) {
    ImFont* font = ImGui::GetFont();
    float textWidth = font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, message).x;
    float textHeight = ImGui::GetFontSize();
    
    // Gambar latar belakang chat bubble
    drawList->AddRectFilled(
        ImVec2(x - textWidth/2 - 5, y - 5),
        ImVec2(x + textWidth/2 + 5, y + textHeight + 5),
        IM_COL32(255, 255, 255, 220),
        5.0f
    );
    
    // Gambar border chat bubble
    drawList->AddRect(
        ImVec2(x - textWidth/2 - 5, y - 5),
        ImVec2(x + textWidth/2 + 5, y + textHeight + 5),
        IM_COL32(100, 100, 100, 150),
        5.0f,
        0,
        1.0f
    );
    
    // Gambar teks chat
    drawList->AddText(
        ImVec2(x - textWidth/2, y),
        IM_COL32(0, 0, 0, 255),
        message
    );
    
    // Gambar segitiga kecil di bawah bubble
    drawList->AddTriangleFilled(
        ImVec2(x, y + textHeight + 5),
        ImVec2(x - 5, y + textHeight + 10),
        ImVec2(x + 5, y + textHeight + 10),
        IM_COL32(255, 255, 255, 220)
    );
}

// Mendapatkan item di posisi tertentu
int WorldGraphics::getItemAt(int x, int y, bool isForeground) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        if (isForeground) {
            return foreground[x][y];
        } else {
            return background[x][y];
        }
    }
    return 0;
}

// Menempatkan item di posisi tertentu
bool WorldGraphics::placeItem(int x, int y, int itemId, bool isForeground) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        // Dapatkan informasi item
        Item item = ItemManager::getInstance().getItem(itemId);
        
        // Periksa apakah item valid
        if (item.id > 0) {
            // Periksa apakah tipe item sesuai dengan layer yang dipilih
            bool isItemBackground = item.isBackground;
            if (isItemBackground != isForeground) {
                if (isForeground) {
                    foreground[x][y] = itemId;
                } else {
                    background[x][y] = itemId;
                }
                return true;
            }
        }
    }
    return false;
}

// Menghapus item di posisi tertentu
bool WorldGraphics::removeItem(int x, int y, bool isForeground) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        if (isForeground) {
            if (foreground[x][y] > 0) {
                foreground[x][y] = 0;
                return true;
            }
        } else {
            if (background[x][y] > 0) {
                background[x][y] = 0;
                return true;
            }
        }
    }
    return false;
}

// Menyimpan world ke file
bool WorldGraphics::saveWorld(const std::string& filename) {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Simpan ukuran world
        file.write(reinterpret_cast<const char*>(&width), sizeof(width));
        file.write(reinterpret_cast<const char*>(&height), sizeof(height));
        
        // Simpan data foreground
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                file.write(reinterpret_cast<const char*>(&foreground[x][y]), sizeof(foreground[x][y]));
            }
        }
        
        // Simpan data background
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                file.write(reinterpret_cast<const char*>(&background[x][y]), sizeof(background[x][y]));
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saat menyimpan world: " << e.what() << std::endl;
        return false;
    }
}

// Memuat world dari file
bool WorldGraphics::loadWorld(const std::string& filename) {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Baca ukuran world
        int newWidth, newHeight;
        file.read(reinterpret_cast<char*>(&newWidth), sizeof(newWidth));
        file.read(reinterpret_cast<char*>(&newHeight), sizeof(newHeight));
        
        // Resize array jika ukuran berbeda
        if (newWidth != width || newHeight != height) {
            width = newWidth;
            height = newHeight;
            foreground.resize(width, std::vector<int>(height, 0));
            background.resize(width, std::vector<int>(height, 0));
        }
        
        // Baca data foreground
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                file.read(reinterpret_cast<char*>(&foreground[x][y]), sizeof(foreground[x][y]));
            }
        }
        
        // Baca data background
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                file.read(reinterpret_cast<char*>(&background[x][y]), sizeof(background[x][y]));
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saat memuat world: " << e.what() << std::endl;
        return false;
    }
}

// Implementasi fungsi drawBlock
void drawBlock(ImDrawList* drawList, float x, float y, ImVec4 color) {
    drawList->AddRectFilled(
        ImVec2(x, y),
        ImVec2(x + TILE_SIZE, y + TILE_SIZE),
        ImColor(color)
    );
    
    // Tambahkan garis tepi untuk efek 3D
    drawList->AddRect(
        ImVec2(x, y),
        ImVec2(x + TILE_SIZE, y + TILE_SIZE),
        IM_COL32(0, 0, 0, 100),
        0.0f,
        0,
        2.0f
    );
}