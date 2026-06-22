// ========================================================================
// Interface Gráfica (Qt6 / QtWidgets)
// Para usar a GUI: descomente este bloco e COMENTE o bloco de terminal abaixo.
// Build da GUI: cmake -S . -B build && cmake --build build
// ========================================================================
// #include "views/gui/GuiApp.h"
//
// int main(int argc, char **argv)
// {
//     return runGui(argc, argv);
// }

//========================================================================
// Interface Terminal
//========================================================================
#include <iostream>
#include <filesystem>

#include "views/terminal-interface/StudentInterface.h"
#include "views/terminal-interface/ModuleInterface.h"
#include "views/terminal-interface/RegistrationInterface.h"
#include "views/terminal-interface/BackupInterface.h"
#include "views/terminal-interface/SearchInterface.h"

using namespace std;

int main()
{
    filesystem::create_directories("data");          // arquivos de dados do sistema
    filesystem::create_directories("data-compress"); // arquivos compactados (backups)

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
        case 4:
            BackupMenu();
            break;
        case 5:
            SearchMenu();
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
