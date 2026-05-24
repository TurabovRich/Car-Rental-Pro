#pragma once

class Invoice {
public:
  int id{0};
  int reservationId{0};
  double subtotal{0.0};
  double vat{0.0};
  double lateFee{0.0};
  double damageFee{0.0};
  double total{0.0};

  static constexpr double VAT_RATE = 0.12;

  Invoice() = default;
  Invoice(int id_, int reservationId_, double subtotal_, double lateFee_, double damageFee_)
    : id(id_), reservationId(reservationId_), subtotal(subtotal_), lateFee(lateFee_), damageFee(damageFee_) {
      vat = subtotal * VAT_RATE;
      total = subtotal + vat + lateFee + damageFee;
    }
};
