#pragma once
#include <QWidget>
class RentalService;
class QTableView;
class QStandardItemModel;

class ReturnsTab : public QWidget {
  Q_OBJECT
public:
  explicit ReturnsTab(RentalService* service, QWidget* parent = nullptr);
public slots:
  void refresh();
private slots:
  void onProcessReturn();
private:
  RentalService* m_service{nullptr};
  QTableView* m_table{nullptr};
  QStandardItemModel* m_model{nullptr};
};
