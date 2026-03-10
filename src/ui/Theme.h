#pragma once
#include <QString>

class QApplication;

namespace Theme {
  void apply(QApplication& app);
  QString styleSheet();
}

