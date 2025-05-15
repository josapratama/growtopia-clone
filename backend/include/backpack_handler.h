#pragma once

#include <string>
#include <nlohmann/json.hpp>

// Fungsi untuk menyimpan backpack ke database
bool saveBackpack(int userId, const nlohmann::json& items);

// Fungsi untuk memuat backpack dari database
nlohmann::json loadBackpack(int userId);

// Fungsi untuk menambahkan item ke backpack
bool addItemToBackpack(int userId, int itemId, int count);

// Fungsi untuk menghapus item dari backpack
bool removeItemFromBackpack(int userId, int itemId, int count);