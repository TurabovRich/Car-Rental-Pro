#include "ui/tabs/UserPortalTab.h"

#include "service/RentalService.h"
#include "utils/Date.h"

#include <QDateEdit>
#include <QComboBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

UserPortalTab::UserPortalTab(RentalService* service, int lockedCustomerId, QWidget* parent)
  : QWidget(parent), m_service(service), m_lockedCustomerId(lockedCustomerId) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(10);

  m_customer = new QComboBox(this);
  m_customer->setVisible(false);

  auto* searchRow = new QHBoxLayout();
  searchRow->setContentsMargins(0, 0, 0, 0);
  searchRow->setSpacing(8);
  m_search = new QLineEdit(this);
  m_search->setPlaceholderText("Search cars (brand, model, plate, type)...");
  searchRow->addWidget(m_search, 1);
  root->addLayout(searchRow);

  m_table = new QTableView(this);
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels({"Vehicle","Type","Year","Plate","Daily Rate"});
  m_proxy = new QSortFilterProxyModel(this);
  m_proxy->setSourceModel(m_model);
  m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
  m_proxy->setFilterKeyColumn(-1);
  m_table->setModel(m_proxy);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setAlternatingRowColors(true);
  m_table->setSortingEnabled(true);
  m_table->horizontalHeader()->setStretchLastSection(true);
  root->addWidget(m_table, 1);

  auto* book = new QWidget(this);
  book->setObjectName("bookingCard");
  book->setStyleSheet(
    "#bookingCard {"
    "  background: #ffffff;"
    "  border: 1px solid #e2e2e2;"
    "  border-radius: 12px;"
    "}"
  );
  auto* bookLayout = new QHBoxLayout(book);
  bookLayout->setContentsMargins(12, 10, 12, 10);
  bookLayout->setSpacing(10);

  m_start = new QDateEdit(QDate::currentDate(), book);
  m_start->setCalendarPopup(true);
  m_start->setDisplayFormat("yyyy-MM-dd");
  m_end = new QDateEdit(QDate::currentDate().addDays(1), book);
  m_end->setCalendarPopup(true);
  m_end->setDisplayFormat("yyyy-MM-dd");

  bookLayout->addWidget(new QLabel("Start", book));
  bookLayout->addWidget(m_start);
  bookLayout->addWidget(new QLabel("End", book));
  bookLayout->addWidget(m_end);

  m_pricePreview = new QLabel(book);
  m_pricePreview->setStyleSheet("color: #555555;");
  m_pricePreview->setMinimumWidth(280);
  bookLayout->addWidget(m_pricePreview, 1);

  m_reserveBtn = new QPushButton("Reserve selected", book);
  bookLayout->addWidget(m_reserveBtn);
  root->addWidget(book);

  connect(m_reserveBtn, &QPushButton::clicked, this, &UserPortalTab::onReserveSelected);
  connect(m_search, &QLineEdit::textChanged, this, &UserPortalTab::onSearchTextChanged);
  connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &UserPortalTab::onSelectionChanged);
  connect(m_start, &QDateEdit::dateChanged, this, [this]{
    refresh();
  });
  connect(m_end, &QDateEdit::dateChanged, this, [this]{
    refresh();
  });

  refresh();
}

int UserPortalTab::selectedCustomerId() const {
  if (m_lockedCustomerId != 0) return m_lockedCustomerId;
  if (!m_customer || m_customer->count() == 0) return 0;
  return m_customer->currentData().toInt();
}

int UserPortalTab::selectedVehicleId() const {
  if (!m_table) return 0;
  auto proxyIdx = m_table->currentIndex();
  if (!proxyIdx.isValid()) return 0;
  auto idx = m_proxy->mapToSource(proxyIdx);
  if (!m_model->item(idx.row(), 0)) return 0;
  return m_model->item(idx.row(), 0)->data(Qt::UserRole).toInt();
}

