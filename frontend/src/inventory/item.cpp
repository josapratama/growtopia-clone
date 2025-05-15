#include "../../include/inventory/item.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

// Inisialisasi instance singleton
ItemManager* ItemManager::instance = nullptr;

ItemManager::ItemManager() {
    // Konstruktor kosong
}

ItemManager& ItemManager::getInstance() {
    if (instance == nullptr) {
        instance = new ItemManager();
    }
    return *instance;
}

bool ItemManager::loadItems() {
    try {
        // Buat HTTP client untuk terhubung ke server
        http_client client(U("http://localhost:5000"));
        
        // Buat request untuk mendapatkan daftar item
        uri_builder builder(U("/items"));
        
        // Kirim request GET
        http_request request(methods::GET);
        request.set_request_uri(builder.to_uri());
        
        // Terima response
        http_response response = client.request(request).get();
        
        // Periksa status response
        if (response.status_code() == status_codes::OK) {
            // Parse JSON response
            json::value jsonItems = response.extract_json().get();
            
            // Iterasi melalui array item
            for (const auto& item : jsonItems.as_array()) {
                int id = item.at(U("id")).as_integer();
                std::string name = utility::conversions::to_utf8string(item.at(U("name")).as_string());
                std::string type = utility::conversions::to_utf8string(item.at(U("type")).as_string());
                int rarity = item.at(U("rarity")).as_integer();
                std::string description = utility::conversions::to_utf8string(item.at(U("description")).as_string());
                int textureId = item.at(U("texture_id")).as_integer();
                bool collision = item.at(U("collision")).as_bool();
                bool isBackground = item.at(U("is_background")).as_bool();
                std::string category = utility::conversions::to_utf8string(item.at(U("item_category")).as_string());
                
                // Parse properties jika ada
                nlohmann::json properties;
                if (item.has_field(U("item_properties"))) {
                    std::string propsStr = utility::conversions::to_utf8string(item.at(U("item_properties")).as_string());
                    if (!propsStr.empty()) {
                        properties = nlohmann::json::parse(propsStr);
                    }
                }
                
                // Buat objek Item dan tambahkan ke map
                Item newItem(id, name, type, rarity, description, textureId, 
                             collision, isBackground, category, properties);
                items[id] = newItem;
            }
            
            std::cout << "Berhasil memuat " << items.size() << " item dari server." << std::endl;
            return true;
        } else {
            std::cerr << "Gagal mendapatkan item dari server. Status: " 
                      << response.status_code() << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saat memuat item: " << e.what() << std::endl;
        return false;
    }
}

Item ItemManager::getItem(int id) const {
    auto it = items.find(id);
    if (it != items.end()) {
        return it->second;
    }
    // Return item kosong jika tidak ditemukan
    return Item();
}

const std::map<int, Item>& ItemManager::getAllItems() const {
    return items;
}

std::vector<Item> ItemManager::getItemsByCategory(const std::string& category) const {
    std::vector<Item> result;
    for (const auto& pair : items) {
        if (pair.second.category == category) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Item> ItemManager::getItemsByType(const std::string& type) const {
    std::vector<Item> result;
    for (const auto& pair : items) {
        if (pair.second.type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// Tambahkan fungsi untuk mendapatkan item default
std::vector<Item> ItemManager::getDefaultItems() const {
    std::vector<Item> defaultItems;
    
    // Tambahkan item punch dan wrench
    auto punchItem = getItem(1); // ID 1 untuk punch
    auto wrenchItem = getItem(2); // ID 2 untuk wrench
    
    if (punchItem.id > 0) defaultItems.push_back(punchItem);
    if (wrenchItem.id > 0) defaultItems.push_back(wrenchItem);
    
    return defaultItems;
}