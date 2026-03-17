#pragma once

#include <QString>

/**
 * Represents an application login account persisted in `data/users.csv`.
 *
 * Notes:
 * - `passwordHashHex` stores a SHA-256 hex digest of the password (never the raw password).
 * - Normal users are linked to exactly one `Customer` through `customerId`.
 * - Admin accounts do not need a linked customer (`customerId == 0`).
 */
struct UserAccount {
  int id{0};
  QString username;
  QString passwordHashHex; // SHA-256 hex digest of password
  QString role;            // "Admin" or "User"
  int customerId{0};       // for role=="User", links to Customer.id
};

