#pragma once
#include "../core/pages.h"

namespace Login {
    // Fungsi untuk menampilkan dialog loading
    bool renderLoadingDialog(AppPage& currentPage);
    
    // Fungsi untuk menampilkan form login
    bool renderLoginForm(AppPage& currentPage);
    
    // Fungsi untuk menampilkan popup error
    void renderErrorPopups();
}