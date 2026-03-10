#pragma once
#include <stdexcept>
#include <string>

class ValidationException : public std::runtime_error {
public:
  explicit ValidationException(const std::string& msg) : std::runtime_error(msg) {}
};

class FileException : public std::runtime_error {
public:
  explicit FileException(const std::string& msg) : std::runtime_error(msg) {}
};

class BookingException : public std::runtime_error {
public:
  explicit BookingException(const std::string& msg) : std::runtime_error(msg) {}
};
