#include "ui/dialogs/CustomerHistoryDialog.h"
#include "service/RentalService.h"
#include "domain/Customer.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QDialogButtonBox>
#include <QLabel>

CustomerHistoryDialog::CustomerHistoryDialog(RentalService* service, int customerId, QWidget* parent)
  : QDialog(parent), m_service(service), m_customerId(customerId) {
  setWindowTitle("Customer History");
  resize(900, 600);

  auto* root = new QVBoxLayout(this);
  m_header = new QLabel(this);
  m_header->setWordWrap(true);
  m_header->setStyleSheet("QLabel { font-size: 16px; font-weight: 600; }");
  root->addWidget(m_header);

  m_summary = new QLabel(this);
  m_summary->setWordWrap(true);
  root->addWidget(m_summary);

  m_tabs = new QTabWidget(this);
  root->addWidget(m_tabs, 1);

  m_resTable = new QTableView(this);
  m_resModel = new QStandardItemModel(this);
  m_resModel->setHorizontalHeaderLabels({"Reservation ID","Vehicle","Plate","Start","End","Status"});
  m_resTable->setModel(m_resModel);
  m_resTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_resTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_resTable->setAlternatingRowColors(true);
  m_resTable->setSortingEnabled(true);
  m_resTable->horizontalHeader()->setStretchLastSection(true);
  m_tabs->addTab(m_resTable, "Reservations");

  m_invTable = new QTableView(this);
  m_invModel = new QStandardItemModel(this);
  m_invModel->setHorizontalHeaderLabels({"Invoice ID","Reservation ID","Subtotal","VAT","Late Fee","Damage Fee","Total"});
  m_invTable->setModel(m_invModel);
  m_invTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_invTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_invTable->setAlternatingRowColors(true);
  m_invTable->setSortingEnabled(true);
  m_invTable->horizontalHeader()->setStretchLastSection(true);
  m_tabs->addTab(m_invTable, "Invoices");

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  root->addWidget(buttons);

  buildReservations();
  buildInvoices();
  updateSummary();
}

void CustomerHistoryDialog::buildReservations() {
  m_resModel->removeRows(0, m_resModel->rowCount());
  if (!m_service) return;

  for (const auto& r : m_service->reservations()) {
    if (r.customerId != m_customerId) continue;
    QString vehicleName = QString::number(r.vehicleId);
    QString plate = "-";
    if (auto v = m_service->findVehicle(r.vehicleId); v) {
      vehicleName = v->type() + " " + v->brand + " " + v->model;
      plate = v->plate;
    }
    QList<QStandardItem*> row;
    row << new QStandardItem(QString::number(r.id))
        << new QStandardItem(vehicleName)
        << new QStandardItem(plate)
        << new QStandardItem(r.start.toIso())
        << new QStandardItem(r.end.toIso())
        << new QStandardItem(r.status);
    m_resModel->appendRow(row);
  }
  m_resTable->resizeColumnsToContents();
}

void CustomerHistoryDialog::buildInvoices() {
  m_invModel->removeRows(0, m_invModel->rowCount());
  if (!m_service) return;

  auto belongsToCustomer = [&](int reservationId) -> bool {
    for (const auto& r : m_service->reservations()) {
      if (r.id == reservationId) return r.customerId == m_customerId;
    }
    return false;
  };

  for (const auto& inv : m_service->invoices()) {
    if (!belongsToCustomer(inv.reservationId)) continue;
    QList<QStandardItem*> row;
    row << new QStandardItem(QString::number(inv.id))
        << new QStandardItem(QString::number(inv.reservationId))
        << new QStandardItem(QString::number(inv.subtotal, 'f', 2))
        << new QStandardItem(QString::number(inv.vat, 'f', 2))
        << new QStandardItem(QString::number(inv.lateFee, 'f', 2))
        << new QStandardItem(QString::number(inv.damageFee, 'f', 2))
        << new QStandardItem(QString::number(inv.total, 'f', 2));
    m_invModel->appendRow(row);
  }
  m_invTable->resizeColumnsToContents();
}

void CustomerHistoryDialog::updateSummary() {
  if (!m_service) return;

  QString name = QString::number(m_customerId);
  QString license;
  QString phone;
  if (auto c = m_service->findCustomer(m_customerId); c.has_value()) {
    name = c->fullName;
    license = c->licenseNo;
    phone = c->phone;
  }
  m_header->setText(name + " (ID " + QString::number(m_customerId) + ")");

  int reservations = 0;
  int active = 0;
  for (const auto& r : m_service->reservations()) {
    if (r.customerId != m_customerId) continue;
    reservations++;
    if (r.status == "Active") active++;
  }

  int invoices = 0;
  double spent = 0.0;
  for (const auto& inv : m_service->invoices()) {
    for (const auto& r : m_service->reservations()) {
      if (r.id != inv.reservationId) continue;
      if (r.customerId != m_customerId) break;
      invoices++;
      spent += inv.total;
      break;
    }
  }

  QString line = "Reservations: " + QString::number(reservations) +
                 " (Active: " + QString::number(active) + ")\n" +
                 "Invoices: " + QString::number(invoices) +
                 " | Total spent: $" + QString::number(spent, 'f', 2);
  if (!license.isEmpty()) line += "\nLicense: " + license;
  if (!phone.isEmpty()) line += " | Phone: " + phone;
  m_summary->setText(line);
}

