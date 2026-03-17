#pragma once

#include <vector>
#include <optional>
#include <QString>

#include "domain/UserAccount.h"

class FileManager;
class RentalService;

/**
 * Local authentication & registration service.
 *
 * Persistence:
 * - Accounts are stored in `data/users.csv` through `FileManager::{loadUsers,saveUsers}`.
 * - Passwords are NEVER stored; only `SHA-256(password)` hex is persisted.
 *
 * Roles:
 * - "Admin": opens admin panel UI.
 * - "User": opens user UI and is restricted to a single linked customer (`customerId`).
 *
 * Registration:
 * - `registerUser(...)` creates BOTH:
 *   - a new `Customer` (in `customers.csv`) and
 *   - a new `UserAccount` linked to that customer (in `users.csv`).
 */
class AuthService {
public:
  AuthService(FileManager* storage, RentalService* rental);

  void load();
  void save();

  /**
   * Ensures at least one admin exists.
   *
   * If no admin is present, a default `admin/admin` account is created.
   * Intended for demo/dev convenience.
   */
  void ensureDefaultAdmin();

  /**
   * Attempts to authenticate a user.
   *
   * Returns a `UserAccount` on success, `std::nullopt` on failure.
   */
  std::optional<UserAccount> login(const QString& username, const QString& password) const;

  /**
   * Registers a NORMAL user account and creates a linked `Customer`.
   *
   * Throws `ValidationException` if validation fails (duplicate username, invalid license/phone, etc).
   */
  UserAccount registerUser(const QString& username,
                           const QString& password,
                           const QString& fullName,
                           const QString& licenseNo,
                           const QString& phone);

  const std::vector<UserAccount>& users() const { return m_users; }

  /**
   * Deterministic SHA-256 hash helper used for password verification.
   * Returns lowercase hex.
   */
  static QString hashPasswordHex(const QString& password);

private:
  int nextUserId() const;
  int nextCustomerId() const;
  bool usernameTaken(const QString& username) const;

  FileManager* m_storage{nullptr};
  RentalService* m_rental{nullptr};
  std::vector<UserAccount> m_users;
};

