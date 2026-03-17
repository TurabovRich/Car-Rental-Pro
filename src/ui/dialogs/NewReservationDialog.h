#pragma once
#include <QDialog>
#include <optional>
class RentalService; class QComboBox; class QDateEdit; class QLabel;

class NewReservationDialog : public QDialog {
  Q_OBJECT
public:
  explicit NewReservationDialog(RentalService* service, QWidget* parent = nullptr);
  NewReservationDialog(RentalService* service,
                       std::optional<int> preselectedCustomerId,
                       std::optional<int> preselectedVehicleId,
                       bool lockCustomer,
                       bool lockVehicle,
                       QWidget* parent = nullptr);
  void createReservation();
private slots:
  void recomputePreview();
private:
  RentalService* m_service{nullptr};
  QComboBox* m_customer{};
  QComboBox* m_vehicle{};
  QDateEdit* m_start{};
  QDateEdit* m_end{};
  QLabel* m_preview{};
};
