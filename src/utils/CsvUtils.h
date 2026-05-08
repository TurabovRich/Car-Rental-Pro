#pragma once

#include <QString>
#include <QStringList>

namespace CsvUtils {

// Compile-time polymorphism via templates:
// one generic API, specialized behavior per type.
template <typename T>
QString toCell(const T& value) {
  return QString::number(value);
}

template <>
inline QString toCell<QString>(const QString& value) {
  return value;
}

template <>
inline QString toCell<const char*>(const char* const& value) {
  return QString::fromUtf8(value);
}

template <typename... Args>
QString makeRow(const Args&... args) {
  QStringList cols{toCell(args)...};
  return cols.join(",") + "\n";
}

} // namespace CsvUtils

