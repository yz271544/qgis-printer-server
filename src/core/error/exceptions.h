//
// Created by Lyndon on 2025/1/20.
//

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>


class ApplicationError : public std::exception {
public:
    explicit ApplicationError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class InvalidOperationException : public std::exception {
public:
    explicit InvalidOperationException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class EgonException : public std::exception {
public:
    explicit EgonException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class UnknownFieldType : public std::exception {
public:
    explicit UnknownFieldType(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class UnknownArgNum : public std::exception {
public:
    explicit UnknownArgNum(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class TypeMappingError : public std::exception {
public:
    explicit TypeMappingError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class TableNotExistsError : public std::exception {
public:
    explicit TableNotExistsError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class FileOperationError : public std::exception {
public:
    explicit FileOperationError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class GeometryCheckError : public std::exception {
public:
    explicit GeometryCheckError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class XServerRequestError : public std::exception {
public:
    explicit XServerRequestError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};

class SystemUnknownError : public std::exception {
public:
    explicit SystemUnknownError(const std::string& message) : message_(message) {}
    const char* what() const noexcept override {
        return message_.c_str();
    }
private:
    std::string message_;
};


#endif //EXCEPTIONS_H
