#include "../include/database_utils.h"
#include "../include/database_config.h"
#include <iostream>
#include <pqxx/pqxx>

namespace db {
    // Fungsi utilitas database dapat diimplementasikan di sini
    // Misalnya fungsi untuk query umum, koneksi, dll.
    
    bool executeQuery(const std::string& query) {
        try {
            pqxx::connection c(CONNECTION_STR);
            pqxx::work txn(c);
            txn.exec(query);
            txn.commit();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[Database Error] " << e.what() << std::endl;
            return false;
        }
    }
    
    pqxx::result executeQueryWithResult(const std::string& query) {
        try {
            pqxx::connection c(CONNECTION_STR);
            pqxx::work txn(c);
            pqxx::result r = txn.exec(query);
            txn.commit();
            return r;
        } catch (const std::exception& e) {
            std::cerr << "[Database Error] " << e.what() << std::endl;
            return pqxx::result();
        }
    }
}