#include "../../include/ui/theme.h"
#include "imgui.h"

// Warna tema UI
ImVec4 buttonColor = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);
ImVec4 buttonHoverColor = ImVec4(0.3f, 0.5f, 0.9f, 1.0f);
ImVec4 buttonActiveColor = ImVec4(0.1f, 0.3f, 0.7f, 1.0f);
ImVec4 windowBgColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
ImVec4 headerColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 borderColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

// Fungsi untuk menginisialisasi tema UI
void initUITheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Warna
    style.Colors[ImGuiCol_Button] = buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = buttonHoverColor;
    style.Colors[ImGuiCol_ButtonActive] = buttonActiveColor;
    style.Colors[ImGuiCol_WindowBg] = windowBgColor;
    style.Colors[ImGuiCol_Header] = headerColor;
    style.Colors[ImGuiCol_Text] = textColor;
    style.Colors[ImGuiCol_Border] = borderColor;
    
    // Rounding
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    
    // Padding
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(10, 8);
    style.ItemInnerSpacing = ImVec2(6, 6);
    
    // Border
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
}

// Fungsi untuk mengatur tema aplikasi
void setGrowtopiaTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Warna utama dengan tema hijau-biru Growtopia
    const ImVec4 bgColor         = ImVec4(0.08f, 0.12f, 0.14f, 1.00f);
    const ImVec4 lightBgColor    = ImVec4(0.12f, 0.17f, 0.20f, 1.00f);
    const ImVec4 accentColor     = ImVec4(0.18f, 0.72f, 0.36f, 1.00f); // Hijau Growtopia
    const ImVec4 accentColor2    = ImVec4(0.15f, 0.55f, 0.60f, 1.00f); // Biru-hijau
    const ImVec4 textColor       = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    const ImVec4 textDisabled    = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    const ImVec4 borderColor     = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

    // Pengaturan umum
    style.WindowPadding     = ImVec2(12, 12);
    style.FramePadding      = ImVec2(8, 6);
    style.ItemSpacing       = ImVec2(12, 8);
    style.ItemInnerSpacing  = ImVec2(8, 6);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing     = 25.0f;
    style.ScrollbarSize     = 15.0f;
    style.GrabMinSize       = 10.0f;

    // Pembulatan sudut
    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 6.0f;

    // Pengaturan border
    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.TabBorderSize     = 1.0f;

    // Pengaturan warna
    colors[ImGuiCol_Text]                  = textColor;
    colors[ImGuiCol_TextDisabled]          = textDisabled;
    colors[ImGuiCol_WindowBg]              = bgColor;
    colors[ImGuiCol_ChildBg]               = lightBgColor;
    colors[ImGuiCol_PopupBg]               = bgColor;
    colors[ImGuiCol_Border]                = borderColor;
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = lightBgColor;
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.15f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.19f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = accentColor;
    colors[ImGuiCol_CheckMark]             = accentColor;
    colors[ImGuiCol_SliderGrab]            = accentColor;
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.98f, 0.59f, 1.00f);
    colors[ImGuiCol_Button]                = accentColor;
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.98f, 0.59f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.25f, 1.00f);
    colors[ImGuiCol_Header]                = accentColor2;
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.80f, 0.90f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.06f, 0.53f, 0.63f, 1.00f);
    colors[ImGuiCol_Separator]             = borderColor;
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered]            = accentColor2;
    colors[ImGuiCol_TabActive]             = accentColor;
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    
    // Tambahan untuk Growtopia
    colors[ImGuiCol_TableHeaderBg]         = accentColor2;
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.15f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.10f, 0.15f, 0.20f, 0.50f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.12f, 0.17f, 0.22f, 0.50f);
}

// Fungsi untuk mengatur tema khusus dialog
void setDialogTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    // Warna khusus untuk dialog
    colors[ImGuiCol_WindowBg]              = ImVec4(0.06f, 0.10f, 0.12f, 0.95f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.08f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.18f, 0.72f, 0.36f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.18f, 0.72f, 0.36f, 0.80f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.98f, 0.59f, 0.80f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.25f, 1.00f);
    
    // Pengaturan khusus untuk dialog
    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
}

// Fungsi untuk mengatur tema khusus loading screen
void setLoadingScreenTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    // Warna khusus untuk loading screen
    colors[ImGuiCol_WindowBg]              = ImVec4(0.05f, 0.08f, 0.10f, 0.98f);
    colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.80f, 0.50f, 1.00f);
    
    // Pengaturan khusus untuk loading screen
    style.WindowRounding = 12.0f;
    style.WindowBorderSize = 0.0f;
}

// Fungsi untuk mengatur tema khusus world view
void setWorldViewTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    // Warna khusus untuk world view
    colors[ImGuiCol_WindowBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // Transparan
    colors[ImGuiCol_ChildBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.70f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.90f);
    colors[ImGuiCol_Border]                = ImVec4(0.18f, 0.72f, 0.36f, 0.50f);
    
    // Pengaturan khusus untuk world view
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
}

void loadCustomFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    
    // Tambahkan font default dengan ukuran yang lebih besar
    io.Fonts->AddFontDefault();
    
    // Catatan: Untuk implementasi lengkap, Anda perlu menambahkan file font ke proyek
    // dan memuat font dari file tersebut. Contoh:
    // io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Bold.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 14.0f);
}


// Fungsi untuk mengembalikan tema default
void resetTheme()
{
    setGrowtopiaTheme();
}