#include "../../include/login/state.h"
#include <string>
#include <chrono>

namespace Login {
    // State untuk halaman login
    bool isConnecting = false;
    std::string serverResponse = "";
    bool showServerResponse = false;
    
    // State untuk dialog loading
    bool showLoadingDialog = false;
    int loadingStep = 0;
    std::chrono::steady_clock::time_point loadingStartTime;
    std::string currentUsername = "";
    int onlineFriends = 0;
    
    // Tambahkan implementasi loginStage
    int loginStage = 0; // 0: Pilih akun, 1: Login dengan GrowID
    
    // Inisialisasi state
    void initLoginState() {
        isConnecting = false;
        serverResponse = "";
        showServerResponse = false;
        showLoadingDialog = false;
        loadingStep = 0;
        currentUsername = "";
        onlineFriends = 0;
        loginStage = 0; // Reset ke tahap pemilihan akun
    }
}