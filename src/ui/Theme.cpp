#include "ui/Theme.h"
#include <QApplication>
#include <QPalette>
#include <QColor>

QString Theme::styleSheet() {
  // Light modern look for Widgets (keeps native feel, but cleaner).
  return QStringLiteral(R"(
QWidget {
  font-size: 13px;
}

QMainWindow {
  background: #f6f7fb;
}

QTabWidget::pane {
  border: 1px solid #e6e6e6;
  background: #ffffff;
  border-radius: 12px;
  margin: 6px;
}

QTabBar::tab {
  background: transparent;
  padding: 10px 14px;
  margin: 6px 4px 0 4px;
  border-top-left-radius: 10px;
  border-top-right-radius: 10px;
  color: #3b3b3b;
}
QTabBar::tab:selected {
  background: #ffffff;
  border: 1px solid #e6e6e6;
  border-bottom: 0px;
  color: #111111;
}
QTabBar::tab:hover {
  background: #f3f5ff;
}

QPushButton {
  background: #2b6fff;
  color: white;
  border: 0px;
  border-radius: 10px;
  padding: 9px 14px;
  font-weight: 600;
}
QPushButton:hover { background: #225fe0; }
QPushButton:pressed { background: #1b4fc0; }
QPushButton:disabled { background: #b8c7ff; }

QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit {
  background: #ffffff;
  border: 1px solid #dcdcdc;
  border-radius: 10px;
  padding: 7px 10px;
}
QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {
  border: 1px solid #2b6fff;
}

QTableView {
  background: #ffffff;
  border: 1px solid #e6e6e6;
  border-radius: 12px;
  gridline-color: #eeeeee;
  selection-background-color: #dbe8ff;
  selection-color: #000000;
  color: #000000;
}
QTableView::item:hover {
  background: #f5f7ff;
  
}
QHeaderView::section {
  background: #f2f4f8;
  border: 0px;
  border-bottom: 1px solid #e6e6e6;
  padding: 10px;
  font-weight: 700;
  color: #000000;
}
QLineEdit[placeholderText=\"true\"] {
  color: #999999;
}
)");
}

void Theme::apply(QApplication& app) {
  app.setStyle("Fusion");

  QPalette p = app.palette();
  p.setColor(QPalette::Window, QColor("#f6f7fb"));
  p.setColor(QPalette::Base, QColor("#ffffff"));
  p.setColor(QPalette::AlternateBase, QColor("#fbfcff"));
  p.setColor(QPalette::Button, QColor("#2b6fff"));
  p.setColor(QPalette::ButtonText, QColor("#ffffff"));
  p.setColor(QPalette::Text, QColor("#111111"));
  p.setColor(QPalette::WindowText, QColor("#111111"));
  app.setPalette(p);

  app.setStyleSheet(styleSheet());
}

