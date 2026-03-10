#pragma once
#include <QString>

class Validation {
public:
  static void requireNonEmpty(const QString& s, const char* fieldName);
  static void requirePositive(double x, const char* fieldName);
  static void requirePhone(const QString& phone);
  static void requireLicense(const QString& lic);
};
