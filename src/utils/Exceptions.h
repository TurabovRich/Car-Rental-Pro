#pragma once
#include <stdexcept>
#include <string>

// Thrown when user input or business rules fail validation.
class ValidationException : public std::runtime_error {
public:
  explicit ValidationException(const std::string& msg) : std::runtime_error(msg) {}
};

// Thrown when a CSV file cannot be opened or parsed.
class FileException : public std::runtime_error {
public:
  explicit FileException(const std::string& msg) : std::runtime_error(msg) {}
};

// Thrown when a reservation or return cannot be completed.
class BookingException : public std::runtime_error {
public:
  explicit BookingException(const std::string& msg) : std::runtime_error(msg) {}
};
