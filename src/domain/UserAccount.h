#pragma once
#include <QString>

// row in users.csv
struct UserAccount {
  int id{0};
  QString username;
  QString password;
  QString role;            // Admin or User
  int customerId{0};       // links to customers.csv for normal users
};
