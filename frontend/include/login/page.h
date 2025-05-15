#pragma once
#include "../core/pages.h"

// Fungsi utama untuk render halaman login
bool renderLoginPage(AppPage& currentPage);

// Fungsi untuk memproses login
bool processLogin(const char* username, const char* password, bool isGuest);