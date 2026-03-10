#pragma once
#include <QString>

class Date {
public:
  int y{1970}, m{1}, d{1};

  Date() = default;
  Date(int year, int month, int day);

  static Date fromIso(const QString& iso); // YYYY-MM-DD
  QString toIso() const;

  static int daysBetween(const Date& start, const Date& end);

  bool operator<(const Date& other) const;
  bool operator==(const Date& other) const;
};
