#pragma once
#include "domain/Vehicle.h"

class Sedan : public Vehicle {
public:
  using Vehicle::Vehicle;
  QString type() const override { return "Sedan"; }
  double dailyRate() const override { return basePrice; }
};

class SUV : public Vehicle {
public:
  using Vehicle::Vehicle;
  QString type() const override { return "SUV"; }
  double dailyRate() const override { return basePrice * 1.15; }
};

class Truck : public Vehicle {
public:
  using Vehicle::Vehicle;
  QString type() const override { return "Truck"; }
  double dailyRate() const override { return basePrice * 1.25; }
};

class Electric : public Vehicle {
public:
  using Vehicle::Vehicle;
  QString type() const override { return "Electric"; }
  double dailyRate() const override { return basePrice * 1.10; }
};
