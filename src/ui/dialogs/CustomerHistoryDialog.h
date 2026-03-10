#pragma once
#include <QDialog>

class RentalService;
class QLabel;
class QTabWidget;
class QTableView;
class QStandardItemModel;

class CustomerHistoryDialog : public QDialog {
  Q_OBJECT
public:
  explicit CustomerHistoryDialog(RentalService* service, int customerId, QWidget* parent = nullptr);

private:
  void buildReservations();
  void buildInvoices();
  void updateSummary();

  RentalService* m_service{nullptr};
  int m_customerId{0};

  QLabel* m_header{nullptr};
  QLabel* m_summary{nullptr};

  QTabWidget* m_tabs{nullptr};
  QTableView* m_resTable{nullptr};
  QStandardItemModel* m_resModel{nullptr};
  QTableView* m_invTable{nullptr};
  QStandardItemModel* m_invModel{nullptr};
};

