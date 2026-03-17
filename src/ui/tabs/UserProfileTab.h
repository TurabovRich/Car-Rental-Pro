#pragma once

#include <QWidget>

class RentalService;
class QLineEdit;
class QLabel;
class QPushButton;

/**
 * User-facing profile editor for the linked `Customer`.
 *
 * Persistence:
 * - Updates are written to `customers.csv` via `RentalService::updateCustomer(...)`.
 *
 * This does NOT edit login credentials (username/password), only the customer profile fields.
 */
class UserProfileTab : public QWidget {
  Q_OBJECT
public:
  explicit UserProfileTab(RentalService* service, int customerId, QWidget* parent = nullptr);

public slots:
  void refresh();

private slots:
  void onSave();

private:
  RentalService* m_service{nullptr};
  int m_customerId{0};

  QLabel* m_header{nullptr};
  QLineEdit* m_fullName{nullptr};
  QLineEdit* m_license{nullptr};
  QLineEdit* m_phone{nullptr};
  QPushButton* m_save{nullptr};
};

