#include "ui/dialogs/AddCarDialog.h"
#include "service/RentalService.h"
#include "domain/Vehicles.h"
#include "utils/Validation.h"
#include "utils/Exceptions.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QPixmap>

AddCarDialog::AddCarDialog(RentalService* service, QWidget* parent) : QDialog(parent), m_service(service) {
  setWindowTitle("Add Car");
  auto* form = new QFormLayout(this);

  m_id = new QSpinBox(this); m_id->setRange(1, 1000000);
  m_type = new QComboBox(this); m_type->addItems({"Sedan","SUV","Truck","Electric"});
  m_brand = new QLineEdit(this);
  m_model = new QLineEdit(this);
  m_year = new QSpinBox(this); m_year->setRange(1980, 2100); m_year->setValue(2022);
  m_plate = new QLineEdit(this);
  m_basePrice = new QDoubleSpinBox(this); m_basePrice->setRange(1.0, 100000.0); m_basePrice->setDecimals(2); m_basePrice->setValue(50.0);
  m_available = new QCheckBox(this); m_available->setChecked(true);

  m_imagePath = new QLineEdit(this);
  m_imagePath->setReadOnly(true);
  auto* browse = new QPushButton("Browse…", this);
  connect(browse, &QPushButton::clicked, this, &AddCarDialog::chooseImage);

  auto* imageRow = new QHBoxLayout();
  imageRow->addWidget(m_imagePath, 1);
  imageRow->addWidget(browse);

  m_imagePreview = new QLabel(this);
  m_imagePreview->setFixedSize(240, 140);
  m_imagePreview->setStyleSheet("QLabel { border: 1px solid #d0d0d0; border-radius: 8px; background: #fafafa; }");
  m_imagePreview->setAlignment(Qt::AlignCenter);
  m_imagePreview->setText("No image");

  form->addRow("ID", m_id);
  form->addRow("Type", m_type);
  form->addRow("Brand", m_brand);
  form->addRow("Model", m_model);
  form->addRow("Year", m_year);
  form->addRow("Plate", m_plate);
  form->addRow("Base price/day", m_basePrice);
  form->addRow("Available", m_available);
  form->addRow("Image", imageRow);
  form->addRow("Preview", m_imagePreview);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
  form->addRow(buttons);
}

void AddCarDialog::loadFromVehicle(const VehiclePtr& v) {
  if (!v) return;
  m_id->setValue(v->id);
  // Do not allow changing ID for existing cars
  m_id->setEnabled(false);

  int typeIndex = m_type->findText(v->type());
  if (typeIndex >= 0) m_type->setCurrentIndex(typeIndex);
  m_brand->setText(v->brand);
  m_model->setText(v->model);
  m_year->setValue(v->year);
  m_plate->setText(v->plate);
  m_basePrice->setValue(v->basePrice);
  m_available->setChecked(v->available);

  m_imagePath->setText(v->imagePath);
  updatePreview();
}

void AddCarDialog::chooseImage() {
  QString file = QFileDialog::getOpenFileName(this, "Choose car image", QString(),
                                              "Images (*.png *.jpg *.jpeg *.webp *.bmp)");
  if (file.isEmpty()) return;
  m_imagePath->setText(file);
  updatePreview();
}

void AddCarDialog::updatePreview() {
  QString file = m_imagePath->text().trimmed();
  if (file.isEmpty()) {
    m_imagePreview->setText("No image");
    m_imagePreview->setPixmap({});
    return;
  }
  QString resolved = file;
  // If we stored a relative path (e.g. images/foo.jpg), resolve it for preview
  if (m_service && !QFileInfo(resolved).isAbsolute()) {
    QString dataDir = m_service->dataDir();
    if (!dataDir.isEmpty()) {
      resolved = QDir(dataDir).filePath(resolved);
    }
  }

  QPixmap px(resolved);
  if (px.isNull()) {
    m_imagePreview->setText("Cannot load image");
    m_imagePreview->setPixmap({});
    return;
  }
  m_imagePreview->setText({});
  m_imagePreview->setPixmap(px.scaled(m_imagePreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

VehiclePtr AddCarDialog::buildVehicle() const {
  Validation::requireNonEmpty(m_brand->text(), "Brand");
  Validation::requireNonEmpty(m_model->text(), "Model");
  Validation::requireNonEmpty(m_plate->text(), "Plate");
  Validation::requirePositive(m_basePrice->value(), "Base price");

  int id = m_id->value();
  QString type = m_type->currentText();
  QString brand = m_brand->text().trimmed();
  QString model = m_model->text().trimmed();
  int year = m_year->value();
  QString plate = m_plate->text().trimmed();
  double basePrice = m_basePrice->value();
  bool avail = m_available->isChecked();

  VehiclePtr v;
  if (type == "Sedan") v = std::make_shared<Sedan>(id, brand, model, year, plate, basePrice, avail);
  else if (type == "SUV") v = std::make_shared<SUV>(id, brand, model, year, plate, basePrice, avail);
  else if (type == "Truck") v = std::make_shared<Truck>(id, brand, model, year, plate, basePrice, avail);
  else v = std::make_shared<Electric>(id, brand, model, year, plate, basePrice, avail);

  QString src = m_imagePath->text().trimmed();
  if (!src.isEmpty()) {
    if (!m_service) throw FileException("Storage not configured (cannot save image)");
    QString dataDir = m_service->dataDir();
    if (dataDir.isEmpty()) throw FileException("Data directory not available");

    QDir dir(dataDir);
    if (!dir.exists("images")) dir.mkpath("images");

    // If user is editing and the image is already stored locally (relative path),
    // do not re-copy; just keep the reference.
    QFileInfo srcInfo(src);
    if (!srcInfo.isAbsolute()) {
      const QString localPath = dir.filePath(src);
      if (QFile::exists(localPath)) {
        v->imagePath = src;
        return v;
      }
    }

    QFileInfo info(src);
    QString ext = info.suffix().isEmpty() ? "png" : info.suffix().toLower();
    QString fileName = plate;
    fileName.replace(" ", "_");
    fileName.replace("/", "_");
    fileName.replace("\\", "_");
    QString rel = "images/" + fileName + "." + ext;
    QString dst = dir.filePath(rel);

    if (QFile::exists(dst)) QFile::remove(dst);
    if (!QFile::copy(src, dst)) throw FileException("Failed to copy image to " + dst.toStdString());

    v->imagePath = rel;
  }

  return v;
}