void UserPortalTab::refresh() {
  if (!m_service) return;

  m_customer->clear();
  for (const auto& c : m_service->customers())
    m_customer->addItem(QString::number(c.id) + " - " + c.fullName, c.id);

  if (m_lockedCustomerId != 0) {
    int idx = m_customer->findData(m_lockedCustomerId);
    if (idx >= 0) m_customer->setCurrentIndex(idx);
    m_customer->setEnabled(false);
  } else {
    m_customer->setVisible(true);
    m_customer->setEnabled(m_customer->count() > 0);
  }

  m_model->removeRows(0, m_model->rowCount());

  Date s;
  Date e;
  bool haveDates = true;
  try {
    s = Date::fromIso(m_start->date().toString(Qt::ISODate));
    e = Date::fromIso(m_end->date().toString(Qt::ISODate));
    (void)Date::daysBetween(s, e);
  } catch (...) {
    haveDates = false;
  }

  for (const auto& v : m_service->vehicles()) {
    if (!v) continue;
    if (!v->available) continue;
    // Important: availability is date-based, not just the `available` flag.
    // A vehicle can be globally enabled, but still booked for a selected date range.
    if (haveDates && !m_service->isVehicleAvailable(v->id, s, e)) continue;

    QList<QStandardItem*> row;
    auto* vehicleItem = new QStandardItem(v->brand + " " + v->model);
    vehicleItem->setData(v->id, Qt::UserRole); // keep internal ID hidden
    row << vehicleItem
        << new QStandardItem(v->type())
        << new QStandardItem(QString::number(v->year))
        << new QStandardItem(v->plate)
        << new QStandardItem(QString::number(v->dailyRate(), 'f', 2));
    for (auto* it : row) it->setTextAlignment(Qt::AlignCenter);
    m_model->appendRow(row);
  }
  m_table->resizeColumnsToContents();
  updatePreview();
}

void UserPortalTab::onSearchTextChanged(const QString& text) {
  m_proxy->setFilterFixedString(text);
}

void UserPortalTab::onSelectionChanged() {
  updatePreview();
}

void UserPortalTab::updatePreview() {
  int vehicleId = selectedVehicleId();
  if (!m_service || vehicleId == 0) {
    m_pricePreview->setText("Select a car to see price preview.");
    m_reserveBtn->setEnabled(false);
    return;
  }

  auto v = m_service->findVehicle(vehicleId);
  if (!v) {
    m_pricePreview->setText("Vehicle not found.");
    m_reserveBtn->setEnabled(false);
    return;
  }

  try {
    Date s = Date::fromIso(m_start->date().toString(Qt::ISODate));
    Date e = Date::fromIso(m_end->date().toString(Qt::ISODate));
    int days = Date::daysBetween(s, e);
    double subtotal = days * v->dailyRate();
    m_pricePreview->setText(QString::number(days) + " day(s) × $" +
                            QString::number(v->dailyRate(), 'f', 2) +
                            " = $" + QString::number(subtotal, 'f', 2));
    bool dateOk = days > 0 && m_service->isVehicleAvailable(vehicleId, s, e);
    if (!dateOk) {
      m_pricePreview->setText("Not available for selected dates.");
    }
    m_reserveBtn->setEnabled(dateOk && selectedCustomerId() != 0);
  } catch (const std::exception& e) {
    m_pricePreview->setText(QString("Invalid dates: ") + e.what());
    m_reserveBtn->setEnabled(false);
  }
}

void UserPortalTab::onReserveSelected() {
  try {
    if (!m_service) return;
    int customerId = selectedCustomerId();
    if (customerId == 0) {
      QMessageBox::information(this, "Info", "Please select a user profile.");
      return;
    }
    int vehicleId = selectedVehicleId();
    if (vehicleId == 0) {
      QMessageBox::information(this, "Info", "Please select a car to reserve.");
      return;
    }

    Date s = Date::fromIso(m_start->date().toString(Qt::ISODate));
    Date e = Date::fromIso(m_end->date().toString(Qt::ISODate));
    auto r = m_service->createReservation(customerId, vehicleId, s, e);

    auto v = m_service->findVehicle(vehicleId);
    QString carLine = v ? (v->type() + " " + v->brand + " " + v->model + " (" + v->plate + ")")
                        : ("Vehicle ID " + QString::number(vehicleId));

    int days = Date::daysBetween(s, e);
    double daily = v ? v->dailyRate() : 0.0;
    double subtotal = days * daily;

    refresh();

    QMessageBox::information(this, "Reserved",
                             "Reservation #" + QString::number(r.id) + "\n"
                             + carLine + "\n"
                             + s.toIso() + " - " + e.toIso() + "\n"
                             + QString::number(days) + " days, about $" + QString::number(subtotal, 'f', 2));
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

