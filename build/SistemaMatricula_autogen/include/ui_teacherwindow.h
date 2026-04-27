/********************************************************************************
** Form generated from reading UI file 'teacherwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEACHERWINDOW_H
#define UI_TEACHERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TeacherWindow
{
public:
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *tab_2;
    QListWidget *modulesList;
    QListWidget *studentsList;

    void setupUi(QWidget *TeacherWindow)
    {
        if (TeacherWindow->objectName().isEmpty())
            TeacherWindow->setObjectName(QString::fromUtf8("TeacherWindow"));
        TeacherWindow->resize(1226, 990);
        tabWidget = new QTabWidget(TeacherWindow);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(490, 50, 221, 41));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        modulesList = new QListWidget(TeacherWindow);
        modulesList->setObjectName(QString::fromUtf8("modulesList"));
        modulesList->setGeometry(QRect(80, 200, 471, 571));
        studentsList = new QListWidget(TeacherWindow);
        studentsList->setObjectName(QString::fromUtf8("studentsList"));
        studentsList->setGeometry(QRect(640, 200, 481, 571));

        retranslateUi(TeacherWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(TeacherWindow);
    } // setupUi

    void retranslateUi(QWidget *TeacherWindow)
    {
        TeacherWindow->setWindowTitle(QCoreApplication::translate("TeacherWindow", "Form", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("TeacherWindow", "Meus Cursos", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("TeacherWindow", "Solicita\303\247\303\265es", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TeacherWindow: public Ui_TeacherWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEACHERWINDOW_H
