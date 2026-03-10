#include "utils/Date.h"
#include "utils/Exceptions.h"
#include <QDate>

Date::Date(int year, int month, int day) : y(year), m(month), d(day) {
  if (!QDate(y, m, d).isValid()) throw ValidationException("Invalid date");
}

Date Date::fromIso(const QString& iso) {
  QDate q = QDate::fromString(iso, Qt::ISODate);
  if (!q.isValid()) throw ValidationException(("Invalid ISO date: " + iso).toStdString());
  return Date(q.year(), q.month(), q.day());
}

QString Date::toIso() const {
  return QDate(y, m, d).toString(Qt::ISODate);
}

int Date::daysBetween(const Date& start, const Date& end) {
  QDate a(start.y, start.m, start.d);
  QDate b(end.y, end.m, end.d);
  int diff = a.daysTo(b);
  if (diff < 0) throw ValidationException("End date must be after start date");
  return diff;
}

bool Date::operator<(const Date& other) const {
  if (y != other.y) return y < other.y;
  if (m != other.m) return m < other.m;
  return d < other.d;
}
bool Date::operator==(const Date& other) const {
  return y == other.y && m == other.m && d == other.d;
}
