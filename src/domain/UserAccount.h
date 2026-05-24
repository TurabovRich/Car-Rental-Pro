#pragma once
#include <QString>

struct UserAccount {
  int id{0};
  QString username;
  QString passwordHashHex;
  QString role;
  int customerId{0};
};
