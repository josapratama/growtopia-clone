#pragma once

// Enum untuk halaman aplikasi
enum AppPage {
    MAIN_MENU,
    LOGIN_PAGE,
    REGISTER_PAGE,
    OPTION_PAGE,
    ABOUT_PAGE,
    WORLD_LIST,
    WORLD_VIEW
};

// Fungsi untuk menampilkan halaman option dan about
void renderOptionPage(AppPage& currentPage);
void renderAboutPage(AppPage& currentPage);