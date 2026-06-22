#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <iostream>
#include <string>
#include "../../dao/UserDAO.cpp"

inline void UserMenu(UserDAO& userDao) {
    int userOption;
    do {
        std::cout << "\n========== MENU DE USUARIOS ==========\n";
        std::cout << "1. Listar Todos os Usuarios\n";
        std::cout << "2. Pesquisar Usuarios por Padrao (KMP / BM)\n";
        std::cout << "0. Voltar ao Menu Principal\n";
        std::cout << "Escolha uma opcao: ";
        std::cin >> userOption;
        std::cin.ignore(); // Limpa o buffer do teclado

        switch (userOption) {
            case 1: {
                userDao.listAll();
                break;
            }
            case 2: {
                int algoritmo;
                std::string termo;

                std::cout << "\n=== PESQUISAR USUARIO POR PADRAO ===\n";
                std::cout << "1. Algoritmo KMP (Knuth-Morris-Pratt)\n";
                std::cout << "2. Algoritmo Boyer-Moore (Bad Character)\n";
                std::cout << "Escolha o algoritmo de busca: ";
                std::cin >> algoritmo;
                std::cin.ignore();

                if (algoritmo != 1 && algoritmo != 2) {
                    std::cout << "Algoritmo invalido!\n";
                    break;
                }

                std::cout << "Digite o trecho do nome que deseja buscar: ";
                std::getline(std::cin, termo);

                // Dispara a busca sequencial descriptografando o arquivo .dat
                userDao.searchByPattern(termo, algoritmo);
                break;
            }
            case 0:
                break;
            default:
                std::cout << "Opcao invalida. Tente novamente.\n";
        }
    } while (userOption != 0);
}

#endif 