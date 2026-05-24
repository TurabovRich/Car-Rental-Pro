#include "service/AuthService.h"

#include "service/RentalService.h"
#include "storage/FileManager.h"
#include "utils/Exceptions.h"
#include "utils/Validation.h"

#include <QCryptographicHash>

AuthService::AuthService(FileManager* storage, RentalService* rental)
  : m_storage(storage), m_rental(rental) {}

void AuthService::load() {
  if (!m_storage) throw FileException("Storage not configured");
  m_storage->loadUsers(m_users);
}

void AuthService::save() {
  if (!m_storage) throw FileException("Storage not configured");
  m_storage->saveUsers(m_users);
}

QString AuthService::hashPasswordHex(const QString& password) {
  auto bytes = password.toUtf8();
  return QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex();
}

int AuthService::nextUserId() const {
  int maxId = 0;
  for (const auto& u : m_users) maxId = std::max(maxId, u.id);
  return maxId + 1;
}

int AuthService::nextCustomerId() const {
  if (!m_rental) return 1;
  int maxId = 0;
  for (const auto& c : m_rental->customers()) maxId = std::max(maxId, c.id);
  return maxId + 1;
}

bool AuthService::usernameTaken(const QString& username) const {
  for (const auto& u : m_users) {
    if (u.username.compare(username, Qt::CaseInsensitive) == 0) return true;
  }
  return false;
}

void AuthService::ensureDefaultAdmin() {
  // First-run helper so the app is usable without manual CSV editing.
  bool hasAdmin = false;
  for (const auto& u : m_users) {
    if (u.role.compare("Admin", Qt::CaseInsensitive) == 0) { hasAdmin = true; break; }
  }
  if (hasAdmin) return;

  UserAccount admin;
  admin.id = nextUserId();
  admin.username = "admin";
  admin.passwordHashHex = hashPasswordHex("admin");
  admin.role = "Admin";
  admin.customerId = 0;
  m_users.push_back(admin);
  save();
}

std::optional<UserAccount> AuthService::login(const QString& username, const QString& password) const {
  QString u = username.trimmed();
  if (u.isEmpty()) return std::nullopt;
  QString want = hashPasswordHex(password);
  for (const auto& acc : m_users) {
    if (acc.username.compare(u, Qt::CaseInsensitive) == 0 &&
        acc.passwordHashHex.compare(want, Qt::CaseInsensitive) == 0) {
      return acc;
    }
  }
  return std::nullopt;
}

UserAccount AuthService::registerUser(const QString& username,
                                      const QString& password,
                                      const QString& fullName,
                                      const QString& licenseNo,
                                      const QString& phone) {
  if (!m_rental) throw ValidationException("Rental service not configured");
  QString u = username.trimmed();
  Validation::requireNonEmpty(u, "Username");
  Validation::requirePassword(password);
  Validation::requireNonEmpty(fullName, "Full name");
  Validation::requireLicense(licenseNo);
  Validation::requirePhone(phone);
  if (usernameTaken(u)) throw ValidationException("Username already taken");

  // Every user account needs a customer profile for bookings.
  int customerId = nextCustomerId();
  m_rental->addCustomer(Customer(customerId, fullName.trimmed(), licenseNo.trimmed(), phone.trimmed()));

  UserAccount acc;
  acc.id = nextUserId();
  acc.username = u;
  acc.passwordHashHex = hashPasswordHex(password);
  acc.role = "User";
  acc.customerId = customerId;
  m_users.push_back(acc);
  save();
  return acc;
}

