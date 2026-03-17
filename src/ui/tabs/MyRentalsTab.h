#pragma once

#include <QWidget>

class RentalService;
class QStandardItemModel;
class QTableView;
class QTabWidget;
class QPushButton;

class MyRentalsTab : public QWidget {
  Q_OBJECT
public:
  explicit MyRentalsTab(RentalService* service, int customerId, QWidget* parent = nullptr);

public slots:
  void refresh();

private:
  void onReturnSelected();
  void buildTable(QTableView* table, QStandardItemModel* model, const QString& statusFilter);

  RentalService* m_service{nullptr};
  int m_customerId{0};

  QTabWidget* m_tabs{nullptr};
  QPushButton* m_returnBtn{nullptr};
  QTableView* m_activeTable{nullptr};
  QStandardItemModel* m_activeModel{nullptr};
  QTableView* m_returnedTable{nullptr};
  QStandardItemModel* m_returnedModel{nullptr};
};

