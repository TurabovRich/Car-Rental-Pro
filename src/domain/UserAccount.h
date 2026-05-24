#pragma once

#include <QString>

// One login row in users.csv. Normal users point at a customer via customerId.
struct UserAccount {
  int id{0};
  QString username;
  QString passwordHashHex; // SHA-256 hex, never the raw password
  QString role;            // "Admin" or "User"
  int customerId{0};       // 0 for admin; otherwise links to Customer.id
};
