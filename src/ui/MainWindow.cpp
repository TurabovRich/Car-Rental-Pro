#include "ui/MainWindow.h"
#include "service/RentalService.h"
#include "ui/tabs/CarsTab.h"
#include "ui/tabs/CustomersTab.h"
#include "ui/tabs/ReservationsTab.h"
#include "ui/tabs/ReturnsTab.h"
#include "ui/tabs/ReportsTab.h"
#include <QTabWidget>

MainWindow::MainWindow(RentalService* service, QWidget* parent)
  : QMainWindow(parent), m_service(service) {
  setWindowTitle("CarRentalPro");
  resize(1000, 650);

  m_tabs = new QTabWidget(this);
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
  setCentralWidget(m_tabs);

  auto refreshAll = [this]() {
    if (m_carsTab) m_carsTab->refresh();
    if (m_customersTab) m_customersTab->refresh();
    if (m_reservationsTab) m_reservationsTab->refresh();
    if (m_returnsTab) m_returnsTab->refresh();
    if (m_reportsTab) m_reportsTab->refresh();
  };

  connect(m_tabs, &QTabWidget::currentChanged, this, [refreshAll](int){ refreshAll(); });
  refreshAll();
}
