#ifndef GUI_LOGIN_WINDOW_H
#define GUI_LOGIN_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QFont>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include <functional>
#include <cstring>

#include "Daos.h"

class LoginWindow : public QWidget
{
public:

    explicit LoginWindow(std::function<bool(const User &)> onLogin, QWidget *parent = nullptr)
        : QWidget(parent), onLogin(std::move(onLogin))
    {
        setWindowTitle("Sistema de Solicitacao de Matricula");
        resize(420, 320);

        auto *title = new QLabel("Sistema de Solicitacao de Matricula", this);
        title->setObjectName("titleLabel");
        title->setAlignment(Qt::AlignCenter);

        orgEdit = new QLineEdit(this);
        userEdit = new QLineEdit(this);
        passEdit = new QLineEdit(this);
        passEdit->setEchoMode(QLineEdit::Password);

        auto *form = new QFormLayout();
        form->addRow("Organizacao:", orgEdit);
        form->addRow("Usuario:", userEdit);
        form->addRow("Senha:", passEdit);

        auto *entrar = new QPushButton("Entrar", this);
        entrar->setDefault(true);

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(title);
        layout->addSpacing(16);
        layout->addLayout(form);
        layout->addSpacing(12);
        layout->addWidget(entrar);
        layout->addStretch();

        connect(entrar, &QPushButton::clicked, this, [this] { tryLogin(); });
        connect(passEdit, &QLineEdit::returnPressed, this, [this] { tryLogin(); });
    }

private:
    void tryLogin()
    {
        char username[50] = {0};
        char password[50] = {0};
        std::strncpy(username, userEdit->text().toUtf8().constData(), sizeof(username) - 1);
        std::strncpy(password, passEdit->text().toUtf8().constData(), sizeof(password) - 1);

        UserDAO dao;
        User u = dao.login(username, password);
        if (u._id == -1)
        {
            QMessageBox::warning(this, "Login", "Usuario ou senha invalidos.");
            return;
        }
        if (onLogin && onLogin(u))
            close();
    }

    std::function<bool(const User &)> onLogin;
    QLineEdit *orgEdit;
    QLineEdit *userEdit;
    QLineEdit *passEdit;
};

#endif
