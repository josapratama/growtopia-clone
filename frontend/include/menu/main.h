#pragma once
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "../core/pages.h"

// Fungsi untuk menampilkan menu utama
bool renderMainMenu(AppPage& currentPage, bool& shouldClose);