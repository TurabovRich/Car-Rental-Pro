#include "ui/tabs/CarsTab.h"
#include "service/RentalService.h"
#include "ui/dialogs/AddCarDialog.h"
#include "ui/delegates/StatusBadgeDelegate.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QLabel>
#include <QLineEdit>
#include <QShortcut>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>

CarsTab::CarsTab(RentalService* service, QWidget* parent) : QWidget(parent), m_service(service) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(8);
  auto* bar = new QHBoxLayout();
  bar->setContentsMargins(0, 0, 0, 0);
  bar->setSpacing(8);
  m_search = new QLineEdit(this);
  m_search->setPlaceholderText("Search cars (brand, model, plate)...");
  auto* addBtn = new QPushButton("Add Car");
  auto* editBtn = new QPushButton("Edit Selected");
  auto* delBtn = new QPushButton("Delete Selected");
  bar->addWidget(m_search, 1);
  bar->addStretch(1);
  bar->addWidget(addBtn);
  bar->addWidget(editBtn);
  bar->addWidget(delBtn);
  root->addLayout(bar);

  m_table = new QTableView(this);
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels({"Image","ID","Vehicle","Type","Year","Plate","Daily Rate","Available"});
  m_proxy = new QSortFilterProxyModel(this);
  m_proxy->setSourceModel(m_model);
  m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
  m_proxy->setFilterKeyColumn(-1); // all columns
  m_table->setModel(m_proxy);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setAlternatingRowColors(true);
  m_table->setSortingEnabled(true);
  m_table->horizontalHeader()->setStretchLastSection(true);
  m_table->verticalHeader()->setDefaultSectionSize(56);
  m_table->setIconSize(QSize(64, 48));

  m_table->setItemDelegateForColumn(7, new StatusBadgeDelegate(m_table));

  auto* splitter = new QSplitter(this);
  splitter->addWidget(m_table);

  m_details = new QWidget(this);
  auto* dLayout = new QVBoxLayout(m_details);
  dLayout->setContentsMargins(0, 0, 0, 0);
  dLayout->setSpacing(0);
  auto* placeholderCard = new QWidget(m_details);
  placeholderCard->setObjectName("detailsCard");
  placeholderCard->setStyleSheet(
    "#detailsCard {"
    "  background: #ffffff;"
    "  border: 1px solid #e2e2e2;"
    "  border-radius: 12px;"
    "}"
  );
  auto* pcLayout = new QVBoxLayout(placeholderCard);
  pcLayout->setContentsMargins(16, 16, 16, 16);
  pcLayout->setSpacing(8);
  auto* placeholder = new QLabel("Select a car to view details.", placeholderCard);
  placeholder->setWordWrap(true);
  placeholder->setStyleSheet("color: #777777;");
  pcLayout->addWidget(placeholder);
  pcLayout->addStretch(1);
  dLayout->addWidget(placeholderCard);
  splitter->addWidget(m_details);
  splitter->setStretchFactor(0, 3);
  splitter->setStretchFactor(1, 2);
  root->addWidget(splitter);

  connect(addBtn, &QPushButton::clicked, this, &CarsTab::onAdd);
  connect(editBtn, &QPushButton::clicked, this, &CarsTab::onEdit);
  connect(delBtn, &QPushButton::clicked, this, &CarsTab::onDelete);
  connect(m_search, &QLineEdit::textChanged, this, &CarsTab::onSearchTextChanged);
  connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &CarsTab::onSelectionChanged);

  auto* findShortcut = new QShortcut(QKeySequence::Find, this);
  connect(findShortcut, &QShortcut::activated, this, [this]{ m_search->setFocus(); });
  auto* addShortcut = new QShortcut(QKeySequence::New, this);
  connect(addShortcut, &QShortcut::activated, this, &CarsTab::onAdd);
  auto* delShortcut = new QShortcut(QKeySequence::Delete, this);
  connect(delShortcut, &QShortcut::activated, this, &CarsTab::onDelete);
  // Double-click row to edit
  connect(m_table, &QTableView::doubleClicked, this, &CarsTab::onEdit);
  refresh();
}

