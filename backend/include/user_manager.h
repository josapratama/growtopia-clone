#pragma once
#include <string>
#include <map>
#include <mutex>
#include <winsock2.h>

// Struktur untuk menyimpan informasi pengguna
struct User {
    std::string username;
    bool isLoggedIn;
    SOCKET socket;
};

// Kelas untuk mengelola pengguna
class UserManager {
public:
    static UserManager& getInstance();
    
    // Menambahkan pengguna ke daftar aktif
    bool addActiveUser(const std::string& username, SOCKET socket);
    
    // Menghapus pengguna dari daftar aktif
    void removeActiveUser(const std::string& username);
    
    // Memeriksa apakah pengguna sudah aktif
    bool isUserActive(const std::string& username);
    
    // Mendapatkan daftar pengguna aktif
    std::string getActiveUsersList();
    
    // Menghasilkan username acak
    std::string generateRandomUsername();

private:
    UserManager() {}  // Singleton
    
    std::map<std::string, User> activeUsers;
    std::mutex usersMutex;
};