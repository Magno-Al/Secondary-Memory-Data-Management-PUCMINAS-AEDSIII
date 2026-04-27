#include "TeacherWindow.h"
#include "ui_teacherwindow.h"

TeacherWindow::TeacherWindow(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::TeacherWindow)
{
    ui->setupUi(this);
}

TeacherWindow::~TeacherWindow()
{
    delete ui;
}