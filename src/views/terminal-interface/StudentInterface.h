#ifndef STUDENT_INTERFACE_H
#define STUDENT_INTERFACE_H

#include <iostream>
#include <vector>

#include "MenuOptions.h"
#include "../../dao/StudentDAO.cpp"

using namespace std;

void StudentMenu()
{
    StudentDAO dao;
    int option, id;

    do
    {
        crudMenu("ESTUDANTES");
        cin >> option;
        cin.ignore();

        switch (option)
        {
        case 1:
        {
            Student s;
            cout << "Nome: ";
            cin.getline(s.name, 50);
            cout << "Data Nascimento (AAAAMMDD): ";
            cin >> s.birth_date;
            if (dao.create(s))
                cout << "Sucesso! ID: " << s._id << endl;
            break;
        }
        case 2:
        {
            vector<Student> list = dao.readAll();
            if (list.empty())
                cout << "Nenhum registro encontrado.\n";
            else
            {
                for (size_t i = 0; i < list.size(); i++)
                {
                    cout << "ID: " << list[i]._id << " | Nome: " << list[i].name
                         << " | Data Nasc: " << list[i].birth_date << endl;
                }
            }
            break;
        }
        case 3:
        {
            cout << "Digite o ID: ";
            cin >> id;
            Student s = dao.read(id);
            if (s._id != -1)
                cout << "Encontrado: " << s.name << " | Data: " << s.birth_date << endl;
            else
                cout << "Não encontrado." << endl;
            break;
        }
        case 4:
        {
            cout << "ID para atualizar: ";
            cin >> id;
            cin.ignore();
            Student s = dao.read(id);
            if (s._id != -1)
            {
                cout << "Novo nome: ";
                cin.getline(s.name, 50);
                cout << "Nova data: ";
                cin >> s.birth_date;
                cin.ignore();
                if (dao.update(s))
                    cout << "Atualizado com sucesso!\n";
            }
            else
                cout << "Não encontrado.\n";
            break;
        }
        case 5:
        {
            cout << "ID para excluir: ";
            cin >> id;
            if (dao.remove(id))
                cout << "Excluido com sucesso!\n";
            else
                cout << "Erro ao excluir.\n";
            break;
        }
        }
    } while (option != 0);
}

#endif