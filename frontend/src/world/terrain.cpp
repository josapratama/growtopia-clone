#include "../../include/world/terrain.h"
#include "../../include/world/constants.h"
#include "../../include/world/globals.h"
#include <iostream> // Tambahkan ini untuk std::cout
#include <map>
#include <utility>
#include "imgui.h"
#include "../../include/world/utils/drawing.h"
#include "../../include/core/config.h"
#include "../../include/inventory/item.h"
#include <random>

// Generator angka acak
std::random_device rd;
std::mt19937 gen(rd());

// Render latar belakang langit
void renderSky(ImDrawList* drawList) {
    drawList->AddRectFilled(
        ImVec2(0, 0),
        ImVec2(SCR_WIDTH, SCR_HEIGHT * TERRAIN_HEIGHT_RATIO),
        ImColor(skyColor)
    );
}

void renderGroundTerrain(ImDrawList* drawList) {
    // ID item untuk dirt (sesuai database_items.cpp)
    const int DIRT_ID = 1;        // Dirt block
    
    // Warna untuk dirt
    ImColor dirtColor = ImColor(139, 69, 19, 255); // Brown
    
    // Render ground terrain dengan dirt
    for (int x = 0; x < WORLD_WIDTH; x++) {
        float worldX = x * BLOCK_SIZE - cameraX;
        
        // Hanya render blok yang terlihat di layar
        if (worldX < -BLOCK_SIZE || worldX > SCR_WIDTH)
            continue;
        
        // Tentukan ketinggian permukaan tanah
        int surfaceHeight = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO;
        
        // Gambar lapisan rumput di bagian atas tanah
        drawList->AddRectFilled(
            ImVec2(worldX, surfaceHeight),
            ImVec2(worldX + BLOCK_SIZE, surfaceHeight + BLOCK_SIZE),
            ImColor(76, 153, 0, 255) // Warna rumput
        );
        
        // Tambahkan white door di posisi tengah world (sekitar x = WORLD_WIDTH/2)
        if (x >= WORLD_WIDTH/2 - 1 && x <= WORLD_WIDTH/2) {
            // Gambar white door (2 blok lebar, 3 blok tinggi)
            float doorX = worldX;
            float doorY = surfaceHeight - BLOCK_SIZE * 3; // 3 blok di atas permukaan tanah
            
            // Frame pintu (putih)
            drawList->AddRectFilled(
                ImVec2(doorX, doorY),
                ImVec2(doorX + BLOCK_SIZE, doorY + BLOCK_SIZE * 3),
                ImColor(255, 255, 255, 255) // Putih
            );
            
            // Bagian dalam pintu (abu-abu muda)
            drawList->AddRectFilled(
                ImVec2(doorX + BLOCK_SIZE * 0.1f, doorY + BLOCK_SIZE * 0.1f),
                ImVec2(doorX + BLOCK_SIZE * 0.9f, doorY + BLOCK_SIZE * 2.9f),
                ImColor(220, 220, 220, 255) // Abu-abu muda
            );
            
            // Gagang pintu (hitam)
            drawList->AddRectFilled(
                ImVec2(doorX + BLOCK_SIZE * 0.7f, doorY + BLOCK_SIZE * 1.5f),
                ImVec2(doorX + BLOCK_SIZE * 0.9f, doorY + BLOCK_SIZE * 1.7f),
                ImColor(50, 50, 50, 255) // Hitam
            );
        }
        
        // Dalam fungsi renderGroundTerrain, ubah bagian pengecekan blockHealth
        
        // Gambar lapisan tanah (dirt)
        for (int y = 1; y < 10; y++) {
            float blockY = surfaceHeight + y * BLOCK_SIZE;
            
            // Cek apakah block ini sudah dipukul
            std::pair<int, int> blockKey = std::make_pair(x, y);
            bool blockExists = true;
            
            // Jika block ada di map dan health-nya 0 atau kurang, jangan render
            if (blockHealth.find(blockKey) != blockHealth.end() && blockHealth[blockKey] <= 0) {
                blockExists = false;
                std::cout << "Block at x=" << x << ", y=" << y << " is destroyed, not rendering" << std::endl;
            }
            
            // Hanya render jika block masih ada
            if (blockExists) {
                // Tentukan warna berdasarkan health
                ImColor currentDirtColor = dirtColor;
                
                // Jika block sudah dipukul, ubah warnanya menjadi lebih terang
                if (blockHealth.find(blockKey) != blockHealth.end()) {
                    int health = blockHealth[blockKey];
                    if (health == 2) {
                        currentDirtColor = ImColor(159, 89, 39, 255); // Lebih terang
                    } else if (health == 1) {
                        currentDirtColor = ImColor(179, 109, 59, 255); // Paling terang
                    }
                }
                
                // Gambar dirt block
                drawBlock(
                    drawList,
                    worldX,
                    blockY,
                    BLOCK_SIZE,
                    currentDirtColor
                );
                
                // Tambahkan variasi visual dengan menambahkan beberapa detail
                if ((x + y) % 5 == 0) {
                    // Tambahkan batu kecil atau detail lain pada beberapa blok dirt
                    drawList->AddRectFilled(
                        ImVec2(worldX + BLOCK_SIZE * 0.3f, blockY + BLOCK_SIZE * 0.3f),
                        ImVec2(worldX + BLOCK_SIZE * 0.5f, blockY + BLOCK_SIZE * 0.5f),
                        ImColor(100, 100, 100, 150) // Abu-abu untuk batu
                    );
                }
            }
        }
    }
}

