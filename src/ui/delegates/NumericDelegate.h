#pragma once
#include <QStyledItemDelegate>

class NumericDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  explicit NumericDelegate(QObject* parent = nullptr)
    : QStyledItemDelegate(parent) {}

  void initStyleOption(QStyleOptionViewItem* option,
                       const QModelIndex& index) const override {
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
  }
};

