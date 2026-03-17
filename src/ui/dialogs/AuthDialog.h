#pragma once

#include <QDialog>
#include <optional>

#include "domain/UserAccount.h"

class AuthService;
class QTabWidget;
class QLineEdit;
class QLabel;
class QPushButton;

class AuthDialog : public QDialog {
  Q_OBJECT
public:
  explicit AuthDialog(AuthService* auth, QWidget* parent = nullptr);

  std::optional<UserAccount> authenticated() const { return m_authenticated; }

private slots:
  void onLogin();
  void onRegister();
  void updateAdminHint();

private:
  AuthService* m_auth{nullptr};
  std::optional<UserAccount> m_authenticated;

  QTabWidget* m_tabs{nullptr};

  // Login tab
  QLineEdit* m_loginUser{nullptr};
  QLineEdit* m_loginPass{nullptr};
  QLabel* m_loginHint{nullptr};

  // Register tab (normal users only)
  QLineEdit* m_regUser{nullptr};
  QLineEdit* m_regPass{nullptr};
  QLineEdit* m_regName{nullptr};
  QLineEdit* m_regLicense{nullptr};
  QLineEdit* m_regPhone{nullptr};
};

