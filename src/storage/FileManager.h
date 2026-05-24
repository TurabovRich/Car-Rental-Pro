#pragma once
#include <QString>
#include <vector>
#include "domain/Vehicle.h"
#include "domain/Customer.h"
#include "domain/Reservation.h"
#include "domain/Invoice.h"
#include "domain/UserAccount.h"

class FileManager {
public:
  explicit FileManager(QString dataDir);
  QString dataDir() const { return m_dataDir; }

  void loadAll(std::vector<VehiclePtr>& vehicles,
               std::vector<Customer>& customers,
               std::vector<Reservation>& reservations,
               std::vector<Invoice>& invoices);

  void saveAll(const std::vector<VehiclePtr>& vehicles,
               const std::vector<Customer>& customers,
               const std::vector<Reservation>& reservations,
               const std::vector<Invoice>& invoices);

  void loadUsers(std::vector<UserAccount>& users);
  void saveUsers(const std::vector<UserAccount>& users);

private:
  QString m_dataDir;
  QString carsPath() const;
  QString customersPath() const;
  QString reservationsPath() const;
  QString invoicesPath() const;
  QString usersPath() const;
};
