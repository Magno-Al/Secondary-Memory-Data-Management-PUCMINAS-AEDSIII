#ifndef GUI_APP_H
#define GUI_APP_H

#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <filesystem>
#include <cstring>

#include "Daos.h"
#include "LoginWindow.h"
#include "ManagerWindow.h"
#include "StudentWindow.h"
#include "ProfessorWindow.h"

inline int runGui(int argc, char **argv)
{
    std::filesystem::create_directories("data");
    std::filesystem::create_directories("data-compress");

    QApplication app(argc, argv);

    {
        QFile qss(":/style.qss");
        if (qss.open(QFile::ReadOnly | QFile::Text))
            app.setStyleSheet(QString::fromUtf8(qss.readAll()));
    }

    {
        UserDAO udao;
        if (FileManager::getLastId("data/users.dat") == 0)
        {

            User admin;
            std::strncpy(admin.username, "admin", sizeof(admin.username) - 1);
            std::strncpy(admin.password, "admin", sizeof(admin.password) - 1);
            std::strncpy(admin.type, "gerente", sizeof(admin.type) - 1);
            udao.create(admin);
        }
    }

    auto onLogin = [](const User &u) -> bool {
        std::string type(u.type);

        if (type == "gerente")
        {
            auto *w = new ManagerWindow();
            w->setAttribute(Qt::WA_DeleteOnClose);
            w->show();
            return true;
        }
        if (type == "professor")
        {
            TeacherDAO tdao;
            for (const Teacher &t : tdao.readAll())
            {
                if (t.user_id == u._id)
                {
                    auto *w = new ProfessorWindow(t._id);
                    w->setAttribute(Qt::WA_DeleteOnClose);
                    w->show();
                    return true;
                }
            }
            QMessageBox::warning(nullptr, "Login", "Professor vinculado nao encontrado.");
            return false;
        }
        if (type == "aluno")
        {
            StudentDAO sdao;
            for (const Student &s : sdao.readAll())
            {
                if (s.user_id == u._id)
                {
                    auto *w = new StudentWindow(s._id);
                    w->setAttribute(Qt::WA_DeleteOnClose);
                    w->show();
                    return true;
                }
            }
            QMessageBox::warning(nullptr, "Login", "Aluno vinculado nao encontrado.");
            return false;
        }

        QMessageBox::warning(nullptr, "Login", "Tipo de usuario desconhecido.");
        return false;
    };

    LoginWindow login(onLogin);
    login.show();

    return app.exec();
}

#endif
