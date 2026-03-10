#pragma once
#include <vector>
#include <optional>
#include <QString>
#include "domain/Vehicles.h"
#include "domain/Customer.h"
#include "domain/Reservation.h"
#include "domain/Invoice.h"

class FileManager;

class RentalService {
public:
  explicit RentalService(FileManager* storage);

  VehiclePtr findVehicle(int id) const;              // overloading
  VehiclePtr findVehicle(const QString& plate) const; // overloading
  std::optional<Customer> findCustomer(int id) const;

  const std::vector<VehiclePtr>& vehicles() const { return m_vehicles; }
  const std::vector<Customer>& customers() const { return m_customers; }
  const std::vector<Reservation>& reservations() const { return m_reservations; }
  const std::vector<Invoice>& invoices() const { return m_invoices; }

  void load();
  void save();
  QString dataDir() const;

  void addVehicle(const VehiclePtr& v);
  void updateVehicle(const VehiclePtr& v);
  void deleteVehicle(int id);

  void addCustomer(const Customer& c);
  void deleteCustomer(int id);

  Reservation createReservation(int customerId, int vehicleId, const Date& start, const Date& end);
  Invoice processReturn(int reservationId, int lateDays, double damageFee);
  Invoice processReturn(int reservationId, const Date& returnDate, double damageFee);
  Invoice previewReturn(int reservationId, const Date& returnDate, double damageFee) const;

  double totalRevenue() const;

private:
  bool isVehicleAvailableForRange(int vehicleId, const Date& start, const Date& end) const;
  int nextInvoiceId() const;

  FileManager* m_storage{nullptr};
  std::vector<VehiclePtr> m_vehicles;
  std::vector<Customer> m_customers;
  std::vector<Reservation> m_reservations;
  std::vector<Invoice> m_invoices;
};
