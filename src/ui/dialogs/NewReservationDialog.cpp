#include "ui/dialogs/NewReservationDialog.h"
#include "service/RentalService.h"
#include "utils/Date.h"
#include <stdexcept>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>

NewReservationDialog::NewReservationDialog(RentalService* service, QWidget* parent)
  : QDialog(parent), m_service(service) {
  setWindowTitle("New Reservation");
  auto* form = new QFormLayout(this);

  m_customer = new QComboBox(this);
  for (const auto& c : m_service->customers())
    m_customer->addItem(QString::number(c.id) + " - " + c.fullName, c.id);

  m_vehicle = new QComboBox(this);
  for (const auto& v : m_service->vehicles())
    if (v && v->available)
      m_vehicle->addItem(QString::number(v->id) + " - " + v->plate + " - " + v->type() + " " + v->brand + " " + v->model, v->id);

  m_start = new QDateEdit(QDate::currentDate(), this);
  m_start->setCalendarPopup(true);
  m_start->setDisplayFormat("yyyy-MM-dd");

  m_end = new QDateEdit(QDate::currentDate().addDays(1), this);
  m_end->setCalendarPopup(true);
  m_end->setDisplayFormat("yyyy-MM-dd");

  m_preview = new QLabel(this);

  form->addRow("Customer", m_customer);
  form->addRow("Vehicle", m_vehicle);
  form->addRow("Start date", m_start);
  form->addRow("End date", m_end);
  form->addRow("Price preview", m_preview);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  form->addRow(buttons);

  connect(m_vehicle, &QComboBox::currentIndexChanged, this, &NewReservationDialog::recomputePreview);
  connect(m_start, &QDateEdit::dateChanged, this, &NewReservationDialog::recomputePreview);
  connect(m_end, &QDateEdit::dateChanged, this, &NewReservationDialog::recomputePreview);
  recomputePreview();
}

void NewReservationDialog::recomputePreview() {
  int vehicleId = m_vehicle->currentData().toInt();
  auto v = m_service->findVehicle(vehicleId);
  if (!v) { m_preview->setText("Vehicle not found"); return; }

  try {
    Date s = Date::fromIso(m_start->date().toString(Qt::ISODate));
    Date e = Date::fromIso(m_end->date().toString(Qt::ISODate));
    int days = Date::daysBetween(s, e);
    double subtotal = days * v->dailyRate();
    m_preview->setText(QString::number(days) + " day(s) × $" + QString::number(v->dailyRate(), 'f', 2) +
                       " = $" + QString::number(subtotal, 'f', 2));
  } catch (const std::exception& e) {
    m_preview->setText(QString("Invalid dates: ") + e.what());
  }
}

void NewReservationDialog::createReservation() {
  if (m_customer->count() == 0) throw std::runtime_error("No customers available");
  if (m_vehicle->count() == 0) throw std::runtime_error("No vehicles available (check availability flag)");
  int customerId = m_customer->currentData().toInt();
  int vehicleId = m_vehicle->currentData().toInt();
  Date start = Date::fromIso(m_start->date().toString(Qt::ISODate));
  Date end = Date::fromIso(m_end->date().toString(Qt::ISODate));
  m_service->createReservation(customerId, vehicleId, start, end);
}
