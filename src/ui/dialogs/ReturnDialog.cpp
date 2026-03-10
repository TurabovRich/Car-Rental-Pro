#include "ui/dialogs/ReturnDialog.h"
#include "service/RentalService.h"
#include "domain/Customer.h"
#include "domain/Vehicle.h"
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDate>

static QString reservationLabel(const RentalService* service, const Reservation& r) {
  if (!service) return "Reservation " + QString::number(r.id);
  QString customerName = QString::number(r.customerId);
  if (auto c = service->findCustomer(r.customerId); c.has_value()) customerName = c->fullName;

  QString carLabel = QString::number(r.vehicleId);
  if (auto v = service->findVehicle(r.vehicleId); v) {
    carLabel = v->plate + " — " + v->type() + " " + v->brand + " " + v->model;
  }

  return "#" + QString::number(r.id) + " — " + customerName + " — " + carLabel +
         " (" + r.start.toIso() + " → " + r.end.toIso() + ")";
}

ReturnDialog::ReturnDialog(RentalService* service, int preselectReservationId, QWidget* parent)
  : QDialog(parent), m_service(service) {
  setWindowTitle("Process Return");
  auto* form = new QFormLayout(this);

  m_reservation = new QComboBox(this);
  int preselectIndex = -1;
  int idx = 0;
  for (const auto& r : m_service->reservations()) {
    if (r.status != "Active") continue;
    m_reservation->addItem(reservationLabel(m_service, r), r.id);
    if (r.id == preselectReservationId) preselectIndex = idx;
    idx++;
  }
  if (preselectIndex >= 0) m_reservation->setCurrentIndex(preselectIndex);

  m_returnDate = new QDateEdit(QDate::currentDate(), this);
  m_returnDate->setCalendarPopup(true);
  m_returnDate->setDisplayFormat("yyyy-MM-dd");

  m_lateDays = new QLabel(this);
  m_damageFee = new QDoubleSpinBox(this); m_damageFee->setRange(0.0, 100000.0); m_damageFee->setDecimals(2);
  m_details = new QLabel(this);
  m_details->setWordWrap(true);
  m_result = new QLabel(this);
  m_result->setWordWrap(true);

  form->addRow("Reservation", m_reservation);
  form->addRow("Return date", m_returnDate);
  form->addRow("Late days (auto)", m_lateDays);
  form->addRow("Damage fee", m_damageFee);

  auto* processBtn = new QPushButton("Process", this);
  form->addRow(processBtn);
  form->addRow("Reservation details", m_details);
  form->addRow("Invoice preview", m_result);

  auto* closeBtns = new QDialogButtonBox(QDialogButtonBox::Close, this);
  connect(closeBtns, &QDialogButtonBox::rejected, this, &QDialog::reject);
  form->addRow(closeBtns);

  connect(processBtn, &QPushButton::clicked, this, &ReturnDialog::onProcess);
  connect(m_reservation, &QComboBox::currentIndexChanged, this, &ReturnDialog::recomputePreview);
  connect(m_returnDate, &QDateEdit::dateChanged, this, &ReturnDialog::recomputePreview);
  connect(m_damageFee, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ReturnDialog::recomputePreview);
  recomputePreview();
}

void ReturnDialog::recomputePreview() {
  try {
    if (m_reservation->count() == 0) {
      m_lateDays->setText("-");
      m_details->setText("No active reservations.");
      m_result->setText("-");
      return;
    }

    int resId = m_reservation->currentData().toInt();
    const Reservation* rptr = nullptr;
    for (const auto& r : m_service->reservations()) {
      if (r.id == resId) { rptr = &r; break; }
    }
    if (!rptr) { m_details->setText("Reservation not found"); return; }

    QString customerName = QString::number(rptr->customerId);
    if (auto c = m_service->findCustomer(rptr->customerId); c.has_value()) customerName = c->fullName;

    auto v = m_service->findVehicle(rptr->vehicleId);
    QString carLine = "Vehicle: " + QString::number(rptr->vehicleId);
    QString rateLine = "Daily rate: -";
    if (v) {
      carLine = "Vehicle: " + v->plate + " — " + v->type() + " " + v->brand + " " + v->model + " (" + QString::number(v->year) + ")";
      rateLine = "Daily rate: $" + QString::number(v->dailyRate(), 'f', 2);
    }

    Date returnDate = Date::fromIso(m_returnDate->date().toString(Qt::ISODate));
    int lateDays = 0;
    if (rptr->end < returnDate) lateDays = Date::daysBetween(rptr->end, returnDate);
    m_lateDays->setText(QString::number(lateDays));

    auto quote = m_service->previewReturn(resId, returnDate, m_damageFee->value());

    m_details->setText(
      "Customer: " + customerName + "\n" +
      carLine + "\n" +
      "Reserved: " + rptr->start.toIso() + " → " + rptr->end.toIso() + "\n" +
      "Return: " + returnDate.toIso() + "\n" +
      rateLine
    );

    m_result->setText(
      "Subtotal: $" + QString::number(quote.subtotal, 'f', 2) + "\n" +
      "VAT (" + QString::number(Invoice::VAT_RATE * 100.0, 'f', 0) + "%): $" + QString::number(quote.vat, 'f', 2) + "\n" +
      "Late fee: $" + QString::number(quote.lateFee, 'f', 2) + "\n" +
      "Damage fee: $" + QString::number(quote.damageFee, 'f', 2) + "\n" +
      "Total: $" + QString::number(quote.total, 'f', 2)
    );
  } catch (const std::exception& e) {
    m_details->setText(QString("Error: ") + e.what());
    m_result->setText("-");
  }
}

void ReturnDialog::onProcess() {
  try {
    if (m_reservation->count() == 0) {
      QMessageBox::information(this, "Info", "No active reservations.");
      return;
    }
    int resId = m_reservation->currentData().toInt();
    Date returnDate = Date::fromIso(m_returnDate->date().toString(Qt::ISODate));
    auto inv = m_service->processReturn(resId, returnDate, m_damageFee->value());
    QMessageBox::information(this, "Return processed",
                             "Invoice #" + QString::number(inv.id) +
                             "\nTotal: $" + QString::number(inv.total, 'f', 2));
    accept();
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}
