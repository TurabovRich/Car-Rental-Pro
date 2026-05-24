#pragma once

#include <QWidget>

class RentalService;
class QStandardItemModel;
class QTableView;
class QTabWidget;
class QLabel;

// Past reservations and invoices for one customer.
class UserHistoryTab : public QWidget {
  Q_OBJECT
public:
  explicit UserHistoryTab(RentalService* service, int customerId, QWidget* parent = nullptr);

public slots:
  void refresh();

private:
  void buildReservations();
  void buildInvoices();
  void updateSummary();

  RentalService* m_service{nullptr};
  int m_customerId{0};

  QLabel* m_summary{nullptr};
  QTabWidget* m_tabs{nullptr};
  QTableView* m_resTable{nullptr};
  QStandardItemModel* m_resModel{nullptr};
  QTableView* m_invTable{nullptr};
  QStandardItemModel* m_invModel{nullptr};
};
