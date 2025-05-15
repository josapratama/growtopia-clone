#pragma once
#include <string>
#include <chrono>
#include "../core/pages.h"

namespace Login {
    // Variabel untuk dialog loading
    extern bool showLoadingDialog;
    extern int loadingStep;
    extern std::chrono::steady_clock::time_point loadingStartTime;
    extern int onlineFriends;
    
    // Variabel untuk respons server
    extern std::string serverResponse;
    extern bool showServerResponse;
    extern bool isConnecting;
    
    // Username saat ini
    extern std::string currentUsername;
    
    // Tahap login
    extern int loginStage; // 0: Pilih akun, 1: Login dengan GrowID
    
    // Fungsi untuk render dialog
    bool renderLoadingDialog(AppPage& currentPage);
    bool renderLoginForm(AppPage& currentPage);
    void renderErrorPopups();
}

// Inisialisasi state
void initLoginState();

// Reset state setelah login berhasil
void resetLoginState();