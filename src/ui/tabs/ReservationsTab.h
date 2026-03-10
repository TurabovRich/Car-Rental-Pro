#pragma once
#include <QWidget>
class RentalService;
class QTableView;
class QStandardItemModel;

class ReservationsTab : public QWidget {
  Q_OBJECT
public:
  explicit ReservationsTab(RentalService* service, QWidget* parent = nullptr);
public slots:
  void refresh();
private slots:
  void onNewReservation();
  void onProcessReturn();
private:
  RentalService* m_service{nullptr};
  QTableView* m_table{nullptr};
  QStandardItemModel* m_model{nullptr};
};
