#include "../../include/login/page.h"
#include "../../include/login/ui.h"
#include "../../include/login/state.h"
#include "../../include/network/client.h"
#include "../../include/core/config.h"
#include "imgui.h"
#include <iostream>
#include <string>
#include <cstring>

// Referensi ke network client
extern NetworkClient networkClient;

// Akses ke variabel loginStage di namespace Login
extern int Login::loginStage;

bool renderLoginPage(AppPage& currentPage) {
    // Jika dialog loading sedang ditampilkan, hanya render itu
    if (Login::renderLoadingDialog(currentPage)) {
        return true;
    }
    
    // Render form login
    Login::renderLoginForm(currentPage);
    
    // Variabel lokal untuk form login
    static char username[128] = "";
    static char password[128] = "";
    static bool isFirstTimeUser = true;
    
    // Dapatkan ukuran jendela aktual dari ImGui
    ImVec2 actualWindowSize = ImGui::GetIO().DisplaySize;
    
    // Tombol Login - posisikan di tengah panel dengan ukuran lebih besar
    float panelWidth = 600; // Sesuaikan dengan ukuran panel di renderLoginForm
    float panelHeight = 500; // Sesuaikan dengan ukuran panel yang lebih tinggi
    float buttonWidth = 400; // Tombol lebih lebar
    
    // Hanya tampilkan tombol login jika dalam tahap login dengan GrowID
    if (Login::loginStage == 1) {
        // Posisikan tombol login
        ImGui::SetCursorPos(ImVec2((actualWindowSize.x - buttonWidth) * 0.5f, (actualWindowSize.y - panelHeight) * 0.5f + 280));
        
        // Gunakan font lebih besar untuk tombol
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(1.5f); // Font 1.5x lebih besar
        
        // Tombol Login yang lebih besar dengan warna yang mencolok
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f)); // Warna hijau
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
        
        if (ImGui::Button("LOGIN", ImVec2(buttonWidth, 70))) { // Tombol lebih tinggi
            // Hubungkan ke server jika belum terhubung
            if (!networkClient.isClientConnected()) {
                Login::isConnecting = true;
                if (networkClient.connect("127.0.0.1", 5000)) {
                    Login::isConnecting = false;
                    std::cout << "Terhubung ke server" << std::endl;
                } else {
                    Login::isConnecting = false;
                    Login::serverResponse = "Gagal terhubung ke server";
                    Login::showServerResponse = true;
                    ImGui::OpenPopup("Server Response");
                    return true;
                }
            }
            
            if (isFirstTimeUser) {
                // Login sebagai pemain baru dengan username acak
                processLogin(username, password, true);
            } else {
                // Login dengan akun yang sudah ada
                if (strlen(username) > 0 && strlen(password) > 0) {
                    processLogin(username, password, false);
                } else {
                    ImGui::OpenPopup("Login Error");
                }
            }
        }
        ImGui::PopStyleColor(3);
        
        // Tombol Kembali yang lebih besar
        ImGui::SetCursorPos(ImVec2((actualWindowSize.x - buttonWidth) * 0.5f, (actualWindowSize.y - panelHeight) * 0.5f + 360));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Warna abu-abu
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        
        if (ImGui::Button("KEMBALI", ImVec2(buttonWidth, 50))) {
            currentPage = MAIN_MENU;
        }
        ImGui::PopStyleColor(3);
        ImGui::PopFont();
        ImGui::SetWindowFontScale(1.0f); // Reset font scale
    }
    
    // Render popup error
    Login::renderErrorPopups();
    
    return true;
}

bool processLogin(const char* username, const char* password, bool isGuest) {
    if (isGuest) {
        // Login sebagai pemain baru dengan username acak
        std::string guestLoginCommand = "GUEST_LOGIN|";
        if (networkClient.sendCommand(guestLoginCommand)) {
            // Terima respons dari server
            Login::serverResponse = networkClient.receiveResponse();
            
            // Jika login berhasil, mulai dialog loading
            if (Login::serverResponse.find("Login berhasil sebagai") != std::string::npos) {
                // Ekstrak username dari respons
                size_t pos = Login::serverResponse.find("sebagai ");
                if (pos != std::string::npos) {
                    std::string extractedUsername = Login::serverResponse.substr(pos + 7);
                    strcpy(const_cast<char*>(username), extractedUsername.c_str());
                    Login::currentUsername = extractedUsername;
                }
                
                // Mulai dialog loading
                Login::showLoadingDialog = true;
                Login::loadingStep = 0;
                Login::loadingStartTime = std::chrono::steady_clock::now();
                Login::onlineFriends = 0; // Untuk pemain baru, tidak ada teman online
                return true;
            } else {
                Login::showServerResponse = true;
                ImGui::OpenPopup("Server Response");
                return false;
            }
        }
    } else {
        // Login dengan akun yang sudah ada
        std::string loginCommand = "LOGIN|" + std::string(username) + "|" + std::string(password);
        if (networkClient.sendCommand(loginCommand)) {
            // Terima respons dari server
            Login::serverResponse = networkClient.receiveResponse();
            
            // Jika login berhasil, mulai dialog loading
            if (Login::serverResponse == "Login berhasil") {
                Login::currentUsername = username;
                
                // Mulai dialog loading
                Login::showLoadingDialog = true;
                Login::loadingStep = 0;
                Login::loadingStartTime = std::chrono::steady_clock::now();
                
                // Simulasi jumlah teman online (bisa diganti dengan data sebenarnya dari server)
                Login::onlineFriends = rand() % 6;
                return true;
            } else {
                Login::showServerResponse = true;
                ImGui::OpenPopup("Server Response");
                return false;
            }
        }
    }
    return false;
}