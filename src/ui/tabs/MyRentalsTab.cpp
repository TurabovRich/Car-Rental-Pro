#include "ui/tabs/MyRentalsTab.h"

#include "service/RentalService.h"
#include "utils/Date.h"

#include <QDate>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>

MyRentalsTab::MyRentalsTab(RentalService* service, int customerId, QWidget* parent)
  : QWidget(parent), m_service(service), m_customerId(customerId) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(10);

  auto* bar = new QHBoxLayout();
  auto* header = new QLabel("My rentals", this);
  header->setStyleSheet("font-size: 16px; font-weight: 700;");
  bar->addWidget(header);
  bar->addStretch(1);
  m_returnBtn = new QPushButton("Return selected (Active)", this);
  bar->addWidget(m_returnBtn);
  root->addLayout(bar);

  m_tabs = new QTabWidget(this);
  root->addWidget(m_tabs, 1);

  m_activeTable = new QTableView(m_tabs);
  m_activeModel = new QStandardItemModel(m_activeTable);
  m_activeModel->setHorizontalHeaderLabels({"Reservation ID","Vehicle","Plate","Start","End","Status"});
  m_activeTable->setModel(m_activeModel);
  m_activeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_activeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_activeTable->setSelectionMode(QAbstractItemView::SingleSelection);
  m_activeTable->setAlternatingRowColors(true);
  m_activeTable->setSortingEnabled(true);
  m_activeTable->horizontalHeader()->setStretchLastSection(true);
  m_tabs->addTab(m_activeTable, "Active");

  m_returnedTable = new QTableView(m_tabs);
  m_returnedModel = new QStandardItemModel(m_returnedTable);
  m_returnedModel->setHorizontalHeaderLabels({"Reservation ID","Vehicle","Plate","Start","End","Status"});
  m_returnedTable->setModel(m_returnedModel);
  m_returnedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_returnedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_returnedTable->setSelectionMode(QAbstractItemView::SingleSelection);
  m_returnedTable->setAlternatingRowColors(true);
  m_returnedTable->setSortingEnabled(true);
  m_returnedTable->horizontalHeader()->setStretchLastSection(true);
  m_tabs->addTab(m_returnedTable, "Returned");

  connect(m_returnBtn, &QPushButton::clicked, this, &MyRentalsTab::onReturnSelected);

  refresh();
}

void MyRentalsTab::refresh() {
  buildTable(m_activeTable, m_activeModel, "Active");
  buildTable(m_returnedTable, m_returnedModel, "Returned");
  m_returnBtn->setEnabled(m_activeModel->rowCount() > 0);
}

void MyRentalsTab::onReturnSelected() {
  try {
    if (!m_service || !m_activeTable) return;
    auto idx = m_activeTable->currentIndex();
    if (!idx.isValid()) {
      QMessageBox::information(this, "Info", "Select an active rental first.");
      return;
    }
    int resId = m_activeModel->item(idx.row(), 0)->text().toInt();
    // User-side simplified return: return "now" with no damage fee.
    // Admin UI has a more detailed return dialog.
    Date returnDate = Date::fromIso(QDate::currentDate().toString(Qt::ISODate));

    auto inv = m_service->processReturn(resId, returnDate, 0.0);
    QMessageBox::information(this, "Returned",
                             "Car returned.\n"
                             "Total: $" + QString::number(inv.total, 'f', 2));
    refresh();
  } catch (const std::exception& e) {
    QMessageBox::warning(this, "Return failed", e.what());
  }
}

void MyRentalsTab::buildTable(QTableView* table, QStandardItemModel* model, const QString& statusFilter) {
  model->removeRows(0, model->rowCount());
  if (!m_service || m_customerId == 0) return;

  for (const auto& r : m_service->reservations()) {
    if (r.customerId != m_customerId) continue;
    if (r.status != statusFilter) continue;

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
    for (auto* it : row) it->setTextAlignment(Qt::AlignCenter);
    model->appendRow(row);
  }
  table->resizeColumnsToContents();
}

