//
// Created by Lyndon on 2025/1/20.
//

#ifndef EGONEXCEPTION_H
#define EGONEXCEPTION_H

#include <exception>
#include <string>


class EgonException : public std::exception {
public:
    explicit EgonException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};



#endif //EGONEXCEPTION_H
