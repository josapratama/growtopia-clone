#include "../include/user_manager.h"
#include <random>
#include <iostream>

// Implementasi Singleton
UserManager& UserManager::getInstance() {
    static UserManager instance;
    return instance;
}

// Menambahkan pengguna ke daftar aktif
bool UserManager::addActiveUser(const std::string& username, SOCKET socket) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    // Periksa apakah pengguna sudah aktif
    if (activeUsers.find(username) != activeUsers.end()) {
        return false;
    }
    
    // Tambahkan ke daftar
    User newUser = {username, true, socket};
    activeUsers[username] = newUser;
    return true;
}

// Menghapus pengguna dari daftar aktif
void UserManager::removeActiveUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(usersMutex);
    activeUsers.erase(username);
}

// Memeriksa apakah pengguna sudah aktif
bool UserManager::isUserActive(const std::string& username) {
    std::lock_guard<std::mutex> lock(usersMutex);
    return activeUsers.find(username) != activeUsers.end();
}

// Mendapatkan daftar pengguna aktif
std::string UserManager::getActiveUsersList() {
    std::string result = "Pengguna aktif:\n";
    std::lock_guard<std::mutex> lock(usersMutex);
    
    for (const auto& user : activeUsers) {
        result += "- " + user.first + "\n";
    }
    
    return result;
}

// Menghasilkan username acak
std::string UserManager::generateRandomUsername() {
    static const std::string prefixes[] = {"Player", "Grower", "Farmer", "Builder", "Miner", "Wizard"};
    static const int numPrefixes = 6;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> prefixDist(0, numPrefixes - 1);
    std::uniform_int_distribution<> numberDist(1000, 9999);
    
    std::string username = prefixes[prefixDist(gen)] + std::to_string(numberDist(gen));
    return username;
}