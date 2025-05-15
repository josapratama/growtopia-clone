#include "../../include/login/ui.h"
#include "../../include/login/state.h"
#include "../../include/core/config.h"
#include "imgui.h"
#include "imgui_internal.h" // Tambahkan ini untuk menggunakan ImClamp
#include <string>
#include <chrono>
#include <algorithm> // Pindahkan ke sini

namespace Login {
    // Variabel lokal untuk form login
    static char username[128] = "Guest123"; // Username acak default
    static char password[128] = "";
    static bool hasAccount = false; // Checkbox untuk "Saya memiliki akun"
    
    // Tambahkan variabel untuk melacak tahap login
    static int loginStage = 0; // 0: Pilih akun, 1: Login dengan GrowID
    
    // Tambahkan variabel untuk posisi dan status drag dialog loading
    static ImVec2 loadingDialogPos(0, 0);
    static bool isDraggingDialog = false;
    static bool isDialogPositionInitialized = false;
    
    bool renderLoadingDialog(AppPage& currentPage) {
        if (!showLoadingDialog) return false;
        
        // Dapatkan ukuran jendela aktual dari ImGui
        ImVec2 actualWindowSize = ImGui::GetIO().DisplaySize;
        
        // Ukuran panel loading
        float panelWidth = 600;
        float panelHeight = 300;
        
        // Inisialisasi posisi dialog di tengah layar jika belum diinisialisasi
        if (!isDialogPositionInitialized) {
            loadingDialogPos = ImVec2((actualWindowSize.x - panelWidth) * 0.5f, (actualWindowSize.y - panelHeight) * 0.5f);
            isDialogPositionInitialized = true;
        }
        
        // Mengatur posisi jendela berdasarkan posisi yang disimpan
        ImGui::SetNextWindowPos(loadingDialogPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(panelWidth, panelHeight), ImGuiCond_Always);
        
        // Hapus flag NoMove agar dialog bisa digeser
        ImGui::Begin("##LoadingScreen", NULL, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoCollapse
        );
        
        // Dapatkan ukuran jendela saat ini
        ImVec2 windowSize = ImGui::GetWindowSize();
        
        // Periksa apakah user sedang men-drag dialog
        if (ImGui::IsMouseHoveringRect(loadingDialogPos, 
                                      ImVec2(loadingDialogPos.x + panelWidth, loadingDialogPos.y + 30)) && 
            ImGui::IsMouseDown(0)) {
            isDraggingDialog = true;
        }
        
        // Jika sedang di-drag, update posisi dialog
        if (isDraggingDialog && ImGui::IsMouseDragging(0)) {
            ImVec2 dragDelta = ImGui::GetIO().MouseDelta;
            loadingDialogPos.x += dragDelta.x;
            loadingDialogPos.y += dragDelta.y;
            
            // Hapus #include <algorithm> dari sini
            
            // Batasi posisi agar tidak keluar layar
            // Gunakan salah satu dari opsi berikut:
            
            // Opsi 1: Gunakan ImClamp dari imgui_internal.h
            loadingDialogPos.x = ImClamp(loadingDialogPos.x, 0.0f, actualWindowSize.x - panelWidth);
            loadingDialogPos.y = ImClamp(loadingDialogPos.y, 0.0f, actualWindowSize.y - panelHeight);
            
            // ATAU Opsi 2: Gunakan std::clamp jika menggunakan C++17 atau lebih baru
            // loadingDialogPos.x = std::clamp(loadingDialogPos.x, 0.0f, actualWindowSize.x - panelWidth);
            // loadingDialogPos.y = std::clamp(loadingDialogPos.y, 0.0f, actualWindowSize.y - panelHeight);
            
            // ATAU Opsi 3: Implementasi manual dengan std::min dan std::max
            // loadingDialogPos.x = std::max(0.0f, std::min(loadingDialogPos.x, actualWindowSize.x - panelWidth));
            // loadingDialogPos.y = std::max(0.0f, std::min(loadingDialogPos.y, actualWindowSize.y - panelHeight));
        }
        
        // Reset status drag jika mouse dilepas
        if (!ImGui::IsMouseDown(0)) {
            isDraggingDialog = false;
        }
        
        // Buat panel dengan background
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f)); // Background lebih gelap
        ImGui::BeginChild("LoadingPanel", ImVec2(panelWidth, panelHeight), false); // Tanpa border
        
        // Gunakan font lebih besar untuk judul
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(2.0f); // Font 2x lebih besar
        
        // Judul di tengah panel
        float titleWidth = ImGui::CalcTextSize("Connecting to Growtopia").x;
        ImGui::SetCursorPosX((panelWidth - titleWidth) * 0.5f);
        ImGui::Text("Connecting to Growtopia");
        ImGui::PopFont();
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Gunakan font lebih besar untuk konten
        ImGui::SetWindowFontScale(1.5f); // Font 1.5x lebih besar
        
        // Kode loading yang sudah ada
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - loadingStartTime).count();
        
        // Tampilkan pesan berdasarkan langkah loading dengan font lebih besar
        if (loadingStep == 0) {
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Getting server address...");
            if (elapsedTime > 1000) { // Setelah 1 detik
                loadingStep = 1;
            }
        } 
        else if (loadingStep == 1) {
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Getting server address... Done!");
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Located server, connecting...");
            if (elapsedTime > 2000) { // Setelah 2 detik
                loadingStep = 2;
            }
        } 
        else if (loadingStep == 2) {
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Getting server address... Done!");
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Located server, connecting... Done!");
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Logging on, %s...", currentUsername.c_str());
            if (elapsedTime > 3000) { // Setelah 3 detik
                loadingStep = 3;
            }
        } 
        else if (loadingStep == 3) {
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Getting server address... Done!");
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Located server, connecting... Done!");
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Logging on, %s... Done!", currentUsername.c_str());
            
            // Tampilkan pesan selamat datang dengan jumlah teman online
            std::string friendMessage;
            if (onlineFriends == 0) {
                friendMessage = "No friends are online.";
            } else if (onlineFriends == 1) {
                friendMessage = "1 friend is online.";
            } else {
                friendMessage = std::to_string(onlineFriends) + " friends are online.";
            }
            
            ImGui::SetCursorPosX(panelWidth * 0.1f);
            ImGui::Text("Welcome back, %s. %s", currentUsername.c_str(), friendMessage.c_str());
            
            if (elapsedTime > 4500) { // Setelah 4.5 detik
                // Jangan sembunyikan dialog, hanya pindah ke halaman world list
                // showLoadingDialog = false;
                loadingStep = 0;
                // Pindah ke halaman daftar world
                currentPage = WORLD_LIST;
            }
        }
        
        // Tambahkan tombol untuk menutup dialog
        ImGui::SetCursorPos(ImVec2(panelWidth - 200, panelHeight - 40));
        if (ImGui::Button("Close", ImVec2(80, 30))) {
            showLoadingDialog = false;
            loadingStep = 0;
        }
        
        // Tambahkan tombol Cancel untuk kembali ke menu utama
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(panelWidth - 100, panelHeight - 40));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f)); // Warna merah
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Cancel", ImVec2(80, 30))) {
            showLoadingDialog = false;
            loadingStep = 0;
            isConnecting = false;
            currentPage = MAIN_MENU; // Kembali ke menu utama
        }
        ImGui::PopStyleColor(3);
        
        // Reset font scale
        ImGui::SetWindowFontScale(1.0f);
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::End();
        return true;
    }
    
    bool renderLoginForm(AppPage& currentPage) {
        // Dapatkan ukuran jendela aktual dari ImGui
        ImVec2 actualWindowSize = ImGui::GetIO().DisplaySize;
        
        // Mengatur posisi dan ukuran jendela untuk mengisi seluruh layar
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(actualWindowSize, ImGuiCond_Always);
        
        // Hapus border dan judul window
        ImGui::Begin("##LoginScreen", NULL, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground
        );
        
        // Dapatkan ukuran jendela saat ini
        ImVec2 windowSize = ImGui::GetWindowSize();
        
        // Buat panel login di tengah layar - lebih besar
        float panelWidth = 600; // Panel lebih lebar
        float panelHeight = 500; // Panel lebih tinggi untuk menampung tombol tambahan
        
        // Posisikan di tengah layar berdasarkan ukuran jendela saat ini
        ImGui::SetCursorPos(ImVec2((windowSize.x - panelWidth) * 0.5f, (windowSize.y - panelHeight) * 0.5f));
        
        // Buat panel dengan background
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f)); // Background lebih gelap
        ImGui::BeginChild("LoginPanel", ImVec2(panelWidth, panelHeight), false); // Tanpa border
        
        // Gunakan font lebih besar untuk judul
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(2.0f); // Font 2x lebih besar
        
        // Judul di tengah panel
        float titleWidth = ImGui::CalcTextSize("Selamat datang di Growtopia!").x;
        ImGui::SetCursorPosX((panelWidth - titleWidth) * 0.5f);
        ImGui::Text("Selamat datang di Growtopia!");
        ImGui::PopFont();
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Gunakan font lebih besar untuk konten
        ImGui::SetWindowFontScale(1.5f); // Font 1.5x lebih besar
        
        if (loginStage == 0) {
            // Tampilkan dialog pemilihan akun
            float subtitleWidth = ImGui::CalcTextSize("Select an account to continue").x;
            ImGui::SetCursorPosX((panelWidth - subtitleWidth) * 0.5f);
            ImGui::Text("Select an account to continue");
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Ukuran tombol
            float buttonWidth = 400;
            float buttonHeight = 60;
            float buttonX = (panelWidth - buttonWidth) * 0.5f;
            
            // Tombol Continue with Apple
            ImGui::SetCursorPosX(buttonX);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Warna hitam
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::Button("Continue with Apple", ImVec2(buttonWidth, buttonHeight))) {
                // Implementasi login dengan Apple
            }
            ImGui::PopStyleColor(3);
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Tombol Continue with Google
            ImGui::SetCursorPosX(buttonX);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f)); // Warna biru
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.7f, 1.0f));
            if (ImGui::Button("Continue with Google", ImVec2(buttonWidth, buttonHeight))) {
                // Implementasi login dengan Google
            }
            ImGui::PopStyleColor(3);
            
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            // Label untuk legacy users
            float legacyWidth = ImGui::CalcTextSize("--- Legacy Users Only ---").x;
            ImGui::SetCursorPosX((panelWidth - legacyWidth) * 0.5f);
            ImGui::Text("--- Legacy Users Only ---");
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Tombol Growtopia Login
            ImGui::SetCursorPosX(buttonX);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f)); // Warna hijau
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
            if (ImGui::Button("Growtopia Login", ImVec2(buttonWidth, buttonHeight))) {
                loginStage = 1; // Pindah ke tahap login dengan GrowID
            }
            ImGui::PopStyleColor(3);
        }
        else if (loginStage == 1) {
            // Tampilkan dialog login dengan GrowID
            float subtitleWidth = ImGui::CalcTextSize("Log in with your GrowID").x;
            ImGui::SetCursorPosX((panelWidth - subtitleWidth) * 0.5f);
            ImGui::Text("Log in with your GrowID");
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Checkbox untuk pemain yang sudah memiliki akun
            ImGui::SetCursorPosX((panelWidth - panelWidth * 0.8f) * 0.5f);
            ImGui::Checkbox("Saya memiliki akun", &hasAccount);
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Input fields untuk username dan password
            ImGui::PushItemWidth(panelWidth * 0.8f); // Input field lebih lebar
            ImGui::SetCursorPosX((panelWidth - panelWidth * 0.8f) * 0.5f); // Posisikan di tengah
            
            // Username field - readonly jika tidak memiliki akun
            if (!hasAccount) {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Background lebih gelap untuk readonly
                ImGui::InputText("GrowID", username, IM_ARRAYSIZE(username), ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
                
                // Tampilkan pesan untuk pemain baru
                ImGui::SetCursorPosX((panelWidth - panelWidth * 0.8f) * 0.5f);
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + panelWidth * 0.8f);
                ImGui::TextWrapped("Anda akan bermain dengan username acak di atas.");
                ImGui::PopTextWrapPos();
            } else {
                // Username dan password dapat diedit
                ImGui::InputText("GrowID", username, IM_ARRAYSIZE(username));
                
                ImGui::Spacing();
                
                ImGui::SetCursorPosX((panelWidth - panelWidth * 0.8f) * 0.5f); // Posisikan di tengah
                ImGui::InputText("Password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);
            }
            ImGui::PopItemWidth();
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Tombol Login
            float buttonWidth = 400;
            float buttonHeight = 50;
            float buttonX = (panelWidth - buttonWidth) * 0.5f;
            
            ImGui::SetCursorPosX(buttonX);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f)); // Warna hijau
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
            if (ImGui::Button("Log In", ImVec2(buttonWidth, buttonHeight))) {
                // Implementasi login
                if (hasAccount && (strlen(username) == 0 || strlen(password) == 0)) {
                    ImGui::OpenPopup("Login Error");
                } else {
                    // Proses login
                    isConnecting = true;
                    currentUsername = username;
                    // Simulasi loading dialog
                    showLoadingDialog = true;
                    loadingStartTime = std::chrono::steady_clock::now();
                }
            }
            ImGui::PopStyleColor(3);
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Tombol Kembali ke pemilihan akun
            ImGui::SetCursorPosX(buttonX);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Warna abu-abu
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            if (ImGui::Button("Kembali ke Pemilihan Akun", ImVec2(buttonWidth, buttonHeight))) {
                loginStage = 0; // Kembali ke tahap pemilihan akun
            }
            ImGui::PopStyleColor(3);
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Indikator koneksi
        if (isConnecting) {
            ImGui::SetCursorPosX((panelWidth - ImGui::CalcTextSize("Menghubungkan ke server...").x) * 0.5f);
            ImGui::Text("Menghubungkan ke server...");
        }
        
        // Reset font scale
        ImGui::SetWindowFontScale(1.0f);
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::End();
        
        return true;
    }
    
    void renderErrorPopups() {
        // Popup untuk error login
        if (ImGui::BeginPopupModal("Login Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (hasAccount) {
                if (strlen(username) == 0) {
                    ImGui::Text("Username tidak boleh kosong!");
                } else if (strlen(password) == 0) {
                    ImGui::Text("Password tidak boleh kosong!");
                }
            }
            
            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        
        // Popup untuk respons server
        if (showServerResponse && ImGui::BeginPopupModal("Server Response", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", serverResponse.c_str());
            
            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                showServerResponse = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}