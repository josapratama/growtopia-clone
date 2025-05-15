#include "../include/database_init.h"
#include "../include/database_config.h"
#include "../include/database_items.h"
#include <iostream>
#include <pqxx/pqxx>

namespace db {

// Inisialisasi semua tabel
bool initDatabase() {
    try {
        pqxx::connection c(CONNECTION_STR);
        
        // Periksa apakah tabel sudah ada
        bool tablesExist = false;
        try {
            pqxx::work check_txn(c);
            pqxx::result r = check_txn.exec("SELECT 1 FROM users LIMIT 1");
            tablesExist = true;
            check_txn.commit();
        } catch (const std::exception&) {
            // Tabel belum ada, lanjutkan dengan pembuatan tabel
        }

        if (tablesExist) {
            std::cout << "[Database] Tabel-tabel sudah ada. Mereset database...\n";
            // Drop semua tabel dalam urutan yang benar (memperhatikan foreign key constraints)
            pqxx::work drop_txn(c);
            drop_txn.exec("DROP TABLE IF EXISTS bans CASCADE");
            drop_txn.exec("DROP TABLE IF EXISTS logs CASCADE");
            drop_txn.exec("DROP TABLE IF EXISTS user_items CASCADE");
            drop_txn.exec("DROP TABLE IF EXISTS worlds CASCADE");
            drop_txn.exec("DROP TABLE IF EXISTS items CASCADE");
            drop_txn.exec("DROP TABLE IF EXISTS users CASCADE");
            drop_txn.commit();
        }
        
        // Buat transaksi baru untuk membuat tabel
        pqxx::work create_txn(c);

        // Table: users
        create_txn.exec(R"(
            CREATE TABLE IF NOT EXISTS users (
                id SERIAL PRIMARY KEY,
                username TEXT UNIQUE NOT NULL,
                password TEXT NOT NULL,
                register_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                last_login TIMESTAMP
            )
        )");

        // Table: items
        create_txn.exec(R"(
            CREATE TABLE IF NOT EXISTS items (
                id SERIAL PRIMARY KEY,
                name TEXT NOT NULL,
                type TEXT,
                rarity INTEGER,
                description TEXT,
                texture_id INTEGER,
                collision BOOLEAN DEFAULT TRUE,
                is_background BOOLEAN DEFAULT FALSE,
                item_category TEXT,
                item_properties JSONB
            )
        )");

        // Table: user_items (inventory)
        create_txn.exec(R"(
            CREATE TABLE IF NOT EXISTS user_items (
                id SERIAL PRIMARY KEY,
                user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
                item_id INTEGER REFERENCES items(id) ON DELETE CASCADE,
                quantity INTEGER NOT NULL DEFAULT 0,
                UNIQUE(user_id, item_id)
            )
        )");

        // Table: worlds
        create_txn.exec(R"(
            CREATE TABLE IF NOT EXISTS worlds (
                id SERIAL PRIMARY KEY,
                world_name TEXT UNIQUE NOT NULL,
                owner_id INTEGER REFERENCES users(id) ON DELETE SET NULL,
                width INTEGER DEFAULT 100,
                height INTEGER DEFAULT 60,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                locked BOOLEAN DEFAULT FALSE
            )
        )");

        // Table: logs
        create_txn.exec(R"(
            CREATE TABLE IF NOT EXISTS logs (
                id SERIAL PRIMARY KEY,
                user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
                action TEXT NOT NULL,
                description TEXT,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");

        // Table: bans
        create_txn.exec(R"(
            CREATE TABLE IF NOT EXISTS bans (
                user_id INTEGER PRIMARY KEY REFERENCES users(id) ON DELETE CASCADE,
                reason TEXT NOT NULL,
                banned_by TEXT NOT NULL,
                banned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");

        create_txn.commit();
        std::cout << "[Database] Semua tabel berhasil diinisialisasi.\n";
        
        // Inisialisasi item-item default
        initDefaultItems();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Database Error] " << e.what() << std::endl;
        return false;
    }
}

} // namespace db