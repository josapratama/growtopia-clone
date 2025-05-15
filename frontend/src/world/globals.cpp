#include "../../include/world/globals.h"

// Variabel untuk fitur UI
bool showSettingsMenu = false;
bool showSocialPortal = false;
bool showShop = false;
bool hideUI = false;
char chatInput[128] = "";
bool showChatInput = false;
std::string currentChatMessage = "";
float chatMessageTimer = 0.0f;
float cameraX = 0.0f;

// Variabel global yang dideklarasikan di header
bool inWorldView = false;
float playerX = 50.0f;
float playerY = 50.0f;
std::string currentWorldName = "GROWTOPIA";

// Warna untuk elemen dunia
ImVec4 skyColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);
ImVec4 grassColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
ImVec4 blockColor = ImVec4(0.6f, 0.4f, 0.2f, 1.0f);

// Tambahkan di bagian bawah file

// Inisialisasi struktur data untuk health block
std::map<std::pair<int, int>, int> blockHealth;