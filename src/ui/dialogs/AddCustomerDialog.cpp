#include "ui/dialogs/AddCustomerDialog.h"
#include "utils/Validation.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>

AddCustomerDialog::AddCustomerDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Add Customer");
  auto* form = new QFormLayout(this);

  m_id = new QSpinBox(this); m_id->setRange(1, 1000000);
  m_name = new QLineEdit(this);
  m_license = new QLineEdit(this);
  m_phone = new QLineEdit(this);

  form->addRow("ID", m_id);
  form->addRow("Full name", m_name);
  form->addRow("License no", m_license);
  form->addRow("Phone", m_phone);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  form->addRow(buttons);
}

Customer AddCustomerDialog::buildCustomer() const {
  Validation::requireNonEmpty(m_name->text(), "Full name");
  Validation::requireLicense(m_license->text());
  Validation::requirePhone(m_phone->text());
  return Customer(m_id->value(), m_name->text().trimmed(), m_license->text().trimmed(), m_phone->text().trimmed());
}
