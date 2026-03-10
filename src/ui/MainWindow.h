#pragma once
#include <QMainWindow>
class RentalService;
class QTabWidget;
class CarsTab;
class CustomersTab;
class ReservationsTab;
class ReturnsTab;
class ReportsTab;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(RentalService* service, QWidget* parent = nullptr);
private:
  RentalService* m_service{nullptr};
  QTabWidget* m_tabs{nullptr};
  CarsTab* m_carsTab{nullptr};
  CustomersTab* m_customersTab{nullptr};
  ReservationsTab* m_reservationsTab{nullptr};
  ReturnsTab* m_returnsTab{nullptr};
  ReportsTab* m_reportsTab{nullptr};
};
