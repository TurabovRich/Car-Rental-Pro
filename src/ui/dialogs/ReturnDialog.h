#pragma once
#include <QDialog>
class RentalService; class QComboBox; class QDoubleSpinBox; class QLabel;
class QDateEdit;

class ReturnDialog : public QDialog {
  Q_OBJECT
public:
  explicit ReturnDialog(RentalService* service, int preselectReservationId = -1, QWidget* parent = nullptr);
private slots:
  void onProcess();
  void recomputePreview();
private:
  RentalService* m_service{nullptr};
  QComboBox* m_reservation{};
  QDateEdit* m_returnDate{};
  QLabel* m_lateDays{};
  QDoubleSpinBox* m_damageFee{};
  QLabel* m_details{};
  QLabel* m_result{};
};
