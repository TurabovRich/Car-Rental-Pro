#include "ui/delegates/StatusBadgeDelegate.h"
#include <QPainter>
#include <QApplication>

StatusBadgeDelegate::StatusBadgeDelegate(QObject* parent)
  : QStyledItemDelegate(parent) {}

void StatusBadgeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
  QString text = index.data(Qt::DisplayRole).toString();

  QColor bg("#e0e0e0");
  QColor fg("#111111");

  const QString lower = text.toLower();
  if (lower == "true" || lower == "available") {
    text = "Available";
    bg = QColor("#e3f6e8");
    fg = QColor("#137333");
  } else if (lower == "false" || lower == "unavailable") {
    text = "Unavailable";
    bg = QColor("#fde8e8");
    fg = QColor("#b00020");
  } else if (lower == "active" || lower == "reserved") {
    text = "Active";
    bg = QColor("#fff4e5");
    fg = QColor("#b05a00");
  } else if (lower == "returned" || lower == "completed") {
    text = "Returned";
    bg = QColor("#eceff1");
    fg = QColor("#455a64");
  }

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  painter->save();

  // Draw background (selected state)
  QStyle* style = QApplication::style();
  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

  QRect r = opt.rect.adjusted(8, 4, -8, -4);
  const int radius = r.height() / 2;

  // Limit badge width so it does not span entire cell
  QFontMetrics fm(opt.font);
  int textWidth = fm.horizontalAdvance(text);
  int badgeWidth = textWidth + 24; // horizontal padding
  badgeWidth = qMin(badgeWidth, r.width());
  QRect badgeRect(r);
  badgeRect.setWidth(badgeWidth);
  badgeRect.moveCenter(r.center());

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(bg);
  painter->setPen(Qt::NoPen);
  painter->drawRoundedRect(badgeRect, radius, radius);

  painter->setPen(fg);
  painter->drawText(badgeRect, Qt::AlignCenter, text);

  painter->restore();
}

