cmake_minimum_required(VERSION 3.10)
project(GrowtopiaApp)

set(CMAKE_CXX_STANDARD 17)

# Gunakan vcpkg toolchain file dengan path absolut
set(CMAKE_TOOLCHAIN_FILE "D:/coding/bu-aini/growtopia/frontend/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Temukan paket yang diperlukan
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
# Add these new packages
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(SFML COMPONENTS Graphics Window Network Audio System CONFIG REQUIRED)

# Tambahkan executable dengan semua source files
add_executable(GrowtopiaApp 
    src/core/main.cpp
    src/utils/utils.cpp
    src/menu/main.cpp
    src/login/page.cpp
    src/login/state.cpp
    src/login/ui.cpp
    src/network/client.cpp
    src/ui/theme.cpp
    src/world/list.cpp
    src/world/graphics.cpp
    src/world/renderer.cpp
    src/core/option.cpp
    src/inventory/backpack.cpp
    src/inventory/item.cpp
    src/world/ui/chat.cpp
    src/world/player.cpp
    src/world/terrain.cpp
    src/world/ui/settings.cpp
    src/world/ui/social.cpp
    src/world/ui/shop.cpp
    src/world/ui/info.cpp
    src/world/utils/drawing.cpp
    src/world/globals.cpp
    src/inventory/ui/backpack_ui.cpp
)

# Link libraries
target_link_libraries(GrowtopiaApp PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    imgui::imgui
    # Add these new libraries
    nlohmann_json::nlohmann_json
    cpprestsdk::cpprest
    SFML::Graphics SFML::Window SFML::Network SFML::Audio SFML::System
)
