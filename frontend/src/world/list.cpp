#include "../../include/world/list.h"
#include "../../include/world/graphics.h"
#include "../../include/world/renderer.h"
#include "../../include/core/config.h"
#include "../../include/world/globals.h" // Tambahkan include untuk globals.h
#include "../../include/world/player.h" // Tambahkan include untuk player.h
#include "../../include/world/constants.h" // Tambahkan include untuk constants.h
#include "imgui.h"
#include <iostream>
#include <algorithm> // Untuk std::find

std::vector<WorldInfo> worldList;
std::vector<std::string> myWorlds; // Daftar world milik user
std::vector<std::string> recentWorlds; // Daftar world yang baru dikunjungi

ImVec4 playerColor = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);

// Enum untuk filter kategori
enum WorldCategory {
    ALL,
    POPULAR,
    FEATURED,
    GAMES,
    TRADE
};

bool renderWorldListPage(AppPage& currentPage) {
    // Jika sedang dalam tampilan world, render tampilan world
    if (inWorldView) {
        renderWorldView(currentPage);
        return true;
    }
    
    // Jika belum ada data world, tambahkan beberapa contoh
    if (worldList.empty()) {
        worldList.push_back({"MAIN", "System", 42, false});
        worldList.push_back({"START", "System", 15, false});
        worldList.push_back({"TRADE", "System", 28, false});
        worldList.push_back({"SHOP", "Admin", 10, true});
        worldList.push_back({"FARM", "Player1", 3, true});
        worldList.push_back({"CASINO", "Player2", 8, true});
        worldList.push_back({"PARKOUR", "Player3", 5, false});
        worldList.push_back({"GAMES", "System", 20, false});
        worldList.push_back({"MARKET", "Admin", 18, true});
        worldList.push_back({"SCIENCE", "Player4", 7, false});
        worldList.push_back({"MUSIC", "Player5", 12, false});
        
        // Tambahkan beberapa world ke daftar my worlds
        myWorlds.push_back("FARM");
        myWorlds.push_back("MYWORLD");
        
        // Tambahkan beberapa world ke daftar recently visited
        recentWorlds.push_back("MAIN");
        recentWorlds.push_back("TRADE");
        recentWorlds.push_back("SHOP");
    }
    
    // Mengatur posisi dan ukuran jendela
    ImGui::SetNextWindowPos(ImVec2(SCR_WIDTH/2 - 300, SCR_HEIGHT/2 - 250), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("World List", NULL, ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Growtopia Worlds");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Input untuk mencari world dengan placeholder text
    static char searchWorld[64] = "";
    static WorldCategory currentCategory = ALL;
    
    // Tampilkan label "World Name:" di atas input
    ImGui::Text("World Name:");
    
    // Input text dengan placeholder
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.7f);
    bool enterPressed = ImGui::InputText("##worldname", searchWorld, IM_ARRAYSIZE(searchWorld), ImGuiInputTextFlags_EnterReturnsTrue);
    
    // Jika input kosong, tampilkan placeholder text
    if (strlen(searchWorld) == 0 && !ImGui::IsItemActive()) {
        ImVec2 pos = ImGui::GetItemRectMin();
        pos.x += 5; // Padding
        pos.y += 2; // Padding
        ImGui::GetWindowDrawList()->AddText(pos, IM_COL32(128, 128, 128, 200), "Hit enter or click 'Create World' to make this world!");
    }
    ImGui::PopItemWidth();
    
    ImGui::SameLine();
    bool goClicked = ImGui::Button("Create World", ImVec2(100, 0));
    
    // Proses pencarian/pembuatan world
    if ((enterPressed || goClicked) && strlen(searchWorld) > 0) {
        // Cek apakah world sudah ada di daftar
        bool worldFound = false;
        for (const auto& world : worldList) {
            if (world.name == searchWorld) {
                currentWorldName = world.name;
                worldFound = true;
                inWorldView = true;
                
                // Tambahkan ke daftar recently visited jika belum ada
                if (std::find(recentWorlds.begin(), recentWorlds.end(), world.name) == recentWorlds.end()) {
                    // Batasi jumlah recent worlds ke 5
                    if (recentWorlds.size() >= 5) {
                        recentWorlds.pop_back();
                    }
                    recentWorlds.insert(recentWorlds.begin(), world.name);
                }
                
                // Posisikan player di white door
                playerX = WORLD_WIDTH / 2 * BLOCK_SIZE; // Posisi X pintu (tengah world)
                playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3; // Posisi Y pintu
                
                // Reset velocity
                playerVelocityX = 0.0f;
                playerVelocityY = 0.0f;
                
                // Set status player (di dalam world)
                isInsideWorld = true;
                
                break;
            }
        }
        
        // Jika world tidak ditemukan, tambahkan ke daftar
        if (!worldFound && strlen(searchWorld) > 0) {
            worldList.push_back({searchWorld, "You", 1, false});
            currentWorldName = searchWorld;
            inWorldView = true;
            
            // Tambahkan ke daftar my worlds
            myWorlds.push_back(searchWorld);
            
            // Tambahkan ke daftar recently visited
            if (recentWorlds.size() >= 5) {
                recentWorlds.pop_back();
            }
            recentWorlds.insert(recentWorlds.begin(), searchWorld);
            
            // Posisikan player di white door
            playerX = WORLD_WIDTH / 2 * BLOCK_SIZE; // Posisi X pintu (tengah world)
            playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3; // Posisi Y pintu
            
            // Reset velocity
            playerVelocityX = 0.0f;
            playerVelocityY = 0.0f;
            
            // Set status player (di dalam world)
            isInsideWorld = true;
        }
    }
    
    ImGui::Spacing();
    
    // Tombol filter kategori
    ImGui::Text("Filter:");
    ImGui::SameLine();
    if (ImGui::Button("All", ImVec2(60, 25))) currentCategory = ALL;
    ImGui::SameLine();
    if (ImGui::Button("Popular", ImVec2(70, 25))) currentCategory = POPULAR;
    ImGui::SameLine();
    if (ImGui::Button("Featured", ImVec2(70, 25))) currentCategory = FEATURED;
    ImGui::SameLine();
    if (ImGui::Button("Games", ImVec2(60, 25))) currentCategory = GAMES;
    ImGui::SameLine();
    if (ImGui::Button("Trade", ImVec2(60, 25))) currentCategory = TRADE;
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Top Worlds:");
    ImGui::Spacing();
    
    // Tampilkan top worlds dalam grid layout (3x3 atau 3x4)
    float windowWidth = ImGui::GetWindowWidth();
    float itemWidth = windowWidth / 3 - 20; // 3 kolom dengan padding
    
    ImGui::Columns(3, NULL, false);
    
    // Filter world berdasarkan kategori
    std::vector<WorldInfo> filteredWorlds;
    for (const auto& world : worldList) {
        bool shouldInclude = false;
        
        switch (currentCategory) {
            case ALL:
                shouldInclude = true;
                break;
            case POPULAR:
                shouldInclude = world.players > 10;
                break;
            case FEATURED:
                shouldInclude = world.owner == "System";
                break;
            case GAMES:
                shouldInclude = world.name == "GAMES" || world.name == "CASINO" || world.name == "PARKOUR";
                break;
            case TRADE:
                shouldInclude = world.name == "TRADE" || world.name == "MARKET" || world.name == "SHOP";
                break;
        }
        
        if (shouldInclude) {
            filteredWorlds.push_back(world);
        }
    }
    
    // Tampilkan maksimal 9 world
    int maxWorlds = std::min(9, static_cast<int>(filteredWorlds.size()));
    for (int i = 0; i < maxWorlds; i++) {
        const auto& world = filteredWorlds[i];
        
        // Buat card untuk world (tanpa BeginChild/EndChild untuk menghilangkan scroll)
        ImGui::PushID(i);
        bool cardClicked = ImGui::InvisibleButton(("##card_" + world.name).c_str(), ImVec2(itemWidth - 10, 40));
        
        // Gambar isi card di atas invisible button
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // Warna background card
        ImU32 cardColor = IM_COL32(50, 50, 70, 255);
        ImU32 textColor = IM_COL32(255, 255, 255, 255);
        
        // Jika world terkunci, gunakan warna berbeda
        if (world.isLocked) {
            cardColor = IM_COL32(70, 50, 50, 255);
        }
        
        // Jika card dihover, buat lebih terang
        if (ImGui::IsItemHovered()) {
            cardColor = IM_COL32(70, 70, 100, 255);
            if (world.isLocked) {
                cardColor = IM_COL32(100, 70, 70, 255);
            }
        }
        
        // Gambar background card
        drawList->AddRectFilled(rectMin, rectMax, cardColor, 4.0f);
        
        // Nama world dan jumlah player dalam satu baris
        std::string displayName = world.name;
        if (world.isLocked) {
            displayName += " 🔒";
        }
        
        // Hitung lebar teks nama world
        ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
        
        // Gambar nama world (di sebelah kiri)
        drawList->AddText(ImVec2(rectMin.x + 10, rectMin.y + (rectMax.y - rectMin.y - textSize.y) / 2), 
                          textColor, displayName.c_str());
        
        // Gambar jumlah player (di sebelah kanan)
        std::string playerCount = "(" + std::to_string(world.players) + ")";
        ImVec2 countSize = ImGui::CalcTextSize(playerCount.c_str());
        drawList->AddText(ImVec2(rectMax.x - countSize.x - 10, rectMin.y + (rectMax.y - rectMin.y - countSize.y) / 2), 
                          textColor, playerCount.c_str());
        
        // Jika card diklik, masuk ke world
        if (cardClicked) {
            currentWorldName = world.name;
            inWorldView = true;
            
            // Tambahkan ke daftar recently visited
            if (std::find(recentWorlds.begin(), recentWorlds.end(), world.name) == recentWorlds.end()) {
                if (recentWorlds.size() >= 5) {
                    recentWorlds.pop_back();
                }
                recentWorlds.insert(recentWorlds.begin(), world.name);
            }
            
            // Posisikan player di white door
            playerX = WORLD_WIDTH / 2 * BLOCK_SIZE;
            playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3;
            playerVelocityX = 0.0f;
            playerVelocityY = 0.0f;
            isInsideWorld = true;
        }
        
        ImGui::PopID();
        
        // Pindah ke kolom berikutnya
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("My Worlds:");
    ImGui::Spacing();
    
    // Tampilkan my worlds dalam satu baris dengan scrolling horizontal untuk container (bukan card)
    ImGui::BeginChild("MyWorldsScroll", ImVec2(ImGui::GetWindowWidth() - 20, 60), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::Columns(myWorlds.size() > 0 ? myWorlds.size() : 1, NULL, false);
    
    if (myWorlds.empty()) {
        ImGui::Text("You don't have any worlds yet.");
    } else {
        for (size_t i = 0; i < myWorlds.size(); i++) {
            const std::string& worldName = myWorlds[i];
            
            // Cari informasi world
            WorldInfo* worldInfo = nullptr;
            for (auto& world : worldList) {
                if (world.name == worldName) {
                    worldInfo = &world;
                    break;
                }
            }
            
            if (worldInfo) {
                // Buat card untuk world
                ImGui::PushID(("my_" + worldName).c_str());
                bool cardClicked = ImGui::InvisibleButton("##mycard", ImVec2(120, 40));
                
                // Gambar isi card
                ImVec2 rectMin = ImGui::GetItemRectMin();
                ImVec2 rectMax = ImGui::GetItemRectMax();
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                
                // Warna background card
                ImU32 cardColor = IM_COL32(50, 70, 50, 255); // Warna berbeda untuk My Worlds
                ImU32 textColor = IM_COL32(255, 255, 255, 255);
                
                // Jika world terkunci, gunakan warna berbeda
                if (worldInfo->isLocked) {
                    cardColor = IM_COL32(70, 70, 50, 255);
                }
                
                // Jika card dihover, buat lebih terang
                if (ImGui::IsItemHovered()) {
                    cardColor = IM_COL32(70, 100, 70, 255);
                    if (worldInfo->isLocked) {
                        cardColor = IM_COL32(100, 100, 70, 255);
                    }
                }
                
                // Gambar background card
                drawList->AddRectFilled(rectMin, rectMax, cardColor, 4.0f);
                
                // Nama world dan jumlah player dalam satu baris
                std::string displayName = worldName;
                if (worldInfo->isLocked) {
                    displayName += " 🔒";
                }
                
                // Hitung lebar teks nama world
                ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
                
                // Gambar nama world (di sebelah kiri)
                drawList->AddText(ImVec2(rectMin.x + 10, rectMin.y + (rectMax.y - rectMin.y - textSize.y) / 2), 
                                  textColor, displayName.c_str());
                
                // Gambar jumlah player (di sebelah kanan)
                std::string playerCount = "(" + std::to_string(worldInfo->players) + ")";
                ImVec2 countSize = ImGui::CalcTextSize(playerCount.c_str());
                drawList->AddText(ImVec2(rectMax.x - countSize.x - 10, rectMin.y + (rectMax.y - rectMin.y - countSize.y) / 2), 
                                  textColor, playerCount.c_str());
                
                // Jika card diklik, masuk ke world
                if (cardClicked) {
                    currentWorldName = worldName;
                    inWorldView = true;
                    
                    // Tambahkan ke daftar recently visited
                    if (std::find(recentWorlds.begin(), recentWorlds.end(), worldName) == recentWorlds.end()) {
                        if (recentWorlds.size() >= 5) {
                            recentWorlds.pop_back();
                        }
                        recentWorlds.insert(recentWorlds.begin(), worldName);
                    }
                    
                    // Posisikan player di white door
                    playerX = WORLD_WIDTH / 2 * BLOCK_SIZE;
                    playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3;
                    playerVelocityX = 0.0f;
                    playerVelocityY = 0.0f;
                    isInsideWorld = true;
                }
                
                ImGui::PopID();
            } else {
                ImGui::Text("%s", worldName.c_str());
                ImGui::Text("(Not found)");
            }
            
            ImGui::NextColumn();
        }
    }
    
    ImGui::Columns(1);
    ImGui::EndChild();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Recently Visited:");
    ImGui::Spacing();
    
    // Tampilkan recently visited worlds dalam satu baris dengan scrolling horizontal untuk container (bukan card)
    ImGui::BeginChild("RecentWorldsScroll", ImVec2(ImGui::GetWindowWidth() - 20, 60), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::Columns(recentWorlds.size() > 0 ? recentWorlds.size() : 1, NULL, false);
    
    if (recentWorlds.empty()) {
        ImGui::Text("No recently visited worlds.");
    } else {
        for (size_t i = 0; i < recentWorlds.size(); i++) {
            const std::string& worldName = recentWorlds[i];
            
            // Cari informasi world
            WorldInfo* worldInfo = nullptr;
            for (auto& world : worldList) {
                if (world.name == worldName) {
                    worldInfo = &world;
                    break;
                }
            }
            
            if (worldInfo) {
                // Buat card untuk world
                ImGui::PushID(("recent_" + worldName).c_str());
                bool cardClicked = ImGui::InvisibleButton("##recentcard", ImVec2(120, 40));
                
                // Gambar isi card
                ImVec2 rectMin = ImGui::GetItemRectMin();
                ImVec2 rectMax = ImGui::GetItemRectMax();
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                
                // Warna background card
                ImU32 cardColor = IM_COL32(50, 50, 80, 255); // Warna berbeda untuk Recent Worlds
                ImU32 textColor = IM_COL32(255, 255, 255, 255);
                
                // Jika world terkunci, gunakan warna berbeda
                if (worldInfo->isLocked) {
                    cardColor = IM_COL32(70, 50, 80, 255);
                }
                
                // Jika card dihover, buat lebih terang
                if (ImGui::IsItemHovered()) {
                    cardColor = IM_COL32(70, 70, 120, 255);
                    if (worldInfo->isLocked) {
                        cardColor = IM_COL32(100, 70, 120, 255);
                    }
                }
                
                // Gambar background card
                drawList->AddRectFilled(rectMin, rectMax, cardColor, 4.0f);
                
                // Nama world dan jumlah player dalam satu baris
                std::string displayName = worldName;
                if (worldInfo->isLocked) {
                    displayName += " 🔒";
                }
                
                // Hitung lebar teks nama world
                ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
                
                // Gambar nama world (di sebelah kiri)
                drawList->AddText(ImVec2(rectMin.x + 10, rectMin.y + (rectMax.y - rectMin.y - textSize.y) / 2), 
                                  textColor, displayName.c_str());
                
                // Gambar jumlah player (di sebelah kanan)
                std::string playerCount = "(" + std::to_string(worldInfo->players) + ")";
                ImVec2 countSize = ImGui::CalcTextSize(playerCount.c_str());
                drawList->AddText(ImVec2(rectMax.x - countSize.x - 10, rectMin.y + (rectMax.y - rectMin.y - countSize.y) / 2), 
                                  textColor, playerCount.c_str());
                
                // Jika card diklik, masuk ke world
                if (cardClicked) {
                    currentWorldName = worldName;
                    inWorldView = true;
                    
                    // Posisikan player di white door
                    playerX = WORLD_WIDTH / 2 * BLOCK_SIZE;
                    playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3;
                    playerVelocityX = 0.0f;
                    playerVelocityY = 0.0f;
                    isInsideWorld = true;
                }
                
                ImGui::PopID();
            } else {
                ImGui::Text("%s", worldName.c_str());
                ImGui::Text("(Not found)");
            }
            
            ImGui::NextColumn();
        }
    }
    
    ImGui::Columns(1);
    ImGui::EndChild();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Tombol navigasi di bagian bawah
    float buttonWidth = (ImGui::GetWindowWidth() - 40) / 3;
    
    if (ImGui::Button("Back", ImVec2(buttonWidth, 30))) {
        currentPage = MAIN_MENU;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Home", ImVec2(buttonWidth, 30))) {
        // Masuk ke world HOME atau MAIN
        currentWorldName = "MAIN";
        inWorldView = true;
        
        // Tambahkan ke daftar recently visited
        if (std::find(recentWorlds.begin(), recentWorlds.end(), "MAIN") == recentWorlds.end()) {
            if (recentWorlds.size() >= 5) {
                recentWorlds.pop_back();
            }
            recentWorlds.insert(recentWorlds.begin(), "MAIN");
        }
        
        // Posisikan player di white door
        playerX = WORLD_WIDTH / 2 * BLOCK_SIZE;
        playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3;
        playerVelocityX = 0.0f;
        playerVelocityY = 0.0f;
        isInsideWorld = true;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Enter World", ImVec2(buttonWidth, 30))) {
        // Masuk ke world yang dipilih di kolom pencarian
        if (strlen(searchWorld) > 0) {
            // Cek apakah world sudah ada di daftar
            bool worldFound = false;
            for (const auto& world : worldList) {
                if (world.name == searchWorld) {
                    currentWorldName = world.name;
                    worldFound = true;
                    inWorldView = true;
                    
                    // Tambahkan ke daftar recently visited
                    if (std::find(recentWorlds.begin(), recentWorlds.end(), world.name) == recentWorlds.end()) {
                        if (recentWorlds.size() >= 5) {
                            recentWorlds.pop_back();
                        }
                        recentWorlds.insert(recentWorlds.begin(), world.name);
                    }
                    
                    // Posisikan player di white door
                    playerX = WORLD_WIDTH / 2 * BLOCK_SIZE;
                    playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3;
                    playerVelocityX = 0.0f;
                    playerVelocityY = 0.0f;
                    isInsideWorld = true;
                    
                    break;
                }
            }
            
            // Jika world tidak ditemukan, tambahkan ke daftar
            if (!worldFound) {
                worldList.push_back({searchWorld, "You", 1, false});
                currentWorldName = searchWorld;
                inWorldView = true;
                
                // Tambahkan ke daftar my worlds
                myWorlds.push_back(searchWorld);
                
                // Tambahkan ke daftar recently visited
                if (recentWorlds.size() >= 5) {
                    recentWorlds.pop_back();
                }
                recentWorlds.insert(recentWorlds.begin(), searchWorld);
                
                // Posisikan player di white door
                playerX = WORLD_WIDTH / 2 * BLOCK_SIZE;
                playerY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3;
                playerVelocityX = 0.0f;
                playerVelocityY = 0.0f;
                isInsideWorld = true;
            }
        }
    }
    
    ImGui::End();
    
    return true;
}