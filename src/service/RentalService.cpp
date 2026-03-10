#include "service/RentalService.h"
#include "storage/FileManager.h"
#include "utils/Exceptions.h"
#include "utils/Validation.h"
#include <algorithm>

int Reservation::nextId = 1;

RentalService::RentalService(FileManager* storage) : m_storage(storage) {}

QString RentalService::dataDir() const {
  if (!m_storage) return {};
  return m_storage->dataDir();
}

void RentalService::load() {
  if (!m_storage) throw FileException("Storage not configured");
  m_storage->loadAll(m_vehicles, m_customers, m_reservations, m_invoices);
  int maxId = 0;
  for (const auto& r : m_reservations) maxId = std::max(maxId, r.id);
  Reservation::nextId = maxId + 1;
}

void RentalService::save() {
  if (!m_storage) throw FileException("Storage not configured");
  m_storage->saveAll(m_vehicles, m_customers, m_reservations, m_invoices);
}

VehiclePtr RentalService::findVehicle(int id) const {
  for (const auto& v : m_vehicles) if (v && v->id == id) return v;
  return nullptr;
}

VehiclePtr RentalService::findVehicle(const QString& plate) const {
  for (const auto& v : m_vehicles) if (v && v->plate.compare(plate, Qt::CaseInsensitive) == 0) return v;
  return nullptr;
}

std::optional<Customer> RentalService::findCustomer(int id) const {
  for (const auto& c : m_customers) if (c.id == id) return c;
  return std::nullopt;
}

void RentalService::addVehicle(const VehiclePtr& v) {
  if (!v) throw ValidationException("Vehicle is null");
  if (findVehicle(v->id)) throw ValidationException("Vehicle ID already exists");
  if (findVehicle(v->plate)) throw ValidationException("Vehicle plate already exists");
  Validation::requireNonEmpty(v->brand, "Brand");
  Validation::requireNonEmpty(v->model, "Model");
  Validation::requireNonEmpty(v->plate, "Plate");
  Validation::requirePositive(v->basePrice, "Base price");
  m_vehicles.push_back(v);
  save();
}

void RentalService::updateVehicle(const VehiclePtr& v) {
  if (!v) throw ValidationException("Vehicle is null");
  // Find existing by ID
  auto it = std::find_if(m_vehicles.begin(), m_vehicles.end(),
                         [&](const VehiclePtr& cur) { return cur && cur->id == v->id; });
  if (it == m_vehicles.end()) throw ValidationException("Vehicle not found");

  // Ensure plate uniqueness (ignore same vehicle)
  for (const auto& cur : m_vehicles) {
    if (!cur) continue;
    if (cur->id == v->id) continue;
    if (cur->plate.compare(v->plate, Qt::CaseInsensitive) == 0)
      throw ValidationException("Vehicle plate already exists");
  }

  *it = v;
  save();
}

void RentalService::deleteVehicle(int id) {
  auto it = std::remove_if(m_vehicles.begin(), m_vehicles.end(), [&](const VehiclePtr& v){ return v && v->id == id; });
  if (it == m_vehicles.end()) throw ValidationException("Vehicle not found");
  m_vehicles.erase(it, m_vehicles.end());
  save();
}

void RentalService::addCustomer(const Customer& c) {
  if (findCustomer(c.id).has_value()) throw ValidationException("Customer ID already exists");
  Validation::requireNonEmpty(c.fullName, "Full name");
  Validation::requireLicense(c.licenseNo);
  Validation::requirePhone(c.phone);
  m_customers.push_back(c);
  save();
}

void RentalService::deleteCustomer(int id) {
  auto it = std::remove_if(m_customers.begin(), m_customers.end(), [&](const Customer& c){ return c.id == id; });
  if (it == m_customers.end()) throw ValidationException("Customer not found");
  m_customers.erase(it, m_customers.end());
  save();
}

bool RentalService::isVehicleAvailableForRange(int vehicleId, const Date& start, const Date& end) const {
  auto v = findVehicle(vehicleId);
  if (!v) return false;

  for (const auto& r : m_reservations) {
    if (r.vehicleId != vehicleId) continue;
    if (r.status != "Active") continue;
    if (start < r.end && r.start < end) return false;
  }
  return true;
}

Reservation RentalService::createReservation(int customerId, int vehicleId, const Date& start, const Date& end) {
  if (!findCustomer(customerId).has_value()) throw BookingException("Customer not found");
  auto v = findVehicle(vehicleId);
  if (!v) throw BookingException("Vehicle not found");
  if (Date::daysBetween(start, end) <= 0) throw BookingException("End date must be after start date");
  if (!isVehicleAvailableForRange(vehicleId, start, end)) throw BookingException("Vehicle is not available for these dates");

  Reservation r(customerId, vehicleId, start, end);
  m_reservations.push_back(r);
  save();
  return r;
}

int RentalService::nextInvoiceId() const {
  int maxId = 0;
  for (const auto& inv : m_invoices) maxId = std::max(maxId, inv.id);
  return maxId + 1;
}

Invoice RentalService::processReturn(int reservationId, int lateDays, double damageFee) {
  if (lateDays < 0) throw ValidationException("Late days cannot be negative");
  if (damageFee < 0) throw ValidationException("Damage fee cannot be negative");

  auto it = std::find_if(m_reservations.begin(), m_reservations.end(), [&](const Reservation& r){ return r.id == reservationId; });
  if (it == m_reservations.end()) throw BookingException("Reservation not found");
  if (it->status != "Active") throw BookingException("Reservation already returned");

  auto v = findVehicle(it->vehicleId);
  if (!v) throw BookingException("Vehicle not found for reservation");

  int days = Date::daysBetween(it->start, it->end);
  double subtotal = days * v->dailyRate();
  double lateFee = lateDays * (v->dailyRate() * 1.5);

  Invoice inv(nextInvoiceId(), reservationId, subtotal, lateFee, damageFee);
  m_invoices.push_back(inv);

  it->status = "Returned";
  save();
  return inv;
}

Invoice RentalService::processReturn(int reservationId, const Date& returnDate, double damageFee) {
  auto it = std::find_if(m_reservations.begin(), m_reservations.end(), [&](const Reservation& r){ return r.id == reservationId; });
  if (it == m_reservations.end()) throw BookingException("Reservation not found");
  if (it->status != "Active") throw BookingException("Reservation already returned");

  int lateDays = 0;
  if (it->end < returnDate) {
    lateDays = Date::daysBetween(it->end, returnDate);
  }
  return processReturn(reservationId, lateDays, damageFee);
}

Invoice RentalService::previewReturn(int reservationId, const Date& returnDate, double damageFee) const {
  if (damageFee < 0) throw ValidationException("Damage fee cannot be negative");

  auto it = std::find_if(m_reservations.begin(), m_reservations.end(), [&](const Reservation& r){ return r.id == reservationId; });
  if (it == m_reservations.end()) throw BookingException("Reservation not found");
  if (it->status != "Active") throw BookingException("Reservation already returned");

  auto v = findVehicle(it->vehicleId);
  if (!v) throw BookingException("Vehicle not found for reservation");

  int lateDays = 0;
  if (it->end < returnDate) {
    lateDays = Date::daysBetween(it->end, returnDate);
  }

  int days = Date::daysBetween(it->start, it->end);
  double subtotal = days * v->dailyRate();
  double lateFee = lateDays * (v->dailyRate() * 1.5);

  return Invoice(0, reservationId, subtotal, lateFee, damageFee);
}

double RentalService::totalRevenue() const {
  double sum = 0.0;
  for (const auto& inv : m_invoices) sum += inv.total;
  return sum;
}
