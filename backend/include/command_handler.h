#pragma once
#include <string>
#include <winsock2.h>

// Fungsi untuk memproses perintah dari client
std::string processCommand(const std::string& command, std::string& username, bool& isLoggedIn, SOCKET socket = INVALID_SOCKET);

// Fungsi untuk menangani perintah spesifik
std::string handleLoginCommand(const std::string& params, std::string& username, bool& isLoggedIn, SOCKET socket = INVALID_SOCKET);
std::string handleGuestLoginCommand(const std::string& params, std::string& username, bool& isLoggedIn, SOCKET socket = INVALID_SOCKET);
std::string handleRegisterCommand(const std::string& params);
std::string handleLogoutCommand(std::string& username, bool& isLoggedIn);
std::string handleUsersCommand(const std::string& username, bool isLoggedIn);

// Fitur Baru: World, Farming, Lock & Trading
std::string handleWorldJoinCommand(const std::string& params, const std::string& username, bool isLoggedIn);
std::string handleWorldLockCommand(const std::string& params, const std::string& username, bool isLoggedIn);
std::string handleFarmActionCommand(const std::string& params, const std::string& username, bool isLoggedIn);
std::string handleDropItemCommand(const std::string& params, const std::string& username, bool isLoggedIn);
std::string handleTradeCommand(const std::string& params, const std::string& username, bool isLoggedIn);