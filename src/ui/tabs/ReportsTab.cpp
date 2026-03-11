#include "ui/tabs/ReportsTab.h"
#include "service/RentalService.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

QLabel* ReportsTab::makeCard(const QString& title, const QString& value) {
  auto* card = new QLabel(this);
  card->setWordWrap(true);
  card->setText(title + "\n" + value);
  card->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  card->setMinimumHeight(70);
  card->setStyleSheet(
    "QLabel {"
    "  padding: 14px;"
    "  border: 1px solid #e6e6e6;"
    "  border-radius: 12px;"
    "  background: #ffffff;"
    "  font-size: 13px;"
    "}"
  );
  return card;
}

ReportsTab::ReportsTab(RentalService* service, QWidget* parent) : QWidget(parent), m_service(service) {
  auto* root = new QVBoxLayout(this);
  auto* kpis = new QGridLayout();
  m_revenueLabel = makeCard("Total revenue", "$0.00");
  m_kpiInvoices = makeCard("Invoices", "0");
  m_kpiActive = makeCard("Active reservations", "0");
  m_kpiCars = makeCard("Cars", "0");
  kpis->addWidget(m_revenueLabel, 0, 0);
  kpis->addWidget(m_kpiInvoices, 0, 1);
  kpis->addWidget(m_kpiActive, 0, 2);
  kpis->addWidget(m_kpiCars, 0, 3);
  root->addLayout(kpis);

  m_table = new QTableView(this);
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels({"Invoice ID","Reservation ID","Subtotal","VAT","Late Fee","Damage Fee","Total"});
  m_table->setModel(m_model);
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setAlternatingRowColors(true);
  m_table->setSortingEnabled(true);
  m_table->horizontalHeader()->setStretchLastSection(true);
  root->addWidget(m_table);

  refresh();
}

void ReportsTab::refresh() {
  m_model->removeRows(0, m_model->rowCount());
  if (!m_service) return;

  m_revenueLabel->setText("Total revenue\n$" + QString::number(m_service->totalRevenue(), 'f', 2));
  m_kpiInvoices->setText("Invoices\n" + QString::number((int)m_service->invoices().size()));
  int active = 0;
  for (const auto& r : m_service->reservations()) if (r.status == "Active") active++;
  m_kpiActive->setText("Active reservations\n" + QString::number(active));
  m_kpiCars->setText("Cars\n" + QString::number((int)m_service->vehicles().size()));

  for (const auto& inv : m_service->invoices()) {
    auto* idItem = new QStandardItem(QString::number(inv.id));
    auto* resItem = new QStandardItem(QString::number(inv.reservationId));
    auto* subItem = new QStandardItem(QString::number(inv.subtotal, 'f', 2));
    auto* vatItem = new QStandardItem(QString::number(inv.vat, 'f', 2));
    auto* lateItem = new QStandardItem(QString::number(inv.lateFee, 'f', 2));
    auto* dmgItem = new QStandardItem(QString::number(inv.damageFee, 'f', 2));
    auto* totalItem = new QStandardItem(QString::number(inv.total, 'f', 2));

    idItem->setTextAlignment(Qt::AlignCenter);
    resItem->setTextAlignment(Qt::AlignCenter);
    subItem->setTextAlignment(Qt::AlignCenter);
    vatItem->setTextAlignment(Qt::AlignCenter);
    lateItem->setTextAlignment(Qt::AlignCenter);
    dmgItem->setTextAlignment(Qt::AlignCenter);
    totalItem->setTextAlignment(Qt::AlignCenter);

    QList<QStandardItem*> row;
    row << idItem << resItem << subItem << vatItem << lateItem << dmgItem << totalItem;
    m_model->appendRow(row);
  }
  m_table->resizeColumnsToContents();
}
