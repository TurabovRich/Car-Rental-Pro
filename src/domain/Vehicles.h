#pragma once
#include "domain/Vehicle.h"

// Extra interface for types that include insurance pricing.
class Insurable {
public:
  virtual ~Insurable() = default;
  virtual double insuranceMultiplier() const = 0;
};

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

// Inherits SUV pricing and adds an insurance multiplier on top.
class PremiumSUV : public SUV, public Insurable {
public:
  using SUV::SUV;
  QString type() const override { return "PremiumSUV"; }
  double insuranceMultiplier() const override { return 1.20; }
  double dailyRate() const override { return SUV::dailyRate() * insuranceMultiplier(); }
};
