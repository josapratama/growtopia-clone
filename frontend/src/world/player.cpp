#include "../../include/world/player.h"
#include "../../include/world/constants.h"
#include "../../include/world/globals.h"
#include "../../include/core/config.h"
#include "../../include/world/terrain.h"
#include <iostream>

// Tambahkan definisi ini sebelum include cmath
#define _USE_MATH_DEFINES
#include <cmath> // Untuk fungsi sin dan konstanta M_PI

#include "imgui.h"
#include "imgui_internal.h"

// Tambahkan variabel untuk kecepatan dan nama player
float playerVelocityX = 0.0f;
float playerVelocityY = 0.0f;
std::string playerName = "Guest";
bool isTyping = false;

// Variabel untuk item aktif
int activeItemId = 1; // Default: Punch (ID 1)

// Variabel untuk animasi player
float animationTime = 0.0f;
bool isFacingRight = true;
bool isJumping = false;
float jumpHeight = 0.0f;
float jumpStartY = 0.0f;
const float MAX_JUMP_HEIGHT = BLOCK_SIZE * 4.0f; // Tinggi lompatan 4 tile
const float JUMP_SPEED = 5.0f; // Kecepatan lompatan dikurangi dari 10.0f menjadi 5.0f untuk gerakan lebih lambat

// Tambahkan variabel untuk melacak status tombol lompat
bool jumpKeyReleased = true; // Pastikan tombol lompat harus dilepas sebelum bisa melompat lagi

// Proses input keyboard untuk pergerakan pemain
void processPlayerMovement() {
    // Kurangi kecepatan untuk pergerakan yang lebih lambat (seperti berjalan antar tile)
    const float acceleration = 0.7f * PLAYER_MOVE_SPEED; // Ditingkatkan dari 0.5f untuk akselerasi lebih cepat
    const float deceleration = 0.8f * PLAYER_MOVE_SPEED; // Dikurangi dari 0.8f untuk pengereman lebih cepat
    const float maxSpeed = PLAYER_MOVE_SPEED * 0.6f;     // Ditingkatkan dari 0.4f untuk kecepatan maksimum lebih tinggi
    float deltaTime = ImGui::GetIO().DeltaTime;
    
    // Update waktu animasi
    animationTime += deltaTime * 5.0f; // Kecepatan animasi
    if (animationTime > 2.0f * M_PI) {
        animationTime -= 2.0f * M_PI; // Reset animasi setelah satu siklus penuh
    }
    
    // Horizontal movement dengan interpolasi yang lebih smooth
    if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
        // Akselerasi lebih lambat untuk gerakan yang lebih smooth
        playerVelocityX = ImLerp(playerVelocityX, -maxSpeed, acceleration * deltaTime);
        isFacingRight = false; // Menghadap ke kiri
    } else if (ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
        // Akselerasi lebih lambat untuk gerakan yang lebih smooth
        playerVelocityX = ImLerp(playerVelocityX, maxSpeed, acceleration * deltaTime);
        isFacingRight = true; // Menghadap ke kanan
    } else {
        // Deceleration yang lebih smooth ketika tidak ada tombol yang ditekan
        playerVelocityX = ImLerp(playerVelocityX, 0.0f, deceleration * deltaTime);
    }
    
    // Cek apakah tombol lompat (W/UpArrow) sedang ditekan
    bool jumpKeyDown = ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_UpArrow);
    
    // Proses lompatan - hanya jika tombol sudah dilepas sebelumnya
    if (!isJumping && jumpKeyDown && jumpKeyReleased) {
        // Mulai lompatan baru
        isJumping = true;
        jumpStartY = playerY;
        jumpHeight = 0.0f;
        jumpKeyReleased = false; // Tandai bahwa tombol lompat sedang ditekan
    }
    
    // Update status tombol lompat - cek apakah tombol sudah dilepas
    if (!jumpKeyDown) {
        jumpKeyReleased = true; // Tombol sudah dilepas, siap untuk lompatan berikutnya
    }
    
    // Proses lompatan yang sedang berlangsung
    if (isJumping) {
        // Tingkatkan ketinggian lompatan dengan kecepatan yang lebih lambat
        jumpHeight += JUMP_SPEED * deltaTime * 60.0f;
        
        if (jumpHeight <= MAX_JUMP_HEIGHT) {
            // Fase naik dengan interpolasi untuk gerakan lebih smooth
            playerY = ImLerp(playerY, jumpStartY - jumpHeight, 0.2f); // Tambahkan interpolasi
        } else if (jumpHeight <= 2 * MAX_JUMP_HEIGHT) {
            // Fase turun dengan interpolasi untuk gerakan lebih smooth
            playerY = ImLerp(playerY, jumpStartY - (2 * MAX_JUMP_HEIGHT - jumpHeight), 0.2f); // Tambahkan interpolasi
        } else {
            // Lompatan selesai
            playerY = jumpStartY;
            isJumping = false;
        }
    } else if (ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
        // Vertical movement (hanya turun) dengan interpolasi yang lebih smooth
        playerVelocityY = ImLerp(playerVelocityY, maxSpeed, acceleration * deltaTime);
        playerY += playerVelocityY * deltaTime * 60.0f; // Normalisasi untuk 60 FPS
    } else {
        // Reset vertical velocity jika tidak sedang melompat atau turun
        playerVelocityY = 0.0f;
    }
    
    // Update posisi horizontal dengan delta time untuk gerakan yang konsisten
    playerX += playerVelocityX * deltaTime * 60.0f; // Normalisasi untuk 60 FPS
    
    // Batasi posisi karakter dalam world
    playerX = ImClamp(playerX, BLOCK_SIZE, WORLD_WIDTH * BLOCK_SIZE - BLOCK_SIZE);
    playerY = ImClamp(playerY, BLOCK_SIZE, SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE);
}

