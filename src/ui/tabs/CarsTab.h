#pragma once
#include <QWidget>
class RentalService;
class QTableView;
class QStandardItemModel;
class QSortFilterProxyModel;
class QLineEdit;

class CarsTab : public QWidget {
  Q_OBJECT
public:
  explicit CarsTab(RentalService* service, QWidget* parent = nullptr);
public slots:
  void refresh();
private slots:
  void onAdd();
  void onDelete();
  void onEdit();
  void onSelectionChanged();
  void onSearchTextChanged(const QString& text);
private:
  RentalService* m_service{nullptr};
  QTableView* m_table{nullptr};
  QStandardItemModel* m_model{nullptr};
  QSortFilterProxyModel* m_proxy{nullptr};
  QLineEdit* m_search{nullptr};
  QWidget* m_details{nullptr};
};
