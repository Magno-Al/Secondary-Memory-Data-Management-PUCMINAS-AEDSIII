//========================================================================
// Interface Gráfica 
//========================================================================
// #include <QApplication>
// #include "views/TeacherWindow.h"

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);

//     TeacherWindow w;
//     w.show();

//     return a.exec();
// }

//========================================================================
// Interface Terminal
//========================================================================
#include <iostream>

#include "views/terminal-interface/StudentInterface.h"
#include "views/terminal-interface/ModuleInterface.h"
#include "views/terminal-interface/RegistrationInterface.h"

using namespace std;

int main()
{
    int mainOption;

    do
    {
        mainMenu();
        cin >> mainOption;
        cin.ignore();

        switch (mainOption)
        {
        case 1:
            StudentMenu();
            break;
        case 2:
            ModuleMenu();
            break;
        case 3:
            RegistrationMenu();
            break;
        case 0:
            cout << "Encerrando o sistema..." << endl;
            break;
        default:
            cout << "Opcao invalida. Tente novamente." << endl;
        }
    } while (mainOption != 0);

    return 0;
}