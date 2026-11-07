cmake_minimum_required(VERSION 3.16)

# Set toolchain file jika belum diset (menggunakan relative path agar portable)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

project(growtopia_server)

# Aktifkan C++20 untuk kompatibilitas yang lebih baik
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cari libpqxx dan nlohmann_json
find_package(libpqxx CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Pastikan include path benar
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Kompilasi server dengan semua file sumber
add_executable(GrowtopiaServer 
    src/main.cpp
    src/server.cpp
    src/user_manager.cpp
    src/command_handler.cpp
    src/database.cpp
    src/database_init.cpp
    src/database_items.cpp
    src/database_utils.cpp
    src/backpack_handler.cpp
    src/user_db.cpp
    src/log_db.cpp
    src/bcrypt.cpp
)
target_link_libraries(GrowtopiaServer Ws2_32 libpqxx::pqxx nlohmann_json::nlohmann_json)

# Tambahkan dukungan thread jika menggunakan GCC/Clang
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_libraries(GrowtopiaServer pthread)
endif()
