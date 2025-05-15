#include "../include/log_db.h"
#include "../include/database.h"
#include "../include/database_config.h"
#include <iostream>
#include <pqxx/pqxx>

// Mencatat aksi pengguna
bool logUserAction(int userId, const std::string& action, const std::string& description) {
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        txn.exec_params(
            "INSERT INTO logs (user_id, action, description) VALUES ($1, $2, $3)",
            userId, action, description
        );

        txn.commit();
        std::cout << "[Log] Aksi '" << action << "' dicatat untuk user ID " << userId << ".\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Log Error] " << e.what() << std::endl;
        return false;
    }
}

// Melihat log pengguna
std::vector<LogEntry> viewUserLogs(int userId) {
    std::vector<LogEntry> logs;
    
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        pqxx::result r = txn.exec_params(
            "SELECT id, user_id, action, description, timestamp FROM logs WHERE user_id = $1 ORDER BY timestamp DESC",
            userId
        );

        for (const auto& row : r) {
            LogEntry log;
            log.id = row["id"].as<int>();
            log.userId = row["user_id"].as<int>();
            log.action = row["action"].as<std::string>();
            log.description = row["description"].as<std::string>();
            log.timestamp = row["timestamp"].as<std::string>();
            logs.push_back(log);
        }
        
        return logs;
    } catch (const std::exception& e) {
        std::cerr << "[Log View Error] " << e.what() << std::endl;
        return logs;
    }
}

// Memblokir pengguna
bool banUser(int userId, const std::string& reason, const std::string& bannedBy) {
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        txn.exec_params(
            "INSERT INTO bans (user_id, reason, banned_by) VALUES ($1, $2, $3) "
            "ON CONFLICT (user_id) DO UPDATE SET reason = $2, banned_by = $3, banned_at = CURRENT_TIMESTAMP",
            userId, reason, bannedBy
        );

        txn.commit();
        std::cout << "[Ban] User ID " << userId << " telah diblokir oleh " << bannedBy << ".\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Ban Error] " << e.what() << std::endl;
        return false;
    }
}

// Memeriksa apakah pengguna diblokir
bool isUserBanned(int userId) {
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        pqxx::result r = txn.exec_params("SELECT 1 FROM bans WHERE user_id = $1", userId);
        return !r.empty();
    } catch (const std::exception& e) {
        std::cerr << "[Ban Check Error] " << e.what() << std::endl;
        return false;
    }
}