// Render platform melayang dengan variasi blok
void renderFloatingPlatforms(ImDrawList* drawList) {
    // Di dalam fungsi renderGroundTerrain dan renderFloatingPlatforms
    // Tambahkan seed tetap untuk random generator agar tidak berubah setiap frame
    
    // Di bagian atas file, tambahkan:
    static std::mt19937 gen(12345); // Seed tetap untuk konsistensi
    
    // Dan gunakan gen ini untuk semua distribusi acak
    std::uniform_int_distribution<> dist(1, 100); // Variabel blockTypeDist diganti menjadi dist
    
    for (int i = 0; i < 15; i++) {
        float platformX = (SCR_WIDTH * 0.2f + i * SCR_WIDTH * 0.15f) - cameraX;
        float platformY = SCR_HEIGHT * 0.5f - i % 5 * 20.0f;
        
        // Hanya render platform yang terlihat di layar
        if (platformX < -BLOCK_SIZE * 3 || platformX > SCR_WIDTH)
            continue;
        
        // Tentukan tipe platform (rock, dirt, atau lava)
        ImColor platformColor;
        int randValue = dist(gen); // Menggunakan dist
        
        if (randValue <= 70) {
            platformColor = ImColor(139, 69, 19, 255); // Dirt (brown)
        } else if (randValue <= 95) {
            platformColor = ImColor(105, 105, 105, 255); // Rock (gray)
        } else {
            platformColor = ImColor(255, 69, 0, 255); // Lava (red-orange)
        }
        
        for (int j = 0; j < 3; j++) {
            drawBlock(
                drawList,
                platformX + j * BLOCK_SIZE,
                platformY,
                BLOCK_SIZE,
                platformColor
            );
        }
    }
}

// Tambahkan di bagian bawah file

// Struktur data untuk menyimpan health block
#include <map>
#include <utility>
// Hapus baris berikut karena sudah didefinisikan di globals.cpp
// std::map<std::pair<int, int>, int> blockHealth;

// Hapus implementasi kedua dari punchBlock (yang menerima parameter int)
// dan perbaiki implementasi yang menerima parameter float

