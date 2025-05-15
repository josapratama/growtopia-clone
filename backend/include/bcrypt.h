#ifndef BCRYPT_HPP
#define BCRYPT_HPP

#include <string>

namespace bcrypt {
    std::string generateHash(const std::string& password);
    bool validatePassword(const std::string& password, const std::string& hash);
}

#endif // BCRYPT_HPP