#pragma once

#include <vector>
#include <map>
#include <string>
#include "item.h"

// Struktur untuk merepresentasikan item dalam backpack
struct BackpackItem {
    int itemId;
    int count;
    
    BackpackItem(int id = 0, int cnt = 0) : itemId(id), count(cnt) {}
};

class Backpack {
private:
    std::vector<BackpackItem> items;
    int maxSlots;
    
public:
    // Konstruktor
    Backpack(int slots = 20);
    
    // Menambahkan item ke backpack
    bool addItem(int itemId, int count = 1);
    
    // Menghapus item dari backpack
    bool removeItem(int itemId, int count = 1);
    
    // Inisialisasi backpack untuk player baru
    void initNewPlayerBackpack();
    
    // Mendapatkan jumlah item tertentu
    int getItemCount(int itemId) const;
    
    // Mendapatkan semua item dalam backpack
    const std::vector<BackpackItem>& getAllItems() const;
    
    // Memeriksa apakah backpack penuh
    bool isFull() const;
    
    // Mendapatkan jumlah slot yang tersedia
    int getAvailableSlots() const;
    
    // Menyimpan backpack ke server
    bool saveToServer(int userId);
    
    // Memuat backpack dari server
    bool loadFromServer(int userId);
    
    // Mendapatkan jumlah maksimal slot
    int getMaxSlots() const;
};