#include "../include/user_db.h"
#include "../include/database.h"
#include "../include/database_config.h"
#include <iostream>
#include <pqxx/pqxx>

// Registrasi pengguna baru
bool registerUser(const std::string& username, const std::string& password_hash) {
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        // Periksa apakah username sudah digunakan
        pqxx::result r = txn.exec_params("SELECT 1 FROM users WHERE username = $1", username);
        if (!r.empty()) {
            std::cout << "[Database] Username '" << username << "' sudah digunakan.\n";
            return false;
        }

        txn.exec_params("INSERT INTO users (username, password) VALUES ($1, $2)", username, password_hash);
        txn.commit();
        std::cout << "[Database] Pengguna '" << username << "' berhasil didaftarkan.\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Database Error] " << e.what() << std::endl;
        return false;
    }
}

// Autentikasi pengguna
bool authenticateUser(const std::string& username, const std::string& password_hash) {
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        pqxx::result r = txn.exec_params("SELECT password FROM users WHERE username = $1", username);
        if (r.empty()) return false; // Username tidak ditemukan

        std::string storedPassword = r[0][0].as<std::string>();
        if (storedPassword == password_hash) {
            txn.exec_params("UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE username = $1", username);
            txn.commit();
            return true;
        }

        return false;
    } catch (const std::exception& e) {
        std::cerr << "[Database Error] " << e.what() << std::endl;
        return false;
    }
}

// Mendapatkan ID pengguna berdasarkan username
int getUserId(const std::string& username) {
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        pqxx::result r = txn.exec_params("SELECT id FROM users WHERE username = $1", username);
        if (r.empty()) return -1; // Username tidak ditemukan

        return r[0][0].as<int>();
    } catch (const std::exception& e) {
        std::cerr << "[Database Error] " << e.what() << std::endl;
        return -1;
    }
}

// Mendapatkan username berdasarkan ID
std::string getUsername(int userId) {
    try {
        pqxx::connection c(CONNECTION_STR);
        pqxx::work txn(c);

        pqxx::result r = txn.exec_params("SELECT username FROM users WHERE id = $1", userId);
        if (r.empty()) return ""; // ID tidak ditemukan

        return r[0][0].as<std::string>();
    } catch (const std::exception& e) {
        std::cerr << "[Database Error] " << e.what() << std::endl;
        return "";
    }
}