#pragma once
#include <string>
#include <pqxx/pqxx>

namespace db {
    // Fungsi utilitas database dapat ditambahkan di sini
    bool executeQuery(const std::string& query);
    pqxx::result executeQueryWithResult(const std::string& query);
}