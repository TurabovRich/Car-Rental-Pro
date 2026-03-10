#pragma once
#include <QString>
#include <memory>
#include <ostream>

class Vehicle {
public:
  int id{0};
  QString brand;
  QString model;
  int year{0};
  QString plate;
  double basePrice{0.0};
  bool available{true};
  QString imagePath; // relative to data dir (e.g. images/foo.png) or absolute

  Vehicle() = default;
  Vehicle(int id_, QString brand_, QString model_, int year_, QString plate_, double basePrice_, bool available_)
    : id(id_), brand(std::move(brand_)), model(std::move(model_)), year(year_), plate(std::move(plate_)),
      basePrice(basePrice_), available(available_) {}
  virtual ~Vehicle() = default;

  virtual QString type() const = 0;
  virtual double dailyRate() const = 0;

  friend std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    os << v.id << " " << v.type().toStdString() << " " << v.brand.toStdString()
       << " " << v.model.toStdString() << " " << v.year << " " << v.plate.toStdString()
       << " $" << v.basePrice << " avail=" << (v.available ? "true" : "false");
    return os;
  }
};
using VehiclePtr = std::shared_ptr<Vehicle>;
