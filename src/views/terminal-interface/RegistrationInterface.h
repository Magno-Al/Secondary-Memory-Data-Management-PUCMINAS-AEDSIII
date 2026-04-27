#ifndef REGISTRATION_INTERFACE_H
#define REGISTRATION_INTERFACE_H

#include <iostream>
#include <vector>

#include "MenuOptions.h"
#include "../../dao/RegistrationDAO.cpp"

using namespace std;

void RegistrationMenu()
{
    RegistrationDAO dao;
    int option, id;

    do
    {
        crudMenu("MATRICULAS");
        cin >> option;
        cin.ignore();

        switch (option)
        {
        case 1:
        {
            Registration r;
            cout << "ID do Estudante: ";
            cin >> r.student_id;
            cout << "ID do Modulo: ";
            cin >> r.module_id;
            cout << "Data (AAAAMMDD): ";
            cin >> r.date;

            if (dao.create(r))
                cout << "Sucesso! ID: " << r._id << endl;
            break;
        }
        case 2:
        {
            vector<Registration> list = dao.readAll();
            if (list.empty())
                cout << "Nenhum registro encontrado.\n";
            else
            {
                for (size_t i = 0; i < list.size(); i++)
                {
                    string status = "Pendente";
                    if (list[i].revised)
                    {
                        status = list[i].approved ? "Aprovada" : "Recusada";
                    }
                    cout << "ID: " << list[i]._id << " | Student: " << list[i].student_id
                         << " | Module: " << list[i].module_id
                         << " | Status: " << status << endl;
                }
            }
            break;
        }
        case 3:
        {
            cout << "Digite o ID: ";
            cin >> id;
            Registration r = dao.read(id);
            if (r._id != -1)
                cout << "Encontrado: Student " << r.student_id << " no Modulo " << r.module_id << endl;
            else
                cout << "Não encontrado." << endl;
            break;
        }
        case 4:
        {
            cout << "ID para atualizar: ";
            cin >> id;
            cin.ignore();
            Registration r = dao.read(id);
            if (r._id != -1)
            {
                cout << "Novo ID Estudante: ";
                cin >> r.student_id;
                cout << "Novo ID Modulo: ";
                cin >> r.module_id;
                cout << "Nova Data: ";
                cin >> r.date;
                cin.ignore();

                int revised, approved;
                cout << "Revisada? (1 para sim, 0 para nao): ";
                cin >> revised;
                r.revised = (revised == 1);

                if (r.revised)
                {
                    cout << "Aprovada? (1 para sim, 0 para nao): ";
                    cin >> approved;
                    r.approved = (approved == 1);
                }
                else
                {
                    r.approved = false;
                }

                if (dao.update(r))
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
        case 6:
        {
            int modId;
            cout << "Digite o ID do Modulo para listar alunos: ";
            cin >> modId;

            vector<Registration> regs = dao.findByModule(modId);
            if (regs.empty())
            {
                cout << "Nenhuma matricula encontrada para este modulo.\n";
            }
            else
            {
                cout << "\n--- Alunos Inscritos no Modulo " << modId << " ---\n";
                for (const auto &r : regs)
                {
                    cout << "ID Matricula: " << r._id << " | ID Estudante: " << r.student_id << endl;
                }
            }
            break;
        }
        }
    } while (option != 0);
}

#endif