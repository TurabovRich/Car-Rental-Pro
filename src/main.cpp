#include <QApplication>
#include <QEventLoop>
#include <QMessageBox>
#include <QDir>
#include "ui/MainWindow.h"
#include "ui/Theme.h"
#include "service/RentalService.h"
#include "service/AuthService.h"
#include "storage/FileManager.h"
#include "ui/dialogs/AuthDialog.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Theme::apply(app);
  app.setQuitOnLastWindowClosed(false);

  QString dataDir = QDir(QCoreApplication::applicationDirPath()).filePath("data");
  if (!QDir(dataDir).exists()) dataDir = QDir::current().filePath("data");

  FileManager storage(dataDir);
  RentalService service(&storage);
  AuthService auth(&storage, &service);

  try {
    service.load();
  } catch (const std::exception& e) {
    QMessageBox::warning(nullptr, "Warning", e.what());
  }

  try {
    auth.load();
    auth.ensureDefaultAdmin();
  } catch (const std::exception& e) {
    QMessageBox::warning(nullptr, "Warning", e.what());
  }

  bool loggedOut = false;
  while (true) {
    loggedOut = false;
    AuthDialog login(&auth);
    if (login.exec() != QDialog::Accepted || !login.ok()) break;

    UserAccount acc = login.account();
    MainWindow::Mode mode = (acc.role.compare("Admin", Qt::CaseInsensitive) == 0)
                              ? MainWindow::Mode::Admin
                              : MainWindow::Mode::User;

    auto* w = new MainWindow(&service, mode, acc.customerId, acc.username);
    QObject::connect(w, &MainWindow::logoutRequested, w, [&loggedOut, w]() {
      loggedOut = true;
      w->close();
    });

    QEventLoop loop;
    QObject::connect(w, &MainWindow::sessionEnded, &loop, &QEventLoop::quit);
    w->show();
    loop.exec();
    w->deleteLater();

    if (!loggedOut) break;
  }

  try { service.save(); } catch (...) {}
  try { auth.save(); } catch (...) {}
  return 0;
}
