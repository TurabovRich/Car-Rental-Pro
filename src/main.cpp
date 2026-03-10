#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include "ui/MainWindow.h"
#include "ui/Theme.h"
#include "service/RentalService.h"
#include "storage/FileManager.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  Theme::apply(app);

  QString dataDir = QDir(QCoreApplication::applicationDirPath()).filePath("data");
  if (!QDir(dataDir).exists()) dataDir = QDir::current().filePath("data");

  FileManager storage(dataDir);
  RentalService service(&storage);

  try {
    service.load();
  } catch (const std::exception& e) {
    QMessageBox::warning(nullptr, "Load warning", QString("Could not load data: ") + e.what());
  }

  MainWindow w(&service);
  w.show();

  int code = app.exec();

  try { service.save(); } catch (...) {}
  return code;
}
