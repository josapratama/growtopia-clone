#include "../include/database_items.h"
#include "../include/database_config.h"
#include <iostream>
#include <pqxx/pqxx>

namespace db {
    
    // Fungsi untuk menginisialisasi item-item default
    bool initDefaultItems() {
        try {
            pqxx::connection c(CONNECTION_STR);
            pqxx::work txn(c);
            
            // Periksa apakah item sudah ada
            pqxx::result r = txn.exec("SELECT COUNT(*) FROM items");
            int itemCount = r[0][0].as<int>();
            
            if (itemCount > 0) {
                std::cout << "[Database] Item-item sudah ada di database. Menghapus data lama dan mengisi ulang...\n";
                // Hapus semua data dari tabel items
                txn.exec("DELETE FROM items");
                txn.commit();
                std::cout << "Data item-item berhasil dihapus.\n";
                // return true;
            }
                // Buat transaksi baru untuk insert data
                // pqxx::work new_txn(c);
                
                // Lanjutkan dengan insert data
                // Blok item (Foreground)
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                1, "Dirt", "Foreground", 1, "A block of nutritious dirt.", 1, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                2, "Rock", "Foreground", 2, "A chunk of rock. Seems pretty sturdy.", 2, true, false, "Blocks");
        
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                3, "Grass", "Foreground", 1, "A block of grass. It's soft and lush.", 3, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                4, "Lava", "Foreground", 5, "Hot lava. Don't touch!", 4, true, false, "Dangerous");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                5, "Water", "Foreground", 3, "A block of water. Refreshing!", 5, true, false, "Liquids");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                6, "Bedrock", "Foreground", 10, "The hardest substance known to Growtopian.", 6, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                7, "Wood Block", "Foreground", 2, "A block of wood. Useful for building.", 7, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                8, "Stone Wall", "Foreground", 3, "A solid stone wall.", 8, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                9, "Brick", "Foreground", 3, "A sturdy brick block.", 9, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                10, "Sand", "Foreground", 1, "A block of sand. It's coarse and rough and irritating and it gets everywhere.", 10, true, false, "Blocks");
            
            // Background blocks
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                11, "Cave Background", "Background", 1, "A rocky cave background.", 11, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                12, "Dirt Background", "Background", 1, "A dirt background.", 12, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                13, "Rock Background", "Background", 2, "A rock background.", 13, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                14, "Brick Background", "Background", 3, "A brick background.", 14, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                15, "Wood Background", "Background", 2, "A wooden background.", 15, false, true, "Backgrounds");
            
            // Doors dan Entrances
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                16, "Door", "Foreground", 4, "A basic door. Can be locked with a lock.", 16, false, false, "Entrances", "{\"can_be_locked\": true, \"entrance_type\": \"door\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                17, "Steel Door", "Foreground", 5, "A sturdy steel door.", 17, false, false, "Entrances", "{\"can_be_locked\": true, \"entrance_type\": \"door\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                18, "Password Door", "Foreground", 7, "A door that requires a password to enter.", 18, false, false, "Entrances", "{\"can_be_locked\": true, \"entrance_type\": \"password_door\"}");
            
            // Locks
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                19, "World Lock", "Foreground", 10, "The most basic lock. Locks a whole world!", 19, false, false, "Locks", "{\"lock_type\": \"world\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                20, "Small Lock", "Foreground", 8, "Locks a small area.", 20, false, false, "Locks", "{\"lock_type\": \"small\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                21, "Big Lock", "Foreground", 9, "Locks a big area.", 21, false, false, "Locks", "{\"lock_type\": \"big\"}");
            
            // Tools
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                22, "Pickaxe", "Tool", 5, "A basic pickaxe for mining.", 22, false, false, "Tools", "{\"tool_type\": \"mining\", \"tool_power\": 1}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                23, "Stone Pickaxe", "Tool", 6, "A stronger pickaxe for mining.", 23, false, false, "Tools", "{\"tool_type\": \"mining\", \"tool_power\": 2}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                24, "Iron Pickaxe", "Tool", 7, "An even stronger pickaxe for mining.", 24, false, false, "Tools", "{\"tool_type\": \"mining\", \"tool_power\": 3}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                25, "Diamond Pickaxe", "Tool", 8, "The strongest pickaxe for mining.", 25, false, false, "Tools", "{\"tool_type\": \"mining\", \"tool_power\": 4}");
            
            // Lanjutkan dengan item-item lainnya...
            // Seeds
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                26, "Dirt Seed", "Seed", 1, "Plant this to grow a Dirt Tree.", 26, false, false, "Seeds", "{\"grow_time\": 60, \"harvest_item\": 1, \"harvest_count\": 3}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                27, "Rock Seed", "Seed", 2, "Plant this to grow a Rock Tree.", 27, false, false, "Seeds", "{\"grow_time\": 120, \"harvest_item\": 2, \"harvest_count\": 3}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                28, "Grass Seed", "Seed", 1, "Plant this to grow a Grass Tree.", 28, false, false, "Seeds", "{\"grow_time\": 60, \"harvest_item\": 3, \"harvest_count\": 3}");
            
            // Clothing
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                29, "Basic Shirt", "Clothing", 3, "A basic shirt.", 29, false, false, "Clothing", "{\"clothing_type\": \"shirt\", \"clothing_style\": \"basic\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                30, "Basic Pants", "Clothing", 3, "Basic pants.", 30, false, false, "Clothing", "{\"clothing_type\": \"pants\", \"clothing_style\": \"basic\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                31, "Cool Shades", "Clothing", 5, "Some cool shades to wear.", 31, false, false, "Clothing", "{\"clothing_type\": \"face\", \"clothing_style\": \"cool\"}");
            
            // Decorations
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                32, "Torch", "Foreground", 3, "Provides light in dark places.", 32, false, false, "Decorations");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                33, "Chandelier", "Foreground", 6, "A fancy light source.", 33, false, false, "Decorations");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                34, "Painting", "Foreground", 4, "A beautiful painting.", 34, false, false, "Decorations");
            
            // Tambahkan lebih banyak item untuk mencapai 100+ item
            // Blok Khusus
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                35, "Ice Block", "Foreground", 4, "A block of solid ice. It's slippery!", 35, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                36, "Obsidian", "Foreground", 7, "A block of volcanic glass. Very hard.", 36, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                37, "Glass", "Foreground", 4, "A transparent block of glass.", 37, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                38, "Steel Block", "Foreground", 5, "A block of solid steel.", 38, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                39, "Gold Block", "Foreground", 8, "A block of solid gold. Shiny!", 39, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                40, "Diamond Block", "Foreground", 10, "A block of solid diamond. Very rare!", 40, true, false, "Blocks");
            
            // Lebih banyak background
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                41, "Sky Background", "Background", 3, "A beautiful sky background.", 41, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                42, "Space Background", "Background", 6, "A cosmic space background.", 42, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                43, "Lava Background", "Background", 5, "A hot lava background.", 43, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                44, "Water Background", "Background", 3, "A watery background.", 44, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                45, "Jungle Background", "Background", 4, "A lush jungle background.", 45, false, true, "Backgrounds");
            
            // Lebih banyak pintu
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                46, "Golden Door", "Foreground", 8, "A luxurious golden door.", 46, false, false, "Entrances", "{\"can_be_locked\": true, \"entrance_type\": \"door\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                47, "Portal Door", "Foreground", 9, "A mysterious portal door.", 47, false, false, "Entrances", "{\"can_be_locked\": true, \"entrance_type\": \"portal\"}");
            
            // Lebih banyak lock
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                48, "Diamond Lock", "Foreground", 12, "A very rare lock made of diamond.", 48, false, false, "Locks", "{\"lock_type\": \"diamond\"}");
            
            // Lebih banyak tools
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                49, "Hammer", "Tool", 5, "A basic hammer for building.", 49, false, false, "Tools", "{\"tool_type\": \"building\", \"tool_power\": 1}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                50, "Wrench", "Tool", 6, "A wrench for adjusting things.", 50, false, false, "Tools", "{\"tool_type\": \"wrench\", \"tool_power\": 1}");
            
    
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                51, "Lava Seed", "Seed", 5, "Plant this to grow a Lava Tree. Be careful!", 51, false, false, "Seeds", "{\"grow_time\": 300, \"harvest_item\": 4, \"harvest_count\": 2}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                52, "Ice Seed", "Seed", 4, "Plant this to grow an Ice Tree.", 52, false, false, "Seeds", "{\"grow_time\": 180, \"harvest_item\": 35, \"harvest_count\": 3}");
            
            // Platforms
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                53, "Wooden Platform", "Foreground", 3, "A wooden platform you can jump through.", 53, true, false, "Platforms");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                54, "Steel Platform", "Foreground", 5, "A steel platform you can jump through.", 54, true, false, "Platforms");
            
            // More clothing
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                55, "Fancy Shirt", "Clothing", 6, "A fancy shirt for special occasions.", 55, false, false, "Clothing", "{\"clothing_type\": \"shirt\", \"clothing_style\": \"fancy\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                56, "Fancy Pants", "Clothing", 6, "Fancy pants for special occasions.", 56, false, false, "Clothing", "{\"clothing_type\": \"pants\", \"clothing_style\": \"fancy\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                57, "Wizard Hat", "Clothing", 7, "A magical wizard hat.", 57, false, false, "Clothing", "{\"clothing_type\": \"hat\", \"clothing_style\": \"wizard\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                58, "Ninja Mask", "Clothing", 7, "A stealthy ninja mask.", 58, false, false, "Clothing", "{\"clothing_type\": \"face\", \"clothing_style\": \"ninja\"}");
            
            // Special blocks
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                59, "Spike", "Foreground", 5, "A dangerous spike. Don't touch!", 59, true, false, "Dangerous", "{\"damage\": 10}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                60, "Bouncer", "Foreground", 6, "A block that makes you bounce!", 60, true, false, "Special", "{\"bounce_power\": 20}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                61, "Speed Block", "Foreground", 6, "A block that makes you run faster!", 61, true, false, "Special", "{\"speed_boost\": 1.5}");
            
            // More decorations
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                62, "Flower Pot", "Foreground", 3, "A decorative flower pot.", 62, false, false, "Decorations");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                63, "Statue", "Foreground", 7, "A decorative statue.", 63, false, false, "Decorations");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                64, "Bookshelf", "Foreground", 5, "A decorative bookshelf.", 64, true, false, "Decorations");
            
            // Furniture
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                65, "Chair", "Foreground", 4, "A comfortable chair to sit on.", 65, false, false, "Furniture");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                66, "Table", "Foreground", 4, "A sturdy table.", 66, true, false, "Furniture");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                67, "Bed", "Foreground", 5, "A comfortable bed to sleep on.", 67, true, false, "Furniture");
            
            // Weather items
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                68, "Weather Machine - Sunny", "Foreground", 8, "Changes the weather to sunny.", 68, false, false, "Weather", "{\"weather_type\": \"sunny\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                69, "Weather Machine - Rainy", "Foreground", 8, "Changes the weather to rainy.", 69, false, false, "Weather", "{\"weather_type\": \"rainy\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                70, "Weather Machine - Snowy", "Foreground", 8, "Changes the weather to snowy.", 70, false, false, "Weather", "{\"weather_type\": \"snowy\"}");
            
            // Music blocks
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                71, "Music Block - Piano", "Foreground", 6, "Plays piano notes when hit.", 71, true, false, "Music", "{\"instrument\": \"piano\", \"note\": \"C\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                72, "Music Block - Guitar", "Foreground", 6, "Plays guitar notes when hit.", 72, true, false, "Music", "{\"instrument\": \"guitar\", \"note\": \"C\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                73, "Music Block - Drums", "Foreground", 6, "Plays drum beats when hit.", 73, true, false, "Music", "{\"instrument\": \"drums\", \"note\": \"kick\"}");
            
            // Consumables
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                74, "Speed Potion", "Consumable", 5, "Gives you a temporary speed boost.", 74, false, false, "Potions", "{\"effect\": \"speed\", \"effect_value\": 2, \"duration\": 60}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                75, "Jump Potion", "Consumable", 5, "Gives you a temporary jump boost.", 75, false, false, "Potions", "{\"effect\": \"jump\", \"duration\": 30}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                76, "Invisibility Potion", "Consumable", 7, "Makes you temporarily invisible.", 76, false, false, "Potions", "{\"effect\": \"invisibility\", \"duration\": 20}");
            
            // Gems and currency
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                77, "Gem", "Currency", 3, "The main currency in Growtopia.", 77, false, false, "Currency", "{\"value\": 1}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                78, "Growtoken", "Currency", 10, "A special token used for rare items.", 78, false, false, "Currency", "{\"value\": 100}");
            
            // More blocks
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                79, "Marble Block", "Foreground", 6, "A smooth marble block.", 79, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                80, "Granite Block", "Foreground", 5, "A sturdy granite block.", 80, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                81, "Sandstone", "Foreground", 4, "A block of compressed sand.", 81, true, false, "Blocks");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                82, "Mud Block", "Foreground", 2, "A block of wet mud. Slows you down.", 82, true, false, "Blocks");
            
            // More backgrounds
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                83, "Marble Background", "Background", 6, "A smooth marble background.", 83, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                84, "Granite Background", "Background", 5, "A sturdy granite background.", 84, false, true, "Backgrounds");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                85, "Sandstone Background", "Background", 4, "A sandstone background.", 85, false, true, "Backgrounds");
            
            // More tools
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                86, "Fishing Rod", "Tool", 5, "Used for fishing in water.", 86, false, false, "Tools", "{\"tool_type\": \"fishing\", \"tool_power\": 1}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                87, "Shovel", "Tool", 4, "Used for digging.", 87, false, false, "Tools", "{\"tool_type\": \"digging\", \"tool_power\": 1}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                88, "Axe", "Tool", 4, "Used for chopping trees.", 88, false, false, "Tools", "{\"tool_type\": \"chopping\", \"tool_power\": 1}");
            
            // Special items
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                89, "Checkpoint", "Foreground", 7, "Saves your position when you touch it.", 89, false, false, "Special", "{\"checkpoint\": true}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                90, "Teleporter", "Foreground", 8, "Teleports you to another teleporter.", 90, false, false, "Special", "{\"teleport\": true}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                91, "Treasure Chest", "Foreground", 7, "Contains random valuable items.", 91, true, false, "Special", "{\"contains_items\": true}");
            
            // More clothing
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                92, "Royal Crown", "Clothing", 9, "A majestic royal crown.", 92, false, false, "Clothing", "{\"clothing_type\": \"hat\", \"clothing_style\": \"royal\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                93, "Royal Robe", "Clothing", 9, "A majestic royal robe.", 93, false, false, "Clothing", "{\"clothing_type\": \"shirt\", \"clothing_style\": \"royal\"}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                94, "Angel Wings", "Clothing", 10, "Majestic angel wings.", 94, false, false, "Clothing", "{\"clothing_type\": \"back\", \"clothing_style\": \"angel\"}");
            
            // More seeds
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                95, "Gold Seed", "Seed", 8, "Plant this to grow a Gold Tree.", 95, false, false, "Seeds", "{\"grow_time\": 600, \"harvest_item\": 39, \"harvest_count\": 1}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                96, "Diamond Seed", "Seed", 10, "Plant this to grow a Diamond Tree.", 96, false, false, "Seeds", "{\"grow_time\": 1200, \"harvest_item\": 40, \"harvest_count\": 1}");
            
            // More decorations
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                97, "Fountain", "Foreground", 7, "A beautiful decorative fountain.", 97, true, false, "Decorations");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                98, "Grandfather Clock", "Foreground", 6, "A stately grandfather clock.", 98, true, false, "Decorations");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                99, "Fireplace", "Foreground", 6, "A cozy fireplace.", 99, true, false, "Decorations");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                100, "Aquarium", "Foreground", 7, "A beautiful aquarium with fish.", 100, true, false, "Decorations");
            
            // Rare items
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                101, "Dragon Egg", "Foreground", 10, "A rare dragon egg. What will hatch from it?", 101, true, false, "Rare", "{\"hatchable\": true}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                102, "Magic Crystal", "Foreground", 9, "A magical crystal with unknown powers.", 102, true, false, "Rare", "{\"magical\": true}");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                103, "Ancient Artifact", "Foreground", 9, "An ancient artifact from a lost civilization.", 103, true, false, "Rare", "{\"ancient\": true}");
            
            // Event items
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                104, "Halloween Pumpkin", "Foreground", 7, "A spooky Halloween pumpkin.", 104, true, false, "Seasonal");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                105, "Christmas Tree", "Foreground", 7, "A festive Christmas tree.", 105, true, false, "Seasonal");
            
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
                106, "Valentine Heart", "Foreground", 7, "A lovely Valentine's Day heart.", 106, true, false, "Seasonal");
            
                        
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                107, "Hammer", "Tool", 5, "A basic hammer for building.", 49, false, false, "Tools", "{\"tool_type\": \"building\", \"tool_power\": 1}");
            
            // More seeds
            txn.exec_params("INSERT INTO items (id, name, type, rarity, description, texture_id, collision, is_background, item_category, item_properties) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",
                108, "Sand Seed", "Seed", 1, "Plant this to grow a Sand Tree.", 50, false, false, "Seeds", "{\"grow_time\": 60, \"harvest_item\": 10, \"harvest_count\": 3}");
            
            txn.commit();
            std::cout << "[Database] Berhasil menginisialisasi " << 110 << " item default.\n";
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "[Database Error] " << e.what() << std::endl;
            return false;
        }
    }
}