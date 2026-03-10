#pragma once
#include <QString>
#include <ostream>

class Customer {
public:
  int id{0};
  QString fullName;
  QString licenseNo;
  QString phone;

  Customer() = default;
  Customer(int id_, QString fullName_, QString licenseNo_, QString phone_)
    : id(id_), fullName(std::move(fullName_)), licenseNo(std::move(licenseNo_)), phone(std::move(phone_)) {}

  bool operator==(const Customer& other) const { return id == other.id; }
  friend std::ostream& operator<<(std::ostream& os, const Customer& c) {
    os << c.id << " " << c.fullName.toStdString() << " " << c.licenseNo.toStdString()
       << " " << c.phone.toStdString();
    return os;
  }
};