// Fungsi untuk memukul block pada koordinat tertentu
bool punchBlock(float x, float y) { 
    // Debug output untuk melihat koordinat input
    std::cout << "Mouse click at: x=" << x << ", y=" << y << std::endl;
    
    // Konversi koordinat layar ke koordinat world 
    int worldX = (x) / BLOCK_SIZE; 
    int worldY = y / BLOCK_SIZE; 
    
    // Debug output untuk melihat koordinat tile
    std::cout << "Tile coordinates: worldX=" << worldX << ", worldY=" << worldY << std::endl;
    
    // Pastikan koordinat valid 
    if (worldX < 0 || worldX >= WORLD_WIDTH || worldY < 0) { 
        std::cout << "Invalid coordinates" << std::endl;
        return false; 
    } 
    
    // Hitung ketinggian permukaan tanah dalam tile
    int surfaceHeightTile = (SCR_HEIGHT * TERRAIN_HEIGHT_RATIO) / BLOCK_SIZE;
    
    // Debug output untuk melihat ketinggian permukaan
    std::cout << "Surface height in tiles: " << surfaceHeightTile << std::endl;
    
    // Hanya proses block yang berada di bawah permukaan tanah (dirt blocks) 
    if (worldY < surfaceHeightTile) { 
        std::cout << "Click above ground surface" << std::endl;
        return false; 
    } 
    
    // Hitung koordinat Y relatif terhadap permukaan tanah 
    int relativeY = worldY - surfaceHeightTile; 
    
    // Hanya proses dirt blocks (y >= 1, karena y=0 adalah rumput) 
    if (relativeY < 1 || relativeY >= 10) { 
        std::cout << "Click not on dirt layer" << std::endl;
        return false; 
    } 
    
    // Debug output untuk melihat koordinat relatif
    std::cout << "Punch at worldX: " << worldX << ", relativeY: " << relativeY << std::endl; 
    
    // Buat key untuk block ini 
    std::pair<int, int> blockKey = std::make_pair(worldX, relativeY); 
    
    // Jika block belum ada di map, inisialisasi dengan health 3 
    if (blockHealth.find(blockKey) == blockHealth.end()) { 
        blockHealth[blockKey] = 3; 
        std::cout << "New block initialized with health 3" << std::endl; 
    } 
    
    // Kurangi health block 
    blockHealth[blockKey]--; 
    std::cout << "Block health reduced to: " << blockHealth[blockKey] << std::endl; 
    
    // Jika health mencapai 0, block hancur 
    if (blockHealth[blockKey] <= 0) { 
        blockHealth.erase(blockKey); 
        std::cout << "Block destroyed!" << std::endl; 
        return true; // Block hancur 
    } 
    
    return false; // Block belum hancur 
}

// Hapus implementasi kedua punchBlock(int blockX, int blockY) yang ada di bawah
bool punchBlock(int blockX, int blockY) {
    // Konversi koordinat layar ke koordinat world
    int worldX = (blockX + cameraX) / BLOCK_SIZE;
    int worldY = blockY / BLOCK_SIZE;
    
    // Pastikan koordinat valid
    if (worldX < 0 || worldX >= WORLD_WIDTH || worldY < 0) {
        return false;
    }
    
    // Hitung ketinggian permukaan tanah
    int surfaceHeight = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO;
    
    // Hanya proses block yang berada di bawah permukaan tanah (dirt blocks)
    if (blockY < surfaceHeight) {
        return false;
    }
    
    // Hitung koordinat Y relatif terhadap permukaan tanah
    int relativeY = (blockY - surfaceHeight) / BLOCK_SIZE;
    
    // Hanya proses dirt blocks (y >= 1, karena y=0 adalah rumput)
    if (relativeY < 1 || relativeY >= 10) {
        return false;
    }
    
    // Buat key untuk block ini
    std::pair<int, int> blockKey = std::make_pair(worldX, relativeY);
    
    // Jika block belum ada di map, inisialisasi dengan health 3
    if (blockHealth.find(blockKey) == blockHealth.end()) {
        blockHealth[blockKey] = 3;
    }
    
    // Kurangi health block
    blockHealth[blockKey]--;
    
    // Jika health mencapai 0, block hancur
    if (blockHealth[blockKey] <= 0) {
        blockHealth.erase(blockKey);
        return true; // Block hancur
    }
    
    return false; // Block belum hancur
}
