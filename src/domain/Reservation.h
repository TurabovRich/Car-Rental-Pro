#pragma once
#include <QString>
#include "utils/Date.h"

class Reservation {
public:
  static int nextId;

  int id{0};
  int customerId{0};
  int vehicleId{0};
  Date start;
  Date end;
  QString status{"Active"}; // Active / Returned

  Reservation() = default;
  Reservation(int customerId_, int vehicleId_, Date start_, Date end_)
    : id(nextId++), customerId(customerId_), vehicleId(vehicleId_), start(start_), end(end_) {}
};
