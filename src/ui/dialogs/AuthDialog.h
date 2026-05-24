#pragma once

#include <QDialog>

#include "domain/UserAccount.h"

class AuthService;
class QTabWidget;
class QLineEdit;
class QLabel;

class AuthDialog : public QDialog {
  Q_OBJECT
public:
  explicit AuthDialog(AuthService* auth, QWidget* parent = nullptr);

  bool ok() const { return m_ok; }
  const UserAccount& account() const { return m_account; }

private slots:
  void onLogin();
  void onRegister();
  void updateAdminHint();

private:
  AuthService* m_auth{nullptr};
  bool m_ok{false};
  UserAccount m_account;

  QTabWidget* m_tabs{nullptr};
  QLineEdit* m_loginUser{nullptr};
  QLineEdit* m_loginPass{nullptr};
  QLabel* m_loginHint{nullptr};
  QLineEdit* m_regUser{nullptr};
  QLineEdit* m_regPass{nullptr};
  QLineEdit* m_regName{nullptr};
  QLineEdit* m_regLicense{nullptr};
  QLineEdit* m_regPhone{nullptr};
};
