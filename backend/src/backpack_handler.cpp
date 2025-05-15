#include "../include/backpack_handler.h"
#include "../include/database_config.h"
#include <iostream>
#include <pqxx/pqxx>

// Fungsi untuk menyimpan backpack ke database
bool saveBackpack(int userId, const nlohmann::json& items) {
    try {
        pqxx::connection c(db::CONNECTION_STR);
        pqxx::work txn(c);
        
        // Hapus item yang ada di backpack user
        txn.exec_params("DELETE FROM user_items WHERE user_id = $1", userId);
        
        // Tambahkan item baru
        for (auto& item : items.items()) {
            int itemId = std::stoi(item.key());
            int count = item.value().get<int>();
            
            if (count > 0) {
                txn.exec_params("INSERT INTO user_items (user_id, item_id, quantity) VALUES ($1, $2, $3)",
                    userId, itemId, count);
            }
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Backpack Error] " << e.what() << std::endl;
        return false;
    }
}

// Fungsi untuk memuat backpack dari database
nlohmann::json loadBackpack(int userId) {
    nlohmann::json backpack = nlohmann::json::object();
    
    try {
        pqxx::connection c(db::CONNECTION_STR);
        pqxx::work txn(c);
        
        // Ambil semua item milik user
        pqxx::result r = txn.exec_params(
            "SELECT ui.item_id, ui.quantity, i.name, i.type, i.rarity, i.description, i.texture_id, i.item_category, i.item_properties "
            "FROM user_items ui "
            "JOIN items i ON ui.item_id = i.id "
            "WHERE ui.user_id = $1", 
            userId);
        
        // Buat struktur JSON untuk backpack
        for (auto row : r) {
            int itemId = row["item_id"].as<int>();
            int quantity = row["quantity"].as<int>();
            std::string name = row["name"].as<std::string>();
            std::string type = row["type"].as<std::string>();
            int rarity = row["rarity"].as<int>();
            std::string description = row["description"].as<std::string>();
            int textureId = row["texture_id"].as<int>();
            std::string category = row["item_category"].as<std::string>();
            
            // Buat objek item dengan informasi lengkap
            nlohmann::json itemInfo = {
                {"id", itemId},
                {"name", name},
                {"type", type},
                {"rarity", rarity},
                {"description", description},
                {"texture_id", textureId},
                {"category", category},
                {"quantity", quantity}
            };
            
            // Tambahkan properties jika ada
            if (!row["item_properties"].is_null()) {
                std::string propsStr = row["item_properties"].as<std::string>();
                itemInfo["properties"] = nlohmann::json::parse(propsStr);
            }
            
            // Tambahkan ke backpack dengan ID sebagai key
            backpack[std::to_string(itemId)] = itemInfo;
        }
        
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "[Backpack Error] " << e.what() << std::endl;
    }
    
    return backpack;
}

// Fungsi untuk menambahkan item ke backpack
bool addItemToBackpack(int userId, int itemId, int count) {
    if (count <= 0) return false;
    
    try {
        pqxx::connection c(db::CONNECTION_STR);
        pqxx::work txn(c);
        
        // Periksa apakah item sudah ada di backpack
        pqxx::result r = txn.exec_params(
            "SELECT quantity FROM user_items WHERE user_id = $1 AND item_id = $2",
            userId, itemId);
        
        if (r.size() > 0) {
            // Update jumlah item yang sudah ada
            int currentCount = r[0][0].as<int>();
            txn.exec_params(
                "UPDATE user_items SET quantity = $1 WHERE user_id = $2 AND item_id = $3",
                currentCount + count, userId, itemId);
        } else {
            // Tambahkan item baru ke backpack
            txn.exec_params(
                "INSERT INTO user_items (user_id, item_id, quantity) VALUES ($1, $2, $3)",
                userId, itemId, count);
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Backpack Error] " << e.what() << std::endl;
        return false;
    }
}

// Fungsi untuk menghapus item dari backpack
bool removeItemFromBackpack(int userId, int itemId, int count) {
    if (count <= 0) return false;
    
    try {
        pqxx::connection c(db::CONNECTION_STR);
        pqxx::work txn(c);
        
        // Periksa jumlah item yang dimiliki
        pqxx::result r = txn.exec_params(
            "SELECT quantity FROM user_items WHERE user_id = $1 AND item_id = $2",
            userId, itemId);
        
        if (r.size() == 0) {
            // Item tidak ditemukan
            return false;
        }
        
        int currentCount = r[0][0].as<int>();
        
        if (currentCount <= count) {
            // Hapus item jika jumlahnya kurang dari atau sama dengan yang akan dihapus
            txn.exec_params(
                "DELETE FROM user_items WHERE user_id = $1 AND item_id = $2",
                userId, itemId);
        } else {
            // Kurangi jumlah item
            txn.exec_params(
                "UPDATE user_items SET quantity = $1 WHERE user_id = $2 AND item_id = $3",
                currentCount - count, userId, itemId);
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Backpack Error] " << e.what() << std::endl;
        return false;
    }
}