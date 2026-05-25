#include "ui/dialogs/AuthDialog.h"

#include "service/AuthService.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

static QWidget* makeCard(QWidget* parent) {
  auto* w = new QWidget(parent);
  w->setObjectName("authCard");
  w->setStyleSheet(
    "#authCard {"
    "  background: #ffffff;"
    "  border: 1px solid #e2e2e2;"
    "  border-radius: 12px;"
    "}"
  );
  return w;
}

AuthDialog::AuthDialog(AuthService* auth, QWidget* parent)
  : QDialog(parent), m_auth(auth) {
  setWindowTitle("Sign in");
  resize(520, 420);

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(10);

  auto* title = new QLabel("CarRentalPro", this);
  title->setStyleSheet("font-size: 18px; font-weight: 700;");
  root->addWidget(title);

  auto* subtitle = new QLabel("Sign in to continue.", this);
  subtitle->setStyleSheet("color: #666666;");
  root->addWidget(subtitle);

  m_tabs = new QTabWidget(this);
  root->addWidget(m_tabs, 1);

  // --- login tab ---
  {
    auto* page = new QWidget(m_tabs);
    auto* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(0, 0, 0, 0);

    auto* card = makeCard(page);
    auto* form = new QFormLayout(card);
    form->setContentsMargins(14, 14, 14, 14);
    form->setSpacing(10);

    m_loginUser = new QLineEdit(card);
    m_loginUser->setPlaceholderText("Username");
    m_loginPass = new QLineEdit(card);
    m_loginPass->setPlaceholderText("Password");
    m_loginPass->setEchoMode(QLineEdit::Password);

    m_loginHint = new QLabel(card);
    m_loginHint->setWordWrap(true);
    m_loginHint->setStyleSheet("color: #666666;");

    form->addRow("Username", m_loginUser);
    form->addRow("Password", m_loginPass);
    form->addRow("", m_loginHint);

    auto* btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    auto* loginBtn = new QPushButton("Login", card);
    loginBtn->setDefault(true);
    btnRow->addWidget(loginBtn);
    form->addRow(btnRow);

    pageLayout->addWidget(card);
    pageLayout->addStretch(1);
    m_tabs->addTab(page, "Login");

    connect(loginBtn, &QPushButton::clicked, this, &AuthDialog::onLogin);
    connect(m_loginUser, &QLineEdit::textChanged, this, &AuthDialog::updateAdminHint);
    updateAdminHint();
  }

  // --- register tab (creates customer + user) ---
  {
    auto* page = new QWidget(m_tabs);
    auto* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(0, 0, 0, 0);

    auto* card = makeCard(page);
    auto* form = new QFormLayout(card);
    form->setContentsMargins(14, 14, 14, 14);
    form->setSpacing(10);

    m_regUser = new QLineEdit(card);
    m_regUser->setPlaceholderText("Choose a username");
    m_regPass = new QLineEdit(card);
    m_regPass->setPlaceholderText("Min 8 chars, Aa1...");
    m_regPass->setEchoMode(QLineEdit::Password);
    m_regName = new QLineEdit(card);
    m_regName->setPlaceholderText("Full name");
    m_regLicense = new QLineEdit(card);
    m_regLicense->setPlaceholderText("Driver license number");
    m_regPhone = new QLineEdit(card);
    m_regPhone->setPlaceholderText("Phone number");

    form->addRow("Username", m_regUser);
    form->addRow("Password", m_regPass);
    form->addRow("Full name", m_regName);
    form->addRow("License", m_regLicense);
    form->addRow("Phone", m_regPhone);
    auto* pwHelp = new QLabel("Password must include uppercase, lowercase, and a number.", card);
    pwHelp->setWordWrap(true);
    pwHelp->setStyleSheet("color: #666666;");
    form->addRow("", pwHelp);

    auto* btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    auto* regBtn = new QPushButton("Create account", card);
    btnRow->addWidget(regBtn);
    form->addRow(btnRow);

    pageLayout->addWidget(card);
    pageLayout->addStretch(1);
    m_tabs->addTab(page, "Register");

    connect(regBtn, &QPushButton::clicked, this, &AuthDialog::onRegister);
  }
}

void AuthDialog::updateAdminHint() {
  if (!m_loginHint) return;
  const QString u = m_loginUser ? m_loginUser->text().trimmed() : QString();
  if (u.compare("admin", Qt::CaseInsensitive) == 0) {
    m_loginHint->setText("Default admin password is also admin.");
  } else {
    m_loginHint->setText("");
  }
}

void AuthDialog::onLogin() {
  if (!m_auth) return;
  try {
    const QString u = m_loginUser->text();
    const QString p = m_loginPass->text();
    UserAccount acc;
    if (!m_auth->login(u, p, acc)) {
      QMessageBox::warning(this, "Login failed", "Invalid username or password.");
      return;
    }
    m_account = acc;
    m_ok = true;
    accept();
  } catch (const std::exception& e) {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void AuthDialog::onRegister() {
  if (!m_auth) return;
  try {
    UserAccount acc = m_auth->registerUser(
      m_regUser->text(),
      m_regPass->text(),
      m_regName->text(),
      m_regLicense->text(),
      m_regPhone->text()
    );
    QMessageBox::information(this, "Account created", "Your account was created. You can login now.");
    m_tabs->setCurrentIndex(0);
    if (m_loginUser) m_loginUser->setText(acc.username);
    if (m_loginPass) m_loginPass->clear();
    if (m_regPass) m_regPass->clear();
  } catch (const std::exception& e) {
    QMessageBox::warning(this, "Registration failed", e.what());
  }
}

