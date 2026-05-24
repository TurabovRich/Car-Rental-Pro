#include "ui/dialogs/NewReservationDialog.h"
#include "service/RentalService.h"
#include "utils/Date.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QMessageBox>

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
      m_vehicle->addItem(QString::number(v->id) + " - " + v->plate + " - " + v->type(), v->id);

  m_start = new QDateEdit(QDate::currentDate(), this);
  m_start->setCalendarPopup(true);
  m_start->setDisplayFormat("yyyy-MM-dd");

  m_end = new QDateEdit(QDate::currentDate().addDays(1), this);
  m_end->setCalendarPopup(true);
  m_end->setDisplayFormat("yyyy-MM-dd");

  m_preview = new QLabel(this);
  form->addRow("Customer", m_customer);
  form->addRow("Vehicle", m_vehicle);
  form->addRow("Start", m_start);
  form->addRow("End", m_end);
  form->addRow("Preview", m_preview);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  form->addRow(buttons);
  connect(buttons, &QDialogButtonBox::accepted, this, [this]() { createReservation(); accept(); });
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  connect(m_vehicle, &QComboBox::currentIndexChanged, this, &NewReservationDialog::recomputePreview);
  connect(m_start, &QDateEdit::dateChanged, this, &NewReservationDialog::recomputePreview);
  connect(m_end, &QDateEdit::dateChanged, this, &NewReservationDialog::recomputePreview);
  recomputePreview();
}

void NewReservationDialog::recomputePreview() {
  auto v = m_service->findVehicle(m_vehicle->currentData().toInt());
  if (!v) { m_preview->setText("-"); return; }
  Date s = Date::fromIso(m_start->date().toString(Qt::ISODate));
  Date e = Date::fromIso(m_end->date().toString(Qt::ISODate));
  int days = Date::daysBetween(s, e);
  m_preview->setText(QString::number(days) + " days, $" + QString::number(days * v->dailyRate(), 'f', 2));
}

void NewReservationDialog::createReservation() {
  try {
    int customerId = m_customer->currentData().toInt();
    int vehicleId = m_vehicle->currentData().toInt();
    Date s = Date::fromIso(m_start->date().toString(Qt::ISODate));
    Date e = Date::fromIso(m_end->date().toString(Qt::ISODate));
    m_service->createReservation(customerId, vehicleId, s, e);
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}
