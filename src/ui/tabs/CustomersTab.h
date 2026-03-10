#pragma once
#include <QWidget>
class RentalService;
class QTableView;
class QStandardItemModel;
class QSortFilterProxyModel;
class QLineEdit;

class CustomersTab : public QWidget {
  Q_OBJECT
public:
  explicit CustomersTab(RentalService* service, QWidget* parent = nullptr);
public slots:
  void refresh();
private slots:
  void onAdd();
  void onDelete();
  void onHistory();
private:
  RentalService* m_service{nullptr};
  QTableView* m_table{nullptr};
  QStandardItemModel* m_model{nullptr};
  QSortFilterProxyModel* m_proxy{nullptr};
  QLineEdit* m_search{nullptr};
  QWidget* m_details{nullptr};
  void onSelectionChanged();
  void onSearchTextChanged(const QString& text);
};
