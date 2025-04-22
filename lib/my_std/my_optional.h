#pragma once

#include <exception>
#include <string>
#include <iostream>

struct BadOptionalAccess : public std::exception {    
    const char* what() const noexcept override{
        return "optional doesn't contain value";
    }
};

template<typename T>
class Optional 
{
public:
    Optional() {}

    Optional(const T& value) : value_(value), has_value_(true) {}
    Optional(T&& value) : value_(std::forward<T>(value)), has_value_(true) {}

    Optional(const Optional& other) {
        if (other.has_value_) {
            has_value_ = true;
            value_.real = other.value_.real;
        }
    }

    Optional& operator=(const Optional& other) {
        if (other.has_value_) {
            has_value_ = true;
            value_.real = other.value_.real;
        }
        return *this;
    }

    ~Optional() {
        if (has_value_) {
            (&value_.real)->~T();
        }
    }

    operator bool() const { return has_value_; }

    bool has_value() const {return has_value_; }

    auto& value() {
        if (!has_value_) {
            throw BadOptionalAccess();
        }
        return value_.real;
    }

    auto value_addr() {
        return &value_.real;
    }

private:
    union Storage {
        char litter;
        T real;
        Storage() {litter = 'a';}
        Storage(const T& val) : real(val) {}
        Storage(T&& val) : real(std::forward<T> (val)) {}
        ~Storage() {}
    } value_;

    bool has_value_ = false;
};