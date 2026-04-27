#ifndef MENU_OPTIONS_H
#define MENU_OPTIONS_H

#include <iostream>
#include <string>

void mainMenu()
{
    std::cout << "\n===================================" << std::endl;
    std::cout << "        SISTEMA DE GESTAO          " << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "1. Gerenciar Estudantes" << std::endl;
    std::cout << "2. Gerenciar Modulos (Cursos)" << std::endl;
    std::cout << "3. Gerenciar Matriculas" << std::endl;
    std::cout << "0. Sair do Programa" << std::endl;
    std::cout << "Escolha: ";
    std::cout << "\n";
}

void crudMenu(const std::string &title)
{
    std::cout << "\n--- " << title << " ---" << std::endl;
    std::cout << "1. Cadastrar" << std::endl;
    std::cout << "2. Listar Todos" << std::endl;
    std::cout << "3. Buscar por ID" << std::endl;
    std::cout << "4. Atualizar" << std::endl;
    std::cout << "5. Excluir" << std::endl;
    std::cout << "6. Listar alunos matriculados em um modulo (HASH)" << std::endl;
    std::cout << "0. Voltar ao Menu Principal" << std::endl;
    std::cout << "Escolha: ";
}

#endif