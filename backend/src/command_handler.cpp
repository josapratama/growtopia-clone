#include "../include/command_handler.h"
#include "../include/user_manager.h"
#include "../include/database.h"
#include "../include/bcrypt.h"
#include "../include/user_db.h"
#include "../include/database_config.h"
#include <iostream>
#include <random>
#include <pqxx/pqxx>

// Fungsi untuk memproses perintah dari client
std::string processCommand(const std::string& command, std::string& username, bool& isLoggedIn) {
    // Format perintah: COMMAND|parameter1|parameter2|...
    size_t sep = command.find('|');
    if (sep == std::string::npos) {
        return "Format perintah tidak valid";
    }
    
    std::string cmd = command.substr(0, sep);
    std::string params = (sep < command.length() - 1) ? command.substr(sep + 1) : "";
    
    if (cmd == "LOGIN") {
        return handleLoginCommand(params, username, isLoggedIn);
    } 
    else if (cmd == "GUEST_LOGIN") {
        return handleGuestLoginCommand(params, username, isLoggedIn);
    }
    else if (cmd == "REGISTER") {
        return handleRegisterCommand(params);
    }
    else if (cmd == "LOGOUT") {
        return handleLogoutCommand(username, isLoggedIn);
    }
    else if (cmd == "PING") {
        return "PONG";
    }
    else if (cmd == "USERS") {
        return handleUsersCommand(username, isLoggedIn);
    }
    else {
        return "Perintah tidak dikenal";
    }
}

// Fungsi untuk menangani perintah login
std::string handleLoginCommand(const std::string& params, std::string& username, bool& isLoggedIn) {
    // Format: LOGIN|username|password
    size_t sep = params.find('|');
    if (sep == std::string::npos) {
        return "Format login tidak valid";
    }
    
    std::string user = params.substr(0, sep);
    std::string pass = params.substr(sep + 1);
    
    // Hash password
    std::string hashedPass = bcrypt::generateHash(pass);
    
    // Autentikasi dengan database
    if (authenticateUser(user, hashedPass)) {
        username = user;
        isLoggedIn = true;
        
        // Tambahkan ke daftar pengguna aktif
        if (!UserManager::getInstance().addActiveUser(username, INVALID_SOCKET)) {
            return "Pengguna sudah login di perangkat lain";
        }
        
        return "Login berhasil";
    } else {
        return "Username atau password salah";
    }
}

// Fungsi untuk menangani perintah guest login
std::string handleGuestLoginCommand(const std::string& params, std::string& username, bool& isLoggedIn) {
    // Format: GUEST_LOGIN|username
    std::string user = params;
    
    // Jika username kosong, buat username acak
    if (user.empty()) {
        bool usernameFound = false;
        int attempts = 0;
        
        // Coba hingga 10 kali untuk mendapatkan username yang belum digunakan
        while (!usernameFound && attempts < 10) {
            user = UserManager::getInstance().generateRandomUsername();
            
            // Periksa apakah username sudah ada di database
            pqxx::connection c(CONNECTION_STR);
            pqxx::work txn(c);
            pqxx::result r = txn.exec_params("SELECT 1 FROM users WHERE username = $1", user);
            
            if (r.empty()) {
                usernameFound = true;
            }
            
            attempts++;
        }
        
        if (!usernameFound) {
            return "Gagal membuat username acak. Silakan coba lagi.";
        }
    }
    
    // Buat password acak untuk akun tamu
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(100000, 999999);
    std::string randomPass = std::to_string(dist(gen));
    
    // Hash password
    std::string hashedPass = bcrypt::generateHash(randomPass);
    
    // Daftarkan ke database
    if (registerUser(user, hashedPass)) {
        username = user;
        isLoggedIn = true;
        
        // Tambahkan ke daftar pengguna aktif
        UserManager::getInstance().addActiveUser(username, INVALID_SOCKET);
        
        return "Login berhasil sebagai " + user;
    } else {
        return "Gagal membuat akun. Silakan coba lagi.";
    }
}

// Fungsi untuk menangani perintah register
std::string handleRegisterCommand(const std::string& params) {
    // Format: REGISTER|username|password
    size_t sep = params.find('|');
    if (sep == std::string::npos) {
        return "Format registrasi tidak valid";
    }
    
    std::string user = params.substr(0, sep);
    std::string pass = params.substr(sep + 1);
    
    // Validasi username dan password
    if (user.length() < 3 || user.length() > 16) {
        return "Username harus 3-16 karakter";
    }
    
    if (pass.length() < 6) {
        return "Password minimal 6 karakter";
    }
    
    // Hash password
    std::string hashedPass = bcrypt::generateHash(pass);
    
    // Daftarkan ke database
    if (registerUser(user, hashedPass)) {
        return "Registrasi berhasil! Silakan login.";
    } else {
        return "Registrasi gagal. Username mungkin sudah digunakan.";
    }
}

// Fungsi untuk menangani perintah logout
std::string handleLogoutCommand(std::string& username, bool& isLoggedIn) {
    if (!isLoggedIn) {
        return "Anda belum login";
    }
    
    // Hapus dari daftar pengguna aktif
    UserManager::getInstance().removeActiveUser(username);
    
    isLoggedIn = false;
    username = "";
    return "Logout berhasil";
}

// Fungsi untuk menangani perintah users
std::string handleUsersCommand(const std::string& username, bool isLoggedIn) {
    if (!isLoggedIn) {
        return "Anda harus login terlebih dahulu";
    }
    
    return UserManager::getInstance().getActiveUsersList();
}