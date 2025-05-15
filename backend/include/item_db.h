#pragma once
#include <string>
#include <vector>

struct Item {
    int id;
    std::string name;
    std::string type;
    int rarity;
    std::string description;
};

// Menambahkan item baru ke database
bool addItem(const std::string& name, const std::string& type, int rarity, const std::string& description);

// Mendapatkan item berdasarkan ID
Item getItem(int itemId);

// Mendapatkan semua item
std::vector<Item> getAllItems();

// Menambahkan item ke inventori pengguna
bool addItemToInventory(int userId, int itemId, int quantity);

// Mendapatkan inventori pengguna
std::vector<std::pair<Item, int>> getUserInventory(int userId);