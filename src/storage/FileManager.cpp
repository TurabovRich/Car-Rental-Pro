// all csv read/write lives here
#include "storage/FileManager.h"
#include "domain/Vehicles.h"
#include "utils/Exceptions.h"
#include "utils/Date.h"
#include <QFile>
#include <QTextStream>

static QStringList splitCsvLine(const QString& line) {
  return line.split(",", Qt::KeepEmptyParts);
}

// csv has type as string, we pick the right subclass
static VehiclePtr makeVehicle(const QString& type, int id, const QString& brand, const QString& model,
                              int year, const QString& plate, double basePrice, bool avail) {
  if (type == "SUV") return std::make_shared<SUV>(id, brand, model, year, plate, basePrice, avail);
  if (type == "Truck") return std::make_shared<Truck>(id, brand, model, year, plate, basePrice, avail);
  if (type == "Electric") return std::make_shared<Electric>(id, brand, model, year, plate, basePrice, avail);
  if (type == "PremiumSUV") return std::make_shared<PremiumSUV>(id, brand, model, year, plate, basePrice, avail);
  return std::make_shared<Sedan>(id, brand, model, year, plate, basePrice, avail);
}

FileManager::FileManager(QString dataDir) : m_dataDir(std::move(dataDir)) {}

QString FileManager::carsPath() const { return m_dataDir + "/cars.csv"; }
QString FileManager::customersPath() const { return m_dataDir + "/customers.csv"; }
QString FileManager::reservationsPath() const { return m_dataDir + "/reservations.csv"; }
QString FileManager::invoicesPath() const { return m_dataDir + "/invoices.csv"; }
QString FileManager::usersPath() const { return m_dataDir + "/users.csv"; }

void FileManager::loadAll(std::vector<VehiclePtr>& vehicles,
                          std::vector<Customer>& customers,
                          std::vector<Reservation>& reservations,
                          std::vector<Invoice>& invoices) {
  vehicles.clear(); customers.clear(); reservations.clear(); invoices.clear();

  {
    QFile f(carsPath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      throw FileException("Cannot open " + carsPath().toStdString());
    QTextStream in(&f);
    bool first = true;
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty()) continue;
      if (first) { first = false; continue; } // skip header line
      auto cols = splitCsvLine(line);
      if (cols.size() < 8) continue;

      int id = cols[0].toInt();
      QString type = cols[1];
      QString brand = cols[2];
      QString model = cols[3];
      int year = cols[4].toInt();
      QString plate = cols[5];
      double basePrice = cols[6].toDouble();
      bool avail = (cols[7].toLower() == "true");

      VehiclePtr v = makeVehicle(type, id, brand, model, year, plate, basePrice, avail);
      if (cols.size() >= 9) v->imagePath = cols[8];
      vehicles.push_back(v);
    }
  }

  {
    QFile f(customersPath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      throw FileException("Cannot open " + customersPath().toStdString());
    QTextStream in(&f);
    bool first = true;
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty()) continue;
      if (first) { first = false; continue; }
      auto cols = splitCsvLine(line);
      if (cols.size() < 4) continue;
      customers.emplace_back(cols[0].toInt(), cols[1], cols[2], cols[3]);
    }
  }

  {
    QFile f(reservationsPath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      throw FileException("Cannot open " + reservationsPath().toStdString());
    QTextStream in(&f);
    bool first = true;
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty()) continue;
      if (first) { first = false; continue; }
      auto cols = splitCsvLine(line);
      if (cols.size() < 6) continue;
      Reservation r;
      r.id = cols[0].toInt();
      r.customerId = cols[1].toInt();
      r.vehicleId = cols[2].toInt();
      r.start = Date::fromIso(cols[3]);
      r.end = Date::fromIso(cols[4]);
      r.status = cols[5];
      reservations.push_back(r);
    }
  }

  {
    QFile f(invoicesPath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      throw FileException("Cannot open " + invoicesPath().toStdString());
    QTextStream in(&f);
    bool first = true;
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty()) continue;
      if (first) { first = false; continue; }
      auto cols = splitCsvLine(line);
      if (cols.size() < 7) continue;
      Invoice inv;
      inv.id = cols[0].toInt();
      inv.reservationId = cols[1].toInt();
      inv.subtotal = cols[2].toDouble();
      inv.vat = cols[3].toDouble();
      inv.lateFee = cols[4].toDouble();
      inv.damageFee = cols[5].toDouble();
      inv.total = cols[6].toDouble();
      invoices.push_back(inv);
    }
  }
}

void FileManager::saveAll(const std::vector<VehiclePtr>& vehicles,
                          const std::vector<Customer>& customers,
                          const std::vector<Reservation>& reservations,
                          const std::vector<Invoice>& invoices) {
  {
    QFile f(carsPath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
      throw FileException("Cannot write " + carsPath().toStdString());
    QTextStream out(&f);
    out << "id,type,brand,model,year,plate,basePrice,available,imagePath\n";
    for (const auto& v : vehicles) {
      if (!v) continue;
      out << v->id << "," << v->type() << "," << v->brand << "," << v->model << ","
          << v->year << "," << v->plate << "," << v->basePrice << ","
          << (v->available ? "true" : "false") << "," << v->imagePath << "\n";
    }
  }

  {
    QFile f(customersPath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
      throw FileException("Cannot write " + customersPath().toStdString());
    QTextStream out(&f);
    out << "id,fullName,licenseNo,phone\n";
    for (const auto& c : customers) {
      out << c.id << "," << c.fullName << "," << c.licenseNo << "," << c.phone << "\n";
    }
  }

  {
    QFile f(reservationsPath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
      throw FileException("Cannot write " + reservationsPath().toStdString());
    QTextStream out(&f);
    out << "id,customerId,vehicleId,startDate,endDate,status\n";
    for (const auto& r : reservations) {
      out << r.id << "," << r.customerId << "," << r.vehicleId << ","
          << r.start.toIso() << "," << r.end.toIso() << "," << r.status << "\n";
    }
  }

  {
    QFile f(invoicesPath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
      throw FileException("Cannot write " + invoicesPath().toStdString());
    QTextStream out(&f);
    out << "id,reservationId,subtotal,vat,lateFee,damageFee,total\n";
    for (const auto& inv : invoices) {
      out << inv.id << "," << inv.reservationId << "," << inv.subtotal << ","
          << inv.vat << "," << inv.lateFee << "," << inv.damageFee << "," << inv.total << "\n";
    }
  }
}

void FileManager::loadUsers(std::vector<UserAccount>& users) {
  users.clear();

  QFile f(usersPath());
  if (!f.exists()) return;
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    throw FileException("Cannot open " + usersPath().toStdString());

  QTextStream in(&f);
  bool first = true;
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    if (line.isEmpty()) continue;
    if (first) { first = false; continue; }
    auto cols = splitCsvLine(line);
    if (cols.size() < 4) continue;

    UserAccount u;
    u.id = cols[0].toInt();
    u.username = cols[1];
    u.passwordHashHex = cols[2];
    u.role = cols[3];
    u.customerId = (cols.size() >= 5) ? cols[4].toInt() : 0;
    users.push_back(u);
  }
}

void FileManager::saveUsers(const std::vector<UserAccount>& users) {
  QFile f(usersPath());
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    throw FileException("Cannot write " + usersPath().toStdString());
  QTextStream out(&f);
  out << "id,username,passwordHashHex,role,customerId\n";
  for (const auto& u : users) {
    out << u.id << "," << u.username << "," << u.passwordHashHex << ","
        << u.role << "," << u.customerId << "\n";
  }
}
