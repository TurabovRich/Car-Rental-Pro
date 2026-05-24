#pragma once
#include <QMainWindow>
#include <QString>
class RentalService;
class QTabWidget;
class QLabel;
class QPushButton;
class CarsTab;
class CustomersTab;
class ReservationsTab;
class ReturnsTab;
class ReportsTab;
class UserPortalTab;
class UserProfileTab;
class MyRentalsTab;
class UserHistoryTab;

// Main shell: admin tabs or user tabs depending on who logged in.
class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  enum class Mode { Admin, User };

  explicit MainWindow(RentalService* service,
                      Mode mode,
                      int customerId,
                      const QString& username,
                      QWidget* parent = nullptr);

signals:
  void logoutRequested();
  void sessionEnded();

protected:
  void closeEvent(QCloseEvent* event) override;
private:
  RentalService* m_service{nullptr};
  Mode m_mode{Mode::Admin};
  int m_customerId{0};
  QString m_username;
  QLabel* m_userLabel{nullptr};
  QPushButton* m_logoutBtn{nullptr};
  QTabWidget* m_tabs{nullptr};
  CarsTab* m_carsTab{nullptr};
  CustomersTab* m_customersTab{nullptr};
  ReservationsTab* m_reservationsTab{nullptr};
  ReturnsTab* m_returnsTab{nullptr};
  ReportsTab* m_reportsTab{nullptr};
  UserPortalTab* m_userPortalTab{nullptr};
  MyRentalsTab* m_myRentalsTab{nullptr};
  UserProfileTab* m_userProfileTab{nullptr};
  UserHistoryTab* m_userHistoryTab{nullptr};
};
