#pragma once
#include <QDialog>
#include "domain/Customer.h"
class QLineEdit; class QSpinBox;

class AddCustomerDialog : public QDialog {
  Q_OBJECT
public:
  explicit AddCustomerDialog(QWidget* parent = nullptr);
  Customer buildCustomer() const;
private:
  QSpinBox* m_id{};
  QLineEdit* m_name{};
  QLineEdit* m_license{};
  QLineEdit* m_phone{};
};
