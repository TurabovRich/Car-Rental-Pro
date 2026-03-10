#include "ui/tabs/ReturnsTab.h"
#include "service/RentalService.h"
#include "ui/dialogs/ReturnDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QMessageBox>

ReturnsTab::ReturnsTab(RentalService* service, QWidget* parent) : QWidget(parent), m_service(service) {
  auto* root = new QVBoxLayout(this);

  auto* bar = new QHBoxLayout();
  auto* btn = new QPushButton("Process Return (selected)", this);
  bar->addWidget(btn);
  bar->addStretch(1);
  root->addLayout(bar);

  m_table = new QTableView(this);
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels({"Reservation ID","Customer","Vehicle","Plate","Start","End","Status"});
  m_table->setModel(m_model);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setAlternatingRowColors(true);
  m_table->setSortingEnabled(true);
  m_table->horizontalHeader()->setStretchLastSection(true);
  root->addWidget(m_table);

  root->addStretch(1);
  connect(btn, &QPushButton::clicked, this, &ReturnsTab::onProcessReturn);
  refresh();
}

void ReturnsTab::refresh() {
  m_model->removeRows(0, m_model->rowCount());
  if (!m_service) return;

  for (const auto& r : m_service->reservations()) {
    if (r.status != "Active") continue;

    QString customerName = QString::number(r.customerId);
    if (auto c = m_service->findCustomer(r.customerId); c.has_value()) customerName = c->fullName;

    QString vehicleName = QString::number(r.vehicleId);
    QString plate = "-";
    if (auto v = m_service->findVehicle(r.vehicleId); v) {
      vehicleName = v->type() + " " + v->brand + " " + v->model;
      plate = v->plate;
    }

    QList<QStandardItem*> row;
    row << new QStandardItem(QString::number(r.id))
        << new QStandardItem(customerName)
        << new QStandardItem(vehicleName)
        << new QStandardItem(plate)
        << new QStandardItem(r.start.toIso())
        << new QStandardItem(r.end.toIso())
        << new QStandardItem(r.status);
    m_model->appendRow(row);
  }
  m_table->resizeColumnsToContents();
}

void ReturnsTab::onProcessReturn() {
  try {
    int preselectId = -1;
    if (auto idx = m_table->currentIndex(); idx.isValid()) {
      preselectId = m_model->item(idx.row(), 0)->text().toInt();
    }

    ReturnDialog dlg(m_service, preselectId, this);
    if (dlg.exec() == QDialog::Accepted) refresh();
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}
