#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

// Struktur untuk merepresentasikan item di client
struct Item {
    int id;
    std::string name;
    std::string type;
    int rarity;
    std::string description;
    int textureId;
    bool collision;
    bool isBackground;
    std::string category;
    nlohmann::json properties;

    // Constructor default
    Item() : id(0), rarity(0), textureId(0), collision(false), isBackground(false) {}

    // Constructor dengan parameter
    Item(int _id, const std::string& _name, const std::string& _type, int _rarity,
         const std::string& _desc, int _texId, bool _collision, bool _isBg, 
         const std::string& _category, const nlohmann::json& _props = nlohmann::json())
        : id(_id), name(_name), type(_type), rarity(_rarity), description(_desc),
          textureId(_texId), collision(_collision), isBackground(_isBg),
          category(_category), properties(_props) {}
};

// Kelas untuk mengelola item
class ItemManager {
private:
    static ItemManager* instance;
    std::map<int, Item> items;
    
    // Private constructor untuk singleton
    ItemManager();

public:
    static ItemManager& getInstance();
    
    // Memuat semua item dari server
    bool loadItems();
    
    // Mendapatkan item berdasarkan ID
    Item getItem(int id) const;
    
    // Mendapatkan semua item
    const std::map<int, Item>& getAllItems() const;
    
    // Mendapatkan item berdasarkan kategori
    std::vector<Item> getItemsByCategory(const std::string& category) const;
    
    // Mendapatkan item berdasarkan tipe (Foreground/Background)
    std::vector<Item> getItemsByType(const std::string& type) const;
    
    // Tambahkan deklarasi fungsi baru
    std::vector<Item> getDefaultItems() const;
};