#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <iostream>
#include <vector>

#include "MenuOptions.h"
#include "../../dao/ModuleDAO.cpp"

using namespace std;

void ModuleMenu()
{
    ModuleDAO dao;
    int option, id;

    do
    {
        crudMenu("MODULOS");
        cin >> option;
        cin.ignore();

        switch (option)
        {
        case 1:
        {
            Module m;
            cout << "Nome do Modulo: ";
            cin.getline(m.name, 30);
            cout << "ID do Curso Associado: ";
            cin >> m.course_id;
            cout << "ID do Professor Responsavel: ";
            cin >> m.teacher_id;
            if (dao.create(m))
                cout << "Sucesso! ID: " << m._id << endl;
            break;
        }
        case 2:
        {
            vector<Module> list = dao.readAll();
            if (list.empty())
                cout << "Nenhum registro encontrado.\n";
            else
            {
                for (size_t i = 0; i < list.size(); i++)
                {
                    cout << "ID: " << list[i]._id << " | Nome: " << list[i].name
                         << " | Course ID: " << list[i].course_id
                         << " | Teacher ID: " << list[i].teacher_id << endl;
                }
            }
            break;
        }
        case 3:
        {
            cout << "Digite o ID: ";
            cin >> id;
            Module m = dao.read(id);
            if (m._id != -1)
                cout << "Encontrado: " << m.name << " | Teacher: " << m.teacher_id << endl;
            else
                cout << "Não encontrado." << endl;
            break;
        }
        case 4:
        {
            cout << "ID para atualizar: ";
            cin >> id;
            cin.ignore();
            Module m = dao.read(id);
            if (m._id != -1)
            {
                cout << "Novo nome: ";
                cin.getline(m.name, 30);
                cout << "Novo ID Curso: ";
                cin >> m.course_id;
                cout << "Novo ID Professor: ";
                cin >> m.teacher_id;
                cin.ignore();
                if (dao.update(m))
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