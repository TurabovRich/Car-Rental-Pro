#include "ui/MainWindow.h"
#include "service/RentalService.h"
#include "ui/tabs/CarsTab.h"
#include "ui/tabs/CustomersTab.h"
#include "ui/tabs/ReservationsTab.h"
#include "ui/tabs/ReturnsTab.h"
#include "ui/tabs/ReportsTab.h"
#include "ui/tabs/UserPortalTab.h"
#include "ui/tabs/MyRentalsTab.h"
#include "ui/tabs/UserProfileTab.h"
#include "ui/tabs/UserHistoryTab.h"
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(RentalService* service,
                       Mode mode,
                       int customerId,
                       const QString& username,
                       QWidget* parent)
  : QMainWindow(parent),
    m_service(service),
    m_mode(mode),
    m_customerId(customerId),
    m_username(username) {
  setWindowTitle((m_mode == Mode::Admin) ? "CarRentalPro — Admin" : ("CarRentalPro — " + m_username));
  resize(1000, 650);

  auto* root = new QWidget(this);
  auto* rootLayout = new QVBoxLayout(root);
  rootLayout->setContentsMargins(10, 10, 10, 10);
  rootLayout->setSpacing(8);

  auto* header = new QWidget(root);
  auto* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(0, 0, 0, 0);
  headerLayout->setSpacing(10);

  auto* appTitle = new QLabel("CarRentalPro", header);
  appTitle->setStyleSheet("font-size: 14px; font-weight: 700;");
  headerLayout->addWidget(appTitle);
  headerLayout->addStretch(1);

  m_userLabel = new QLabel(header);
  m_userLabel->setStyleSheet("color: #444444; font-weight: 600;");
  m_userLabel->setText((m_mode == Mode::Admin) ? ("Admin: " + m_username) : m_username);
  headerLayout->addWidget(m_userLabel);

  m_logoutBtn = new QPushButton("Logout", header);
  headerLayout->addWidget(m_logoutBtn);
  rootLayout->addWidget(header);

  connect(m_logoutBtn, &QPushButton::clicked, this, &MainWindow::logoutRequested);

  m_tabs = new QTabWidget(root);

  // admin vs user see different tabs
  if (m_mode == Mode::Admin) {
    m_carsTab = new CarsTab(m_service, m_tabs);
    m_customersTab = new CustomersTab(m_service, m_tabs);
    m_reservationsTab = new ReservationsTab(m_service, m_tabs);
    m_returnsTab = new ReturnsTab(m_service, m_tabs);
    m_reportsTab = new ReportsTab(m_service, m_tabs);

    m_tabs->addTab(m_carsTab, "Cars");
    m_tabs->addTab(m_customersTab, "Customers");
    m_tabs->addTab(m_reservationsTab, "Reservations");
    m_tabs->addTab(m_returnsTab, "Returns");
    m_tabs->addTab(m_reportsTab, "Reports");
  } else {
    m_userPortalTab = new UserPortalTab(m_service, m_customerId, m_tabs);
    m_myRentalsTab = new MyRentalsTab(m_service, m_customerId, m_tabs);
    m_userHistoryTab = new UserHistoryTab(m_service, m_customerId, m_tabs);
    m_userProfileTab = new UserProfileTab(m_service, m_customerId, m_tabs);
    m_tabs->addTab(m_userPortalTab, "Rent a Car");
    m_tabs->addTab(m_myRentalsTab, "My Rentals");
    m_tabs->addTab(m_userHistoryTab, "History");
    m_tabs->addTab(m_userProfileTab, "Profile");
  }
  rootLayout->addWidget(m_tabs, 1);
  setCentralWidget(root);

  auto refreshAll = [this]() {
    if (m_carsTab) m_carsTab->refresh();
    if (m_customersTab) m_customersTab->refresh();
    if (m_reservationsTab) m_reservationsTab->refresh();
    if (m_returnsTab) m_returnsTab->refresh();
    if (m_reportsTab) m_reportsTab->refresh();
    if (m_userPortalTab) m_userPortalTab->refresh();
    if (m_myRentalsTab) m_myRentalsTab->refresh();
    if (m_userHistoryTab) m_userHistoryTab->refresh();
    if (m_userProfileTab) m_userProfileTab->refresh();
  };

  // reload table when switching tabs
  connect(m_tabs, &QTabWidget::currentChanged, this, [refreshAll](int){ refreshAll(); });
  refreshAll();
}

void MainWindow::closeEvent(QCloseEvent* event) {
  QMainWindow::closeEvent(event);
  emit sessionEnded(); // tells main.cpp to leave the event loop
}
