#pragma once
#include <string>
#include <vector>

struct World {
    int id;
    std::string name;
    int ownerId;
    int width;
    int height;
    bool locked;
};

// Membuat dunia baru
bool createWorld(const std::string& worldName, int ownerId, int width = 100, int height = 60);

// Mendapatkan dunia berdasarkan nama
World getWorld(const std::string& worldName);

// Mengubah kepemilikan dunia
bool changeWorldOwner(const std::string& worldName, int newOwnerId);

// Mengunci/membuka dunia
bool setWorldLock(const std::string& worldName, bool locked);