void CarsTab::refresh() {
  m_model->removeRows(0, m_model->rowCount());
  if (!m_service) return;
  for (const auto& v : m_service->vehicles()) {
    if (!v) continue;

    auto* imgItem = new QStandardItem();
    imgItem->setEditable(false);
    imgItem->setTextAlignment(Qt::AlignCenter);
    if (!v->imagePath.isEmpty()) {
      QString full = v->imagePath;
      if (!QFileInfo(full).isAbsolute()) {
        full = QDir(m_service->dataDir()).filePath(v->imagePath);
      }
      QPixmap px(full);
      if (!px.isNull()) {
        imgItem->setIcon(QIcon(px.scaled(64, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
      }
    }

    auto* idItem = new QStandardItem(QString::number(v->id));
    auto* vehicleItem = new QStandardItem(v->brand + " " + v->model);
    auto* yearItem = new QStandardItem(QString::number(v->year));
    auto* rateItem = new QStandardItem(QString::number(v->dailyRate(), 'f', 2));
    idItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    vehicleItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    yearItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rateItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QList<QStandardItem*> row;
    row << imgItem
        << idItem
        << vehicleItem
        << new QStandardItem(v->type())
        << yearItem
        << new QStandardItem(v->plate)
        << rateItem
        << new QStandardItem(v->available ? "true" : "false");
    m_model->appendRow(row);
  }
  m_table->resizeColumnsToContents();
}

void CarsTab::onAdd() {
  try {
    AddCarDialog dlg(m_service, this);
    if (dlg.exec() == QDialog::Accepted) {
      m_service->addVehicle(dlg.buildVehicle());
      refresh();
    }
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void CarsTab::onDelete() {
  try {
    auto proxyIdx = m_table->currentIndex();
    if (!proxyIdx.isValid()) return;
    auto idx = m_proxy->mapToSource(proxyIdx);
    int id = m_model->item(idx.row(), 1)->text().toInt();
    if (QMessageBox::question(this, "Confirm", "Delete car ID " + QString::number(id) + "?") == QMessageBox::Yes) {
      m_service->deleteVehicle(id);
      refresh();
    }
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void CarsTab::onEdit() {
  try {
    auto proxyIdx = m_table->currentIndex();
    if (!proxyIdx.isValid()) return;
    auto idx = m_proxy->mapToSource(proxyIdx);
    int id = m_model->item(idx.row(), 1)->text().toInt();
    if (!m_service) return;
    auto v = m_service->findVehicle(id);
    if (!v) return;

    AddCarDialog dlg(m_service, this);
    dlg.setWindowTitle("Edit Car");
    dlg.loadFromVehicle(v);
    if (dlg.exec() == QDialog::Accepted) {
      auto updated = dlg.buildVehicle();
      // Preserve original ID in case dialog allowed editing
      updated->id = id;
      m_service->updateVehicle(updated);
      refresh();
    }
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void CarsTab::onSelectionChanged() {
  if (!m_details) return;
  auto proxyIdx = m_table->currentIndex();
  if (!proxyIdx.isValid()) {
    for (auto* child : m_details->findChildren<QLabel*>()) child->deleteLater();
    auto* layout = new QVBoxLayout(m_details);
    auto* placeholder = new QLabel("Select a car to view details.", m_details);
    placeholder->setWordWrap(true);
    placeholder->setStyleSheet("color: #777777;");
    layout->addWidget(placeholder);
    layout->addStretch(1);
    return;
  }

  auto idx = m_proxy->mapToSource(proxyIdx);
  QString id      = m_model->item(idx.row(), 1)->text();
  QString vehicle = m_model->item(idx.row(), 2)->text();
  QString type    = m_model->item(idx.row(), 3)->text();
  QString year    = m_model->item(idx.row(), 4)->text();
  QString plate   = m_model->item(idx.row(), 5)->text();
  QString rate    = m_model->item(idx.row(), 6)->text();
  QString avail   = m_model->item(idx.row(), 7)->text();

  delete m_details->layout();

  // Details "card"
  auto* outer = new QVBoxLayout(m_details);
  outer->setContentsMargins(0, 0, 0, 0);
  outer->setSpacing(0);

  auto* card = new QWidget(m_details);
  card->setObjectName("detailsCard");
  card->setStyleSheet(
    "#detailsCard {"
    "  background: #ffffff;"
    "  border: 1px solid #e2e2e2;"
    "  border-radius: 12px;"
    "}"
  );
  auto* layout = new QVBoxLayout(card);
  layout->setContentsMargins(16, 16, 16, 16);
  layout->setSpacing(6);

  auto* title = new QLabel(vehicle, card);
  title->setStyleSheet("font-size: 16px; font-weight: 600;");
  title->setWordWrap(true);
  layout->addWidget(title);

  auto* subtitle = new QLabel("ID " + id + " • " + type, card);
  subtitle->setStyleSheet("color: #666666;");
  layout->addWidget(subtitle);

  auto makeRow = [card, layout](const QString& titleText, const QString& value) {
    auto* lbl = new QLabel("<b>" + titleText + ":</b> " + value, card);
    lbl->setWordWrap(true);
    layout->addWidget(lbl);
  };
  makeRow("Year", year);
  makeRow("Plate", plate);
  makeRow("Daily rate", "$" + rate);
  makeRow("Status", (avail.toLower() == "true") ? "Available" : "Unavailable");

  layout->addStretch(1);
  outer->addWidget(card);
}

void CarsTab::onSearchTextChanged(const QString& text) {
  m_proxy->setFilterFixedString(text);
}
