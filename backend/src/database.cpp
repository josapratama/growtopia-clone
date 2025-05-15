#include "../include/database.h"
#include <iostream>

int database_main() {
    // Inisialisasi database
    if (db::initDatabase()) {
        std::cout << "Database berhasil diinisialisasi\n";
    } else {
        std::cout << "Gagal menginisialisasi database\n";
    }
    
    return 0;
}