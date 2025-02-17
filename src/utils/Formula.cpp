//
// Created by Lyndon on 2025/1/26.
//

#include "Formula.h"

int Formula::bla(int arg1) {
    return arg1 * 2;
}

std::string Formula::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string Formula::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}