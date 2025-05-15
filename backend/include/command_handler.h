#pragma once
#include <string>

// Konstanta koneksi database
extern const std::string CONNECTION_STR;

// Fungsi untuk memproses perintah dari client
std::string processCommand(const std::string& command, std::string& username, bool& isLoggedIn);

// Fungsi untuk menangani perintah spesifik
std::string handleLoginCommand(const std::string& params, std::string& username, bool& isLoggedIn);
std::string handleGuestLoginCommand(const std::string& params, std::string& username, bool& isLoggedIn);
std::string handleRegisterCommand(const std::string& params);
std::string handleLogoutCommand(std::string& username, bool& isLoggedIn);
std::string handleUsersCommand(const std::string& username, bool isLoggedIn);