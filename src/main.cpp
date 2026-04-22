#include <iostream>
#include <string>

#include "dao/StudentDAO.cpp"
#include "models/Student.h"

using namespace std;

void menu() {
    cout << "\n--- SISTEMA DE MATRÍCULA ---" << endl;
    cout << "1. Cadastrar Estudante" << endl;
    cout << "2. Listar Todos" << endl;
    cout << "3. Buscar por ID" << endl;
    cout << "4. Atualizar Estudante" << endl;
    cout << "5. Excluir Estudante" << endl;
    cout << "0. Sair" << endl;
    cout << "Escolha: ";
}

int main() {
    StudentDAO dao;
    int option, id;

    do {
        menu();
        cin >> option;
        cin.ignore();

        switch (option) {
            case 1: { // Cadastrar Estudante
                Student s;
                cout << "Nome: ";
                cin.getline(s.name, 50);
                cout << "Data Nascimento (DDMMYYYY): ";
                cin >> s.birth_date;
                
                if (dao.create(s)) cout << "Sucesso! ID do novo estudante: " << s._id << endl;
                break;
            }
            case 2: { // Listar Todos
                dao.listAll();
                break;
            }
            case 3: { // Buscar por ID
                cout << "Digite o ID: ";
                cin >> id;
                Student s = dao.read(id);
                if (s._id != -1 && !s.removed) {
                    cout << "Encontrado: " << s.name << " | Data: " << s.birth_date << endl;
                } else {
                    cout << "Estudante nao encontrado." << endl;
                }
                break;
            }
            case 4: { // Atualizar Estudante
                cout << "Digite o ID do estudante que deseja atualizar: ";
                cin >> id;
                cin.ignore();

                Student s = dao.read(id);
                if (s._id != -1) {
                    cout << "Dados atuais -> Nome: " << s.name << " | Data: " << s.birth_date << endl;

                    cout << "Digite o novo nome: ";
                    cin.getline(s.name, 50);
                    cout << "Digite a nova data (AAAAMMDD): ";
                    cin >> s.birth_date;
                    cin.ignore();

                    if (dao.update(s)) {
                        cout << "Estudante atualizado com sucesso!" << endl;
                    } else {
                        cout << "Erro ao atualizar os dados no arquivo." << endl;
                    }
                } else {
                    cout << "Estudante com ID " << id << " nao encontrado." << endl;
                }
                break;
            }
            case 5: { // Excluir Estudante
                cout << "ID para excluir: ";
                cin >> id;
                if (dao.remove(id)) cout << "Estudante excluido com sucesso." << endl;
                else cout << "Erro ao excluir." << endl;
                break;
            }
        }
    } while (option != 0);

    return 0;
}