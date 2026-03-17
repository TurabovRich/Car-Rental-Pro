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

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  enum class Mode { Admin, User };
  /**
   * Main application window.
   *
   * The same window class is used for two distinct experiences:
   * - Admin mode: full CRUD + reports.
   * - User mode: restricted UI for a single customer (`customerId`) and a visible username.
   *
   * Logout is implemented by emitting `logoutRequested()`; `main.cpp` listens and restarts the auth dialog.
   */
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
