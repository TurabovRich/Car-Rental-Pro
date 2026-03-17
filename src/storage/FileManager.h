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

  /**
   * Loads the rental domain CSV files:
   * - cars.csv
   * - customers.csv
   * - reservations.csv
   * - invoices.csv
   */
  void loadAll(std::vector<VehiclePtr>& vehicles,
               std::vector<Customer>& customers,
               std::vector<Reservation>& reservations,
               std::vector<Invoice>& invoices);

  /**
   * Saves the rental domain CSV files. This overwrites the CSV contents.
   * Intended to be called after each state mutation.
   */
  void saveAll(const std::vector<VehiclePtr>& vehicles,
               const std::vector<Customer>& customers,
               const std::vector<Reservation>& reservations,
               const std::vector<Invoice>& invoices);

  /**
   * Loads `data/users.csv` containing local login accounts.
   *
   * Schema: id,username,passwordHashHex,role,customerId
   * - `passwordHashHex` is SHA-256(password) in hex (no raw passwords stored).
   */
  void loadUsers(std::vector<UserAccount>& users);

  /**
   * Saves `data/users.csv`. This overwrites the CSV contents.
   */
  void saveUsers(const std::vector<UserAccount>& users);

private:
  QString m_dataDir;
  QString carsPath() const;
  QString customersPath() const;
  QString reservationsPath() const;
  QString invoicesPath() const;
  QString usersPath() const;
};
