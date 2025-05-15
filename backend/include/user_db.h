#pragma once
#include <string>

// Registrasi pengguna baru
bool registerUser(const std::string& username, const std::string& password_hash);

// Autentikasi pengguna
bool authenticateUser(const std::string& username, const std::string& password_hash);

// Mendapatkan ID pengguna berdasarkan username
int getUserId(const std::string& username);

// Mendapatkan username berdasarkan ID
std::string getUsername(int userId);