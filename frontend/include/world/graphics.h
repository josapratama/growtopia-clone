#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include "imgui.h"  // Tambahkan include ini untuk ImDrawList

// Forward declaration
class Item;

class WorldGraphics {
private:
    int width;
    int height;
    std::vector<std::vector<int>> foreground;
    std::vector<std::vector<int>> background;
    std::map<int, sf::Texture> textures;

public:
    // Konstruktor
    WorldGraphics();
    
    // Inisialisasi world kosong
    void initWorld();
    
    // Muat tekstur untuk semua item
    void loadTextures();
    
    // Render world ke window
    void render(sf::RenderWindow& window, int cameraX, int cameraY);
    
    // Mendapatkan item di posisi tertentu
    int getItemAt(int x, int y, bool isForeground) const;
    
    // Menempatkan item di posisi tertentu
    bool placeItem(int x, int y, int itemId, bool isForeground);
    
    // Menghapus item di posisi tertentu
    bool removeItem(int x, int y, bool isForeground);
    
    // Menyimpan world ke file
    bool saveWorld(const std::string& filename);
    
    // Memuat world dari file
    bool loadWorld(const std::string& filename);
};

// Fungsi untuk menggambar chat bubble
void drawChatBubble(ImDrawList* drawList, float x, float y, const char* message);

// Fungsi untuk menggambar block
void drawBlock(ImDrawList* drawList, float x, float y, ImVec4 color);