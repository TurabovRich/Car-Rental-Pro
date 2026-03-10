#include "utils/Validation.h"
#include "utils/Exceptions.h"
#include <QRegularExpression>

void Validation::requireNonEmpty(const QString& s, const char* fieldName) {
  if (s.trimmed().isEmpty()) throw ValidationException(std::string(fieldName) + " cannot be empty");
}
void Validation::requirePositive(double x, const char* fieldName) {
  if (!(x > 0.0)) throw ValidationException(std::string(fieldName) + " must be positive");
}
void Validation::requirePhone(const QString& phone) {
  QRegularExpression re(R"(^\+?\d{7,15}$)");
  if (!re.match(phone.trimmed()).hasMatch()) throw ValidationException("Phone number format is invalid");
}
void Validation::requireLicense(const QString& lic) {
  QString t = lic.trimmed();
  if (t.size() < 5 || t.size() > 20) throw ValidationException("License number length is invalid");
}
