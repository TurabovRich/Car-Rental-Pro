#pragma once
#include <QDialog>
#include "domain/Vehicle.h"
class RentalService;
class QLineEdit; class QComboBox; class QSpinBox; class QDoubleSpinBox; class QCheckBox; class QLabel;

class AddCarDialog : public QDialog {
  Q_OBJECT
public:
  explicit AddCarDialog(RentalService* service, QWidget* parent = nullptr);
  VehiclePtr buildVehicle() const;
  void loadFromVehicle(const VehiclePtr& v);
private:
  void chooseImage();
  void updatePreview();

  RentalService* m_service{nullptr};
  QSpinBox* m_id{};
  QComboBox* m_type{};
  QLineEdit* m_brand{};
  QLineEdit* m_model{};
  QSpinBox* m_year{};
  QLineEdit* m_plate{};
  QDoubleSpinBox* m_basePrice{};
  QCheckBox* m_available{};
  QLineEdit* m_imagePath{};
  QLabel* m_imagePreview{};
};
