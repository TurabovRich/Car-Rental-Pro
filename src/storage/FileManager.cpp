#include "storage/FileManager.h"
#include "domain/Vehicles.h"
#include "utils/Exceptions.h"
#include "utils/Date.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

static QStringList splitCsvLine(const QString& line) {
  return line.split(",", Qt::KeepEmptyParts);
}

FileManager::FileManager(QString dataDir) : m_dataDir(std::move(dataDir)) {}

QString FileManager::carsPath() const { return m_dataDir + "/cars.csv"; }
QString FileManager::customersPath() const { return m_dataDir + "/customers.csv"; }
QString FileManager::reservationsPath() const { return m_dataDir + "/reservations.csv"; }
QString FileManager::invoicesPath() const { return m_dataDir + "/invoices.csv"; }

void FileManager::loadAll(std::vector<VehiclePtr>& vehicles,
                          std::vector<Customer>& customers,
                          std::vector<Reservation>& reservations,
                          std::vector<Invoice>& invoices) {
  vehicles.clear(); customers.clear(); reservations.clear(); invoices.clear();

  // Cars
  {
    QFile f(carsPath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      throw FileException("Cannot open " + carsPath().toStdString());
    QTextStream in(&f);

    // Read header to detect schema (supports both legacy and image-first formats)
    QString headerLine;
    if (in.atEnd()) return;
    headerLine = in.readLine().trimmed();
    auto headerCols = splitCsvLine(headerLine);
    bool imageFirst = !headerCols.isEmpty() &&
                      headerCols[0].trimmed().compare("image", Qt::CaseInsensitive) == 0;

    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty()) continue;
      auto cols = splitCsvLine(line);

      // Trim every column to be robust to spaces in CSV
      for (auto& c : cols) c = c.trimmed();

      VehiclePtr v;

      if (imageFirst) {
        // Format: image,id,type,brand,model,year,plate,basePrice,available
        if (cols.size() < 9) continue;

        QString imageFile = cols[0];
        int id = cols[1].toInt();
        QString type = cols[2];
        QString brand = cols[3];
        QString model = cols[4];
        int year = cols[5].toInt();
        QString plate = cols[6];
        double basePrice = cols[7].toDouble();
        bool avail = (cols[8].toLower() == "true");

        if (type == "Sedan") v = std::make_shared<Sedan>(id, brand, model, year, plate, basePrice, avail);
        else if (type == "SUV") v = std::make_shared<SUV>(id, brand, model, year, plate, basePrice, avail);
        else if (type == "Truck") v = std::make_shared<Truck>(id, brand, model, year, plate, basePrice, avail);
        else if (type == "Electric") v = std::make_shared<Electric>(id, brand, model, year, plate, basePrice, avail);
        else v = std::make_shared<Sedan>(id, brand, model, year, plate, basePrice, avail);

        if (v) {
          if (!imageFile.isEmpty()) {
            // If user only provided filename, assume it's inside "images" folder under data dir
            if (imageFile.contains('/'))
              v->imagePath = imageFile;
            else
              v->imagePath = "images/" + imageFile;
          }
        }
      } else {
        // Legacy format: id,type,brand,model,year,plate,basePrice,available[,imagePath]
        if (cols.size() < 8) continue;

        int id = cols[0].toInt();
        QString type = cols[1];
        QString brand = cols[2];
        QString model = cols[3];
        int year = cols[4].toInt();
        QString plate = cols[5];
        double basePrice = cols[6].toDouble();
        bool avail = (cols[7].toLower() == "true");
        QString imagePath = (cols.size() >= 9) ? cols[8] : QString();

        if (type == "Sedan") v = std::make_shared<Sedan>(id, brand, model, year, plate, basePrice, avail);
        else if (type == "SUV") v = std::make_shared<SUV>(id, brand, model, year, plate, basePrice, avail);
        else if (type == "Truck") v = std::make_shared<Truck>(id, brand, model, year, plate, basePrice, avail);
        else if (type == "Electric") v = std::make_shared<Electric>(id, brand, model, year, plate, basePrice, avail);
        else v = std::make_shared<Sedan>(id, brand, model, year, plate, basePrice, avail);

        if (v) v->imagePath = imagePath;
      }

      // Auto-link image by plate if not provided in CSV
      if (v && v->imagePath.trimmed().isEmpty()) {
        QDir imagesDir(m_dataDir + "/images");
        if (imagesDir.exists()) {
          const QString base = v->plate.trimmed();
          if (!base.isEmpty()) {
            const QStringList filters = {
              base + ".png", base + ".jpg", base + ".jpeg", base + ".webp", base + ".bmp"
            };
            const QStringList matches = imagesDir.entryList(filters, QDir::Files);
            if (!matches.isEmpty()) {
              v->imagePath = "images/" + matches.first();
            }
          }
        }
      }

      if (v) vehicles.push_back(v);
    }
  }

  // Customers
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

  // Reservations
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

  // Invoices
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
  // Cars
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

  // Customers
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

  // Reservations
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

  // Invoices
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