// Fungsi untuk menggambar player dengan animasi
void renderPlayer(ImDrawList* drawList, float playerScreenX) {
    // Warna dasar player
    ImU32 bodyColor = IM_COL32(255, 204, 153, 255); // Warna kulit
    ImU32 outlineColor = IM_COL32(0, 0, 0, 255); // Outline hitam
    ImU32 clothColor = IM_COL32(50, 100, 200, 255); // Warna pakaian biru
    
    // Ukuran player
    float playerWidth = BLOCK_SIZE * 0.8f;
    float playerHeight = BLOCK_SIZE * 1.5f;
    
    // Posisi player di layar
    float screenX = playerScreenX;
    float screenY = playerY;
    
    // Gambar tubuh player (persegi panjang)
    drawList->AddRectFilled(
        ImVec2(screenX - playerWidth/2, screenY - playerHeight/2),
        ImVec2(screenX + playerWidth/2, screenY + playerHeight/2),
        bodyColor,
        5.0f // Rounded corners
    );
    
    // Gambar outline tubuh
    drawList->AddRect(
        ImVec2(screenX - playerWidth/2, screenY - playerHeight/2),
        ImVec2(screenX + playerWidth/2, screenY + playerHeight/2),
        outlineColor,
        5.0f, // Rounded corners
        0, // Flags
        2.0f // Thickness
    );
    
    // Gambar pakaian (bagian bawah tubuh)
    drawList->AddRectFilled(
        ImVec2(screenX - playerWidth/2, screenY),
        ImVec2(screenX + playerWidth/2, screenY + playerHeight/2),
        clothColor,
        3.0f // Rounded corners
    );
    
    // Gambar wajah (mata, hidung, mulut)
    float faceDirection = isFacingRight ? 1.0f : -1.0f;
    
    // Mata kiri
    drawList->AddCircleFilled(
        ImVec2(screenX - playerWidth/5 * faceDirection, screenY - playerHeight/5),
        playerWidth/10,
        IM_COL32(255, 255, 255, 255) // Putih
    );
    drawList->AddCircleFilled(
        ImVec2(screenX - playerWidth/5 * faceDirection, screenY - playerHeight/5),
        playerWidth/20,
        IM_COL32(0, 0, 0, 255) // Hitam (pupil)
    );
    
    // Mata kanan
    drawList->AddCircleFilled(
        ImVec2(screenX - playerWidth/10 * faceDirection, screenY - playerHeight/5),
        playerWidth/10,
        IM_COL32(255, 255, 255, 255) // Putih
    );
    drawList->AddCircleFilled(
        ImVec2(screenX - playerWidth/10 * faceDirection, screenY - playerHeight/5),
        playerWidth/20,
        IM_COL32(0, 0, 0, 255) // Hitam (pupil)
    );
    
    // Hidung
    drawList->AddCircleFilled(
        ImVec2(screenX - playerWidth/7 * faceDirection, screenY - playerHeight/10),
        playerWidth/15,
        IM_COL32(255, 150, 150, 255) // Pink
    );
    
    // Mulut
    drawList->AddLine(
        ImVec2(screenX - playerWidth/4 * faceDirection, screenY),
        ImVec2(screenX - playerWidth/10 * faceDirection, screenY + playerHeight/15),
        IM_COL32(0, 0, 0, 255), // Hitam
        2.0f // Thickness
    );
    
    // Animasi tangan dan kaki
    float legSwing = sin(animationTime) * playerWidth/3;
    float armSwing = sin(animationTime + M_PI/4) * playerWidth/3;
    
    // Hanya animasikan jika player bergerak
    if (fabs(playerVelocityX) > 0.1f) {
        // Kaki kiri
        drawList->AddLine(
            ImVec2(screenX - playerWidth/4, screenY + playerHeight/2),
            ImVec2(screenX - playerWidth/4 + legSwing, screenY + playerHeight/2 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
        
        // Kaki kanan
        drawList->AddLine(
            ImVec2(screenX + playerWidth/4, screenY + playerHeight/2),
            ImVec2(screenX + playerWidth/4 - legSwing, screenY + playerHeight/2 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
        
        // Tangan kiri
        drawList->AddLine(
            ImVec2(screenX - playerWidth/2, screenY - playerHeight/4),
            ImVec2(screenX - playerWidth/2 - armSwing, screenY - playerHeight/4 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
        
        // Tangan kanan
        drawList->AddLine(
            ImVec2(screenX + playerWidth/2, screenY - playerHeight/4),
            ImVec2(screenX + playerWidth/2 + armSwing, screenY - playerHeight/4 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
    } else {
        // Kaki dalam posisi normal
        drawList->AddLine(
            ImVec2(screenX - playerWidth/4, screenY + playerHeight/2),
            ImVec2(screenX - playerWidth/4, screenY + playerHeight/2 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
        
        drawList->AddLine(
            ImVec2(screenX + playerWidth/4, screenY + playerHeight/2),
            ImVec2(screenX + playerWidth/4, screenY + playerHeight/2 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
        
        // Tangan dalam posisi normal
        drawList->AddLine(
            ImVec2(screenX - playerWidth/2, screenY - playerHeight/4),
            ImVec2(screenX - playerWidth/2 - playerWidth/4, screenY - playerHeight/4 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
        
        drawList->AddLine(
            ImVec2(screenX + playerWidth/2, screenY - playerHeight/4),
            ImVec2(screenX + playerWidth/2 + playerWidth/4, screenY - playerHeight/4 + playerHeight/3),
            outlineColor,
            4.0f // Thickness
        );
    }
}

// Update posisi kamera berdasarkan posisi pemain dengan smoothing
void updateCameraPosition() {
    float targetCameraX = cameraX;
    
    if (playerX - cameraX > SCR_WIDTH * CAMERA_RIGHT_THRESHOLD) {
        targetCameraX = playerX - SCR_WIDTH * CAMERA_RIGHT_THRESHOLD;
    } else if (playerX - cameraX < SCR_WIDTH * CAMERA_LEFT_THRESHOLD) {
        targetCameraX = playerX - SCR_WIDTH * CAMERA_LEFT_THRESHOLD;
    }
    
    // Smooth camera movement dengan interpolasi
    cameraX = ImLerp(cameraX, targetCameraX, ImGui::GetIO().DeltaTime * 5.0f);
    
    // Batasi kamera agar tidak keluar dari world
    cameraX = ImClamp(cameraX, 0.0f, WORLD_WIDTH * BLOCK_SIZE - SCR_WIDTH);
}

// Fungsi untuk menginisialisasi posisi player saat masuk world
void initPlayerPosition() {
    // Posisi vertikal di tengah
    playerY = (SCR_HEIGHT * TERRAIN_HEIGHT_RATIO) / 2;
    
    // Posisi horizontal acak
    float randomFactor = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    playerX = BLOCK_SIZE + randomFactor * (WORLD_WIDTH * BLOCK_SIZE - 2 * BLOCK_SIZE);
    
    // Reset velocity
    playerVelocityX = 0.0f;
    playerVelocityY = 0.0f;
    
    // Reset animasi
    animationTime = 0.0f;
    isJumping = false;
    jumpKeyReleased = true; // Reset status tombol lompat
    
    // Set item aktif ke punch secara default
    activeItemId = 1; // Punch
}

// Render nama player dan status mengetik di atas kepala
void renderPlayerName(ImDrawList* drawList, float playerScreenX) {
    ImFont* font = ImGui::GetFont();
    std::string displayText = playerName;
    
    // Tambahkan indikator mengetik jika sedang chat
    if (isTyping || showChatInput) {
        displayText += " (typing...)";
    }
    
    float textWidth = font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, displayText.c_str()).x;
    float textX = playerScreenX - textWidth / 2;
    float textY = playerY - BLOCK_SIZE - 40.0f; // Posisi di atas chat bubble
    
    // Latar belakang nama player
    drawList->AddRectFilled(
        ImVec2(textX - 5, textY - 5),
        ImVec2(textX + textWidth + 5, textY + ImGui::GetFontSize() + 5),
        IM_COL32(0, 0, 0, 150),
        5.0f
    );
    
    // Teks nama player
    drawList->AddText(
        ImVec2(textX, textY),
        IM_COL32(255, 255, 255, 255),
        displayText.c_str()
    );
}

// Tambahkan variabel global untuk status world dan posisi door
bool isInsideWorld = false; // Status player (true = di dalam world, false = di luar world)
float doorX = WORLD_WIDTH / 2 * BLOCK_SIZE; // Posisi X pintu (tengah world)

// Fungsi untuk mendeteksi dan memproses interaksi dengan door
void processDoorInteraction() {
    // Hitung jarak player ke door
    float doorWorldX = doorX;
    float doorWorldY = SCR_HEIGHT * TERRAIN_HEIGHT_RATIO - BLOCK_SIZE * 3; // Posisi Y pintu (di atas permukaan tanah)
    
    // Hitung jarak Euclidean antara player dan door
    float distanceX = std::abs(playerX - doorWorldX);
    float distanceY = std::abs(playerY - doorWorldY);
    
    // Jika player cukup dekat dengan door (dalam radius 2 blok)
    bool isNearDoor = (distanceX < BLOCK_SIZE * 2 && distanceY < BLOCK_SIZE * 2);
    
    // Tampilkan petunjuk jika player dekat dengan door
    if (isNearDoor) {
        // Dapatkan ImDrawList untuk menggambar
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // Tampilkan petunjuk di atas kepala player
        std::string hintText = "Click door to " + std::string(isInsideWorld ? "exit" : "enter") + " world";
        float textWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, hintText.c_str()).x;
        float textX = playerX - cameraX - textWidth / 2;
        float textY = playerY - BLOCK_SIZE * 2;
        
        // Latar belakang petunjuk
        drawList->AddRectFilled(
            ImVec2(textX - 5, textY - 5),
            ImVec2(textX + textWidth + 5, textY + ImGui::GetFontSize() + 5),
            IM_COL32(0, 0, 0, 200),
            5.0f
        );
        
        // Teks petunjuk
        drawList->AddText(
            ImVec2(textX, textY),
            IM_COL32(255, 255, 255, 255),
            hintText.c_str()
        );
        
        // Hapus deteksi tombol E untuk masuk/keluar world
        bool shouldToggleWorld = false;
        
        // Deteksi klik mouse pada door
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            // Dapatkan posisi mouse
            ImVec2 mousePos = ImGui::GetMousePos();
            
            // Konversi ke koordinat world
            float mouseWorldX = mousePos.x + cameraX;
            float mouseWorldY = mousePos.y;
            
            // Cek apakah klik pada area door (door berukuran 1x3 blok)
            bool clickedOnDoor = (
                mouseWorldX >= doorWorldX - BLOCK_SIZE/2 && 
                mouseWorldX <= doorWorldX + BLOCK_SIZE/2 && 
                mouseWorldY >= doorWorldY - BLOCK_SIZE && 
                mouseWorldY <= doorWorldY + BLOCK_SIZE*2
            );
            
            if (clickedOnDoor) {
                shouldToggleWorld = true;
            }
        }
        
        // Jika harus toggle world (hanya dari klik mouse)
        if (shouldToggleWorld) {
            // Toggle status dalam/luar world
            isInsideWorld = !isInsideWorld;
            
            if (!isInsideWorld) {
                // Keluar dari world - kembali ke world list
                inWorldView = false;
                
                // Tampilkan pesan
                std::cout << "Player exited the world through the white door!" << std::endl;
            } else {
                // Masuk ke dalam world - pindahkan player tepat di posisi pintu
                playerX = doorWorldX;
                playerY = doorWorldY; // Tepat di posisi pintu
                
                // Tampilkan pesan
                std::cout << "Player entered the world through the white door!" << std::endl;
            }
            
            // Reset velocity saat masuk/keluar
            playerVelocityX = 0.0f;
            playerVelocityY = 0.0f;
        }
    }
}

// Fungsi untuk mendeteksi dan memproses klik mouse untuk memukul block
void processBlockPunch() {
    // Hanya proses jika item aktif adalah punch (ID 1)
    if (activeItemId != 1) {
        return;
    }
    
    // Deteksi klik mouse kiri
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        // Debug output - pindahkan ke sini agar hanya muncul saat ada klik
        std::cout << "processBlockPunch called, activeItemId: " << activeItemId << std::endl;
        
        // Dapatkan posisi mouse
        ImVec2 mousePos = ImGui::GetMousePos();
        
        // Konversi ke koordinat world
        float worldX = mousePos.x + cameraX;
        float worldY = mousePos.y;
        
        // Coba pukul block pada posisi tersebut
        bool blockDestroyed = punchBlock(worldX, worldY);
        
        // Jika block hancur, bisa tambahkan efek suara atau animasi di sini
        if (blockDestroyed) {
            // TODO: Tambahkan efek suara atau animasi
        }
    }
}