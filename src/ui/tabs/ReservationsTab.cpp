#include "ui/tabs/ReservationsTab.h"
#include "service/RentalService.h"
#include "ui/dialogs/NewReservationDialog.h"
#include "ui/dialogs/ReturnDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QMessageBox>

ReservationsTab::ReservationsTab(RentalService* service, QWidget* parent) : QWidget(parent), m_service(service) {
  auto* root = new QVBoxLayout(this);
  auto* bar = new QHBoxLayout();
  auto* newBtn = new QPushButton("New Reservation");
  auto* returnBtn = new QPushButton("Process Return (selected)");
  bar->addWidget(newBtn);
  bar->addWidget(returnBtn);
  bar->addStretch(1);
  root->addLayout(bar);

  m_table = new QTableView(this);
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels({"ID","Customer","Vehicle","Plate","Start","End","Status"});
  m_table->setModel(m_model);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setAlternatingRowColors(true);
  m_table->setSortingEnabled(true);
  m_table->horizontalHeader()->setStretchLastSection(true);
  root->addWidget(m_table);

  connect(newBtn, &QPushButton::clicked, this, &ReservationsTab::onNewReservation);
  connect(returnBtn, &QPushButton::clicked, this, &ReservationsTab::onProcessReturn);
  refresh();
}

void ReservationsTab::refresh() {
  m_model->removeRows(0, m_model->rowCount());
  if (!m_service) return;
  for (const auto& r : m_service->reservations()) {
    QString customerName = QString::number(r.customerId);
    if (auto c = m_service->findCustomer(r.customerId); c.has_value()) customerName = c->fullName;

    QString vehicleName = QString::number(r.vehicleId);
    QString plate = "-";
    if (auto v = m_service->findVehicle(r.vehicleId); v) {
      vehicleName = v->type() + " " + v->brand + " " + v->model;
      plate = v->plate;
    }

    auto* idItem = new QStandardItem(QString::number(r.id));
    auto* custItem = new QStandardItem(customerName);
    auto* vehItem = new QStandardItem(vehicleName);
    auto* plateItem = new QStandardItem(plate);
    auto* startItem = new QStandardItem(r.start.toIso());
    auto* endItem = new QStandardItem(r.end.toIso());
    auto* statusItem = new QStandardItem(r.status);

    idItem->setTextAlignment(Qt::AlignCenter);
    custItem->setTextAlignment(Qt::AlignCenter);
    vehItem->setTextAlignment(Qt::AlignCenter);
    plateItem->setTextAlignment(Qt::AlignCenter);
    startItem->setTextAlignment(Qt::AlignCenter);
    endItem->setTextAlignment(Qt::AlignCenter);
    statusItem->setTextAlignment(Qt::AlignCenter);

    QList<QStandardItem*> row;
    row << idItem << custItem << vehItem << plateItem << startItem << endItem << statusItem;
    m_model->appendRow(row);
  }
  m_table->resizeColumnsToContents();
}

void ReservationsTab::onNewReservation() {
  try {
    NewReservationDialog dlg(m_service, this);
    if (dlg.exec() == QDialog::Accepted) {
      dlg.createReservation();
      refresh();
    }
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void ReservationsTab::onProcessReturn() {
  try {
    auto idx = m_table->currentIndex();
    if (!idx.isValid()) return;

    int resId = m_model->item(idx.row(), 0)->text().toInt();
    QString status = m_model->item(idx.row(), 6)->text();
    if (status != "Active") {
      QMessageBox::information(this, "Info", "Selected reservation is already returned.");
      return;
    }

    ReturnDialog dlg(m_service, resId, this);
    if (dlg.exec() == QDialog::Accepted) refresh();
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}
