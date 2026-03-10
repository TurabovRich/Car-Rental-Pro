#include "ui/tabs/CustomersTab.h"
#include "service/RentalService.h"
#include "ui/dialogs/AddCustomerDialog.h"
#include "ui/dialogs/CustomerHistoryDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QMessageBox>

CustomersTab::CustomersTab(RentalService* service, QWidget* parent) : QWidget(parent), m_service(service) {
  auto* root = new QVBoxLayout(this);
  auto* bar = new QHBoxLayout();
  auto* addBtn = new QPushButton("Add Customer");
  auto* delBtn = new QPushButton("Delete Selected");
  auto* histBtn = new QPushButton("History");
  bar->addWidget(addBtn);
  bar->addWidget(delBtn);
  bar->addWidget(histBtn);
  bar->addStretch(1);
  root->addLayout(bar);

  m_table = new QTableView(this);
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels({"ID","Full Name","License No","Phone"});
  m_table->setModel(m_model);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setAlternatingRowColors(true);
  m_table->setSortingEnabled(true);
  m_table->horizontalHeader()->setStretchLastSection(true);
  root->addWidget(m_table);

  connect(addBtn, &QPushButton::clicked, this, &CustomersTab::onAdd);
  connect(delBtn, &QPushButton::clicked, this, &CustomersTab::onDelete);
  connect(histBtn, &QPushButton::clicked, this, &CustomersTab::onHistory);
  connect(m_table, &QTableView::doubleClicked, this, [this](const QModelIndex&){ onHistory(); });
  refresh();
}

void CustomersTab::refresh() {
  m_model->removeRows(0, m_model->rowCount());
  if (!m_service) return;
  for (const auto& c : m_service->customers()) {
    QList<QStandardItem*> row;
    row << new QStandardItem(QString::number(c.id))
        << new QStandardItem(c.fullName)
        << new QStandardItem(c.licenseNo)
        << new QStandardItem(c.phone);
    m_model->appendRow(row);
  }
  m_table->resizeColumnsToContents();
}

void CustomersTab::onAdd() {
  try {
    AddCustomerDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
      m_service->addCustomer(dlg.buildCustomer());
      refresh();
    }
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void CustomersTab::onDelete() {
  try {
    auto idx = m_table->currentIndex();
    if (!idx.isValid()) return;
    int id = m_model->item(idx.row(), 0)->text().toInt();
    if (QMessageBox::question(this, "Confirm", "Delete customer ID " + QString::number(id) + "?") == QMessageBox::Yes) {
      m_service->deleteCustomer(id);
      refresh();
    }
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void CustomersTab::onHistory() {
  try {
    auto idx = m_table->currentIndex();
    if (!idx.isValid()) return;
    int id = m_model->item(idx.row(), 0)->text().toInt();
    CustomerHistoryDialog dlg(m_service, id, this);
    dlg.exec();
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}
