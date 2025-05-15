#pragma once
#include <string>
#include <vector>

struct LogEntry {
    int id;
    int userId;
    std::string action;
    std::string description;
    std::string timestamp;
};

// Mencatat aksi pengguna
bool logUserAction(int userId, const std::string& action, const std::string& description);

// Melihat log pengguna
std::vector<LogEntry> viewUserLogs(int userId);

// Memblokir pengguna
bool banUser(int userId, const std::string& reason, const std::string& bannedBy);

// Memeriksa apakah pengguna diblokir
bool isUserBanned(int userId);