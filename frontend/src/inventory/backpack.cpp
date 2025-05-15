#include "../../include/inventory/backpack.h"
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

Backpack::Backpack(int slots) : maxSlots(slots) {
    // Inisialisasi backpack dengan item default
    initNewPlayerBackpack();
}

bool Backpack::addItem(int itemId, int count) {
    if (count <= 0) return false;
    
    // Cari apakah item sudah ada di backpack
    for (auto& item : items) {
        if (item.itemId == itemId) {
            item.count += count;
            return true;
        }
    }
    
    // Jika item belum ada, tambahkan sebagai item baru
    if (items.size() < maxSlots) {
        items.push_back(BackpackItem(itemId, count));
        return true;
    }
    
    // Backpack penuh
    return false;
}

bool Backpack::removeItem(int itemId, int count) {
    if (count <= 0) return false;
    
    // Cari item di backpack
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->itemId == itemId) {
            if (it->count > count) {
                it->count -= count;
                return true;
            } else if (it->count == count) {
                items.erase(it);
                return true;
            } else {
                // Tidak cukup item
                return false;
            }
        }
    }
    
    // Item tidak ditemukan
    return false;
}

int Backpack::getItemCount(int itemId) const {
    for (const auto& item : items) {
        if (item.itemId == itemId) {
            return item.count;
        }
    }
    return 0;
}

const std::vector<BackpackItem>& Backpack::getAllItems() const {
    return items;
}

bool Backpack::isFull() const {
    return items.size() >= maxSlots;
}

int Backpack::getAvailableSlots() const {
    return maxSlots - items.size();
}

bool Backpack::saveToServer(int userId) {
    try {
        // Buat HTTP client untuk terhubung ke server
        http_client client(U("http://localhost:5000"));
        
        // Buat JSON untuk data backpack
        json::value jsonBackpack = json::value::object();
        jsonBackpack[U("user_id")] = json::value::number(userId);
        
        // Buat array untuk item
        json::value jsonItems = json::value::array(items.size());
        for (size_t i = 0; i < items.size(); i++) {
            json::value item = json::value::object();
            item[U("item_id")] = json::value::number(items[i].itemId);
            item[U("count")] = json::value::number(items[i].count);
            jsonItems[i] = item;
        }
        jsonBackpack[U("items")] = jsonItems;
        
        // Kirim request POST
        http_request request(methods::POST);
        request.set_request_uri(U("/backpack/save"));
        request.set_body(jsonBackpack);
        
        // Terima response
        http_response response = client.request(request).get();
        
        // Periksa status response
        if (response.status_code() == status_codes::OK) {
            std::cout << "Backpack berhasil disimpan ke server." << std::endl;
            return true;
        } else {
            std::cerr << "Gagal menyimpan backpack ke server. Status: " 
                      << response.status_code() << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saat menyimpan backpack: " << e.what() << std::endl;
        return false;
    }
}

bool Backpack::loadFromServer(int userId) {
    try {
        // Buat HTTP client untuk terhubung ke server
        http_client client(U("http://localhost:5000"));
        
        // Buat request untuk mendapatkan backpack
        uri_builder builder(U("/backpack/load"));
        builder.append_query(U("user_id"), userId);
        
        // Kirim request GET
        http_request request(methods::GET);
        request.set_request_uri(builder.to_uri());
        
        // Terima response
        http_response response = client.request(request).get();
        
        // Periksa status response
        if (response.status_code() == status_codes::OK) {
            // Parse JSON response
            json::value jsonBackpack = response.extract_json().get();
            
            // Kosongkan backpack saat ini
            items.clear();
            
            // Iterasi melalui array item
            json::array jsonItems = jsonBackpack.at(U("items")).as_array();
            for (const auto& item : jsonItems) {
                int itemId = item.at(U("item_id")).as_integer();
                int count = item.at(U("count")).as_integer();
                
                // Tambahkan item ke backpack
                items.push_back(BackpackItem(itemId, count));
            }
            
            std::cout << "Backpack berhasil dimuat dari server." << std::endl;
            return true;
        } else {
            std::cerr << "Gagal memuat backpack dari server. Status: " 
                      << response.status_code() << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saat memuat backpack: " << e.what() << std::endl;
        return false;
    }
}

// Tambahkan di bawah fungsi getAvailableSlots() atau di tempat yang sesuai

// Inisialisasi backpack untuk player baru dengan 13 slot
// Tambahkan di bagian implementasi initNewPlayerBackpack

void Backpack::initNewPlayerBackpack() {
    // Kosongkan backpack terlebih dahulu
    items.clear();
    
    // Set jumlah slot maksimal
    maxSlots = 13; // Sesuai dengan Growtopia asli
    
    // Tambahkan item default: punch (ID 1) dan wrench (ID 50)
    // Berdasarkan database_items.cpp, wrench memiliki ID 50
    items.push_back(BackpackItem(1, 1)); // Punch (ID 1 untuk item custom punch)
    items.push_back(BackpackItem(50, 1)); // Wrench (ID 50 sesuai database)
}

// Tambahkan di bagian bawah file, sebelum akhir file

// Mendapatkan jumlah maksimal slot
int Backpack::getMaxSlots() const {
    return maxSlots;
}