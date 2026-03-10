#pragma once
#include <QWidget>
class RentalService;
class QLabel;
class QTableView;
class QStandardItemModel;

class ReportsTab : public QWidget {
  Q_OBJECT
public:
  explicit ReportsTab(RentalService* service, QWidget* parent = nullptr);
public slots:
  void refresh();
private:
  QLabel* makeCard(const QString& title, const QString& value);

  RentalService* m_service{nullptr};
  QLabel* m_revenueLabel{nullptr};
  QLabel* m_kpiInvoices{nullptr};
  QLabel* m_kpiActive{nullptr};
  QLabel* m_kpiCars{nullptr};
  QTableView* m_table{nullptr};
  QStandardItemModel* m_model{nullptr};
};
