#pragma once

#include <vector>
#include <optional>
#include <QString>

#include "domain/UserAccount.h"

class FileManager;
class RentalService;

// Handles login, registration, and users.csv persistence.
class AuthService {
public:
  AuthService(FileManager* storage, RentalService* rental);

  void load();
  void save();

  // Creates admin/admin when the app has no admin account yet.
  void ensureDefaultAdmin();

  std::optional<UserAccount> login(const QString& username, const QString& password) const;

  // Registers a normal user and creates the matching Customer row.
  UserAccount registerUser(const QString& username,
                           const QString& password,
                           const QString& fullName,
                           const QString& licenseNo,
                           const QString& phone);

  const std::vector<UserAccount>& users() const { return m_users; }

  static QString hashPasswordHex(const QString& password);

private:
  int nextUserId() const;
  int nextCustomerId() const;
  bool usernameTaken(const QString& username) const;

  FileManager* m_storage{nullptr};
  RentalService* m_rental{nullptr};
  std::vector<UserAccount> m_users;
};
