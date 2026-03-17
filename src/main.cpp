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
  // We keep the process alive across sessions:
  // - Close user/admin window -> either re-open login (logout) or exit the app.
  // This avoids relaunching the process just to "log out".
  app.setQuitOnLastWindowClosed(false);

  QString dataDir = QDir(QCoreApplication::applicationDirPath()).filePath("data");
  if (!QDir(dataDir).exists()) dataDir = QDir::current().filePath("data");

  FileManager storage(dataDir);
  RentalService service(&storage);
  AuthService auth(&storage, &service);

  try {
    service.load();
  } catch (const std::exception& e) {
    QMessageBox::warning(nullptr, "Load warning", QString("Could not load data: ") + e.what());
  }

  try {
    auth.load();
    auth.ensureDefaultAdmin();
  } catch (const std::exception& e) {
    QMessageBox::warning(nullptr, "Auth warning", QString("Could not load users: ") + e.what());
  }

  // "Session loop":
  // - Show auth dialog
  // - Open the appropriate main window (admin vs user)
  // - If user clicks Logout -> close window and restart auth dialog
  // - If user closes window normally -> exit application
  bool loggedOut = false;
  while (true) {
    loggedOut = false;
    AuthDialog login(&auth);
    if (login.exec() != QDialog::Accepted || !login.authenticated().has_value()) break;

    UserAccount acc = *login.authenticated();
    MainWindow::Mode mode = (acc.role.compare("Admin", Qt::CaseInsensitive) == 0)
                              ? MainWindow::Mode::Admin
                              : MainWindow::Mode::User;

    auto* w = new MainWindow(&service, mode, acc.customerId, acc.username);
    QObject::connect(w, &MainWindow::logoutRequested, w, [&loggedOut, w]() {
      loggedOut = true;
      w->close();
    });

    QEventLoop loop;
    // `MainWindow` emits `sessionEnded()` from `closeEvent`, which is used to stop this inner loop.
    QObject::connect(w, &MainWindow::sessionEnded, &loop, &QEventLoop::quit);
    w->show();
    loop.exec();
    w->deleteLater();

    if (!loggedOut) break;
  }

  int code = 0;

  try { service.save(); } catch (...) {}
  try { auth.save(); } catch (...) {}
  return code;
}
