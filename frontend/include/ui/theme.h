#pragma once
#include "imgui.h"

// Warna tema UI
extern ImVec4 buttonColor;
extern ImVec4 buttonHoverColor;
extern ImVec4 buttonActiveColor;
extern ImVec4 windowBgColor;
extern ImVec4 headerColor;
extern ImVec4 textColor;
extern ImVec4 borderColor;

// Fungsi untuk menginisialisasi tema UI
void initUITheme();

// Tambahkan deklarasi fungsi yang digunakan di main.cpp
void loadCustomFonts();
void setGrowtopiaTheme();