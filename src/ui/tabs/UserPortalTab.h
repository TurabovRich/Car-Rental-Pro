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

  QComboBox* m_customer{nullptr};
  QLineEdit* m_search{nullptr};
  QTableView* m_table{nullptr};
  QStandardItemModel* m_model{nullptr};
  QSortFilterProxyModel* m_proxy{nullptr};

  QDateEdit* m_start{nullptr};
  QDateEdit* m_end{nullptr};
  QLabel* m_pricePreview{nullptr};
  QPushButton* m_reserveBtn{nullptr};
};
