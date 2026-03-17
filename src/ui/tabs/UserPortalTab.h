#pragma once

#include <QWidget>

class RentalService;
class QComboBox;
class QDateEdit;
class QLabel;
class QLineEdit;
class QPushButton;
class QSortFilterProxyModel;
class QStandardItemModel;
class QTableView;

/**
 * User-facing "rent a car" tab.
 *
 * UX / security constraints:
 * - In normal user mode, the customer is fixed (`lockedCustomerId`) and must not be switchable.
 * - Vehicle IDs are internal; the table hides them and stores the ID in `Qt::UserRole`.
 * - Car list is filtered by the currently selected date range to avoid showing unavailable cars.
 */
class UserPortalTab : public QWidget {
  Q_OBJECT
public:
  explicit UserPortalTab(RentalService* service, int lockedCustomerId = 0, QWidget* parent = nullptr);

public slots:
  void refresh();

private slots:
  void onReserveSelected();
  void onSearchTextChanged(const QString& text);
  void onSelectionChanged();

private:
  int selectedCustomerId() const;
  int selectedVehicleId() const;
  void updatePreview();

  RentalService* m_service{nullptr};
  int m_lockedCustomerId{0};

  QComboBox* m_customer{nullptr}; // hidden/locked for normal users

  QLineEdit* m_search{nullptr};
  QTableView* m_table{nullptr};
  QStandardItemModel* m_model{nullptr};
  QSortFilterProxyModel* m_proxy{nullptr};

  QDateEdit* m_start{nullptr};
  QDateEdit* m_end{nullptr};
  QLabel* m_pricePreview{nullptr};
  QPushButton* m_reserveBtn{nullptr};
};

