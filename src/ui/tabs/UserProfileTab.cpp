#include "ui/tabs/UserProfileTab.h"

#include "service/RentalService.h"
#include "domain/Customer.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

UserProfileTab::UserProfileTab(RentalService* service, int customerId, QWidget* parent)
  : QWidget(parent), m_service(service), m_customerId(customerId) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(10);

  m_header = new QLabel(this);
  m_header->setWordWrap(true);
  m_header->setStyleSheet("font-size: 16px; font-weight: 700;");
  root->addWidget(m_header);

  auto* card = new QWidget(this);
  card->setObjectName("profileCard");
  card->setStyleSheet(
    "#profileCard {"
    "  background: #ffffff;"
    "  border: 1px solid #e2e2e2;"
    "  border-radius: 12px;"
    "}"
  );

  auto* form = new QFormLayout(card);
  form->setContentsMargins(14, 14, 14, 14);
  form->setSpacing(10);

  m_fullName = new QLineEdit(card);
  m_license = new QLineEdit(card);
  m_phone = new QLineEdit(card);

  form->addRow("Full name", m_fullName);
  form->addRow("License", m_license);
  form->addRow("Phone", m_phone);

  auto* row = new QHBoxLayout();
  row->addStretch(1);
  m_save = new QPushButton("Save changes", card);
  row->addWidget(m_save);
  form->addRow(row);

  root->addWidget(card);
  root->addStretch(1);

  connect(m_save, &QPushButton::clicked, this, &UserProfileTab::onSave);
  refresh();
}

void UserProfileTab::refresh() {
  if (!m_service || m_customerId == 0) {
    m_header->setText("Profile");
    m_fullName->setEnabled(false);
    m_license->setEnabled(false);
    m_phone->setEnabled(false);
    m_save->setEnabled(false);
    return;
  }

  auto c = m_service->findCustomer(m_customerId);
  if (!c.has_value()) {
    m_header->setText("Profile (not found)");
    m_save->setEnabled(false);
    return;
  }

  m_header->setText("Profile — " + c->fullName);
  m_fullName->setText(c->fullName);
  m_license->setText(c->licenseNo);
  m_phone->setText(c->phone);
  m_fullName->setEnabled(true);
  m_license->setEnabled(true);
  m_phone->setEnabled(true);
  m_save->setEnabled(true);
}

void UserProfileTab::onSave() {
  try {
    if (!m_service || m_customerId == 0) return;
    Customer c;
    c.id = m_customerId;
    c.fullName = m_fullName->text().trimmed();
    c.licenseNo = m_license->text().trimmed();
    c.phone = m_phone->text().trimmed();
    m_service->updateCustomer(c);
    QMessageBox::information(this, "Saved", "Profile updated.");
    refresh();
  } catch (const std::exception& e) {
    QMessageBox::warning(this, "Save failed", e.what());
  }
}

