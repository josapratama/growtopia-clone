#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <iostream>
#include <string>

#include "../../include/core/config.h"
#include "../../include/utils/utils.h"
#include "../../include/menu/main.h"
#include "../../include/login/page.h"
#include "../../include/ui/theme.h"
#include "../../include/world/list.h"
#include "../../include/world/renderer.h"

int main()
{
    // Inisialisasi GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Membuat jendela GLFW
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, APP_TITLE, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Gagal membuat jendela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inisialisasi GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Gagal menginisialisasi GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Aktifkan fitur ImGui yang dibutuhkan
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Navigasi keyboard
    
    // Muat font kustom
    loadCustomFonts();
    
    // Terapkan tema Growtopia
    setGrowtopiaTheme();
    
    // Inisialisasi ImGui untuk GLFW dan OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Variabel untuk menyimpan status
    ImVec4 clear_color = ImVec4(0.05f, 0.07f, 0.10f, 1.00f); // Warna background lebih gelap
    
    // Variabel untuk halaman aplikasi
    AppPage currentPage = MAIN_MENU;
    
    // Variabel untuk kontrol aplikasi
    bool shouldClose = false;
    
    // Tambahkan variabel untuk melacak inisialisasi world
    bool worldInitialized = false;

    // Loop render
    while (!glfwWindowShouldClose(window) && !shouldClose)
    {
        // Input
        processInput(window);

        // Render
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Periksa apakah perlu menginisialisasi world
        if (currentPage == AppPage::WORLD_VIEW && !worldInitialized) {
            initWorldAndPlayer();
            worldInitialized = true;
        }

        // Tampilkan halaman sesuai dengan status
        switch (currentPage) {
            case MAIN_MENU:
                renderMainMenu(currentPage, shouldClose);
                break;
            case LOGIN_PAGE:
                renderLoginPage(currentPage);
                break;
            case WORLD_LIST:
                renderWorldListPage(currentPage);
                break;
            case OPTION_PAGE:
                renderOptionPage(currentPage);
                break;
            case ABOUT_PAGE:
                renderAboutPage(currentPage);
                break;
            case WORLD_VIEW: // Tambahkan case untuk WORLD_VIEW
                renderWorldView(currentPage);
                break;
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers dan poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}