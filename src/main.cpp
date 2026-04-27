#include <iostream>
#include <string>
#include "dao/StudentDAO.cpp"
#include "models/Student.h"
#include "dao/TeacherDAO.cpp"
#include "models/Teacher.h"
#include "dao/UserDAO.cpp"
#include "models/User.h"
#include "dao/PhoneDAO.cpp"
#include "models/Phone.h"
#include "dao/EmailDAO.cpp"
#include "models/Email.h"

using namespace std;
void mainMenu() {
    cout << "\n============================" << endl;
    cout << "   SISTEMA ACADEMICO (AEDS)" << endl;
    cout << "============================" << endl;
    cout << "1. Menu de Estudantes" << endl;
    cout << "2. Menu de Professores" << endl;
    cout << "3. Menu de Usuários (Login/Cadastro)" << endl;
    cout << "4. Gerenciar Telefones" << endl;
    cout << "5. Gerenciar Email" << endl;
    cout << "0. Sair" << endl;
    cout << "Escolha: ";
}

void menuStudent(StudentDAO &studentDao){
    int option, id;

    cout << "\n--- GERENCIAR ESTUDANTES ---" << endl;
    cout << "1. Cadastrar Novo Estudante" << endl;
    cout << "2. Listar Todos os Estudantes" << endl;
    cout << "3. Buscar Estudante por ID" << endl;
    cout << "4. Atualizar Dados de Estudante" << endl;
    cout << "5. Excluir Estudante " << endl;
    cout << "0. Sair" << endl;
    cout << "Escolha: ";
    cin >> option;
    cin.ignore();

    switch (option) {
        case 1: { // Cadastrar Estudante
            Student s;
            cout << "Nome: ";
            cin.getline(s.name, 50);
            cout << "Data Nascimento (DDMMYYYY): ";
            cin >> s.birth_date;
            
            if (studentDao.create(s)) cout << "Sucesso! ID do novo estudante: " << s._id << endl;
            break;
        }
        case 2: { // Listar Todos
            studentDao.listAll();
            break;
        }
        case 3: { // Buscar por ID
            cout << "Digite o ID: ";
            cin >> id;
            Student s = studentDao.read(id);
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

            Student s = studentDao.read(id);
            if (s._id != -1) {
                cout << "Dados atuais -> Nome: " << s.name << " | Data: " << s.birth_date << endl;

                cout << "Digite o novo nome: ";
                cin.getline(s.name, 50);
                cout << "Digite a nova data (AAAAMMDD): ";
                cin >> s.birth_date;
                cin.ignore();

                if (studentDao.update(s)) {
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
            if (studentDao.remove(id)) cout << "Estudante excluido com sucesso." << endl;
            else cout << "Erro ao excluir." << endl;
            break;
        }
    }
}

void menuTeacher(TeacherDAO &teacherDao) {
    int option, id;

    cout << "\n--- GERENCIAR PROFESSORES ---" << endl;
    cout << "1. Cadastrar Professor" << endl;
    cout << "2. Listar Professores" << endl;
    cout << "3. Buscar Professor por ID" << endl;
    cout << "4. Atualizar Dados do Professor" << endl;
    cout << "5. Excluir Professor" << endl;
    cout << "0. Voltar" << endl;
    cout << "Escolha: ";
    cin >> option;
    cin.ignore(); 

    switch (option) {
        case 1: { //Cadastrar Professor
            Teacher t;
            cout << "Nome do Professor: "; 
            cin.getline(t.name, 50);
            cout << "Expertise: "; 
            cin.getline(t.expertise, 50);
            cout << "ID do Usuario: ";
            cin >> t.user_id; // Vincula o professor a uma conta de usuário
            
            if (teacherDao.create(t)) {
                cout << "Sucesso! Professor cadastrado com ID: " << t._id << endl;
            } else {
                cout << "Erro ao salvar professor." << endl;
            }
            break;
        }
        case 2: { //Listar todos os Professores
            teacherDao.listAll();
            break;
        }
        case 3: { //Buscar Professor
            cout << "Digite o ID do professor: ";
            cin >> id;

            Teacher t = teacherDao.read(id);

            if (t._id != -1 && !t.removed) {
                cout << "\n[ID: " << t._id << "] " << t.name 
                     << "\nExpertise: " << t.expertise 
                     << "\nUser ID vinculado: " << t.user_id << endl;
            } else {
                cout << "Professor nao encontrado ou removido." << endl;
            }
            break;
        }
        case 4: { //Atualizar dados professor
            cout << "ID do professor para atualizar: ";
            cin >> id;
            cin.ignore();
            Teacher t = teacherDao.read(id);
            if (t._id != -1) {
                cout << "Novo nome (atual: " << t.name << "): ";
                cin.getline(t.name, 50);
                cout << "Nova Expertise (atual: " << t.expertise << "): ";
                cin.getline(t.expertise, 50);
                
                if (teacherDao.update(t)) cout << "Dados atualizados!" << endl;
                else cout << "Falha na atualizacao." << endl;
            } else {
                cout << "Professor nao encontrado." << endl;
            }
            break;
        }
        case 5: { //Remover Professor
            cout << "ID do professor para excluir: ";
            cin >> id;
            if (teacherDao.remove(id)) {
                cout << "Professor removido com sucesso." << endl;
            } else {
                cout << "Erro ao excluir." << endl;
            }
            break;
        }
    }
}

void menuUser(UserDAO &userDao) {
    int option, id;
    
    cout << "\n--- GERENCIAR USUARIOS ---" << endl;
    cout << "1. Cadastrar Novo Usuario" << endl;
    cout << "2. Listar Todos os Usuarios" << endl;
    cout << "3. Buscar Usuario por ID" << endl;
    cout << "4. Excluir Usuario" << endl;
    cout << "0. Sair" << endl;
    cout << "Escolha: ";
    cin >> option;
    cin.ignore(); 

    switch (option) {
        case 1: { //Cadastrar Usuário
            User u;
            cout << "Username: "; 
            cin.getline(u.username, 50);
            cout << "Password: "; 
            cin.getline(u.password, 50);
            cout << "Tipo (admin, student, teacher): "; 
            cin.getline(u.type, 10);
            
            if (userDao.create(u)) {
                cout << "Sucesso! Usuario criado com ID: " << u._id << endl;
                cout << "Guarde este ID para vincular ao Perfil de Aluno/Prof!" << endl;
            } else {
                cout << "Erro ao criar usuario." << endl;
            }
            break;
        }
        case 2: { //Listar Usuários
            userDao.listAll();
            break;
        }
        case 3: { //Buscar Usuário
            cout << "Digite o ID do usuario: ";
            cin >> id;
            User u = userDao.read(id); // Supondo que voce implementou o read no UserDAO
            if (u._id != -1 && !u.removed) {
                cout << "\n[ID: " << u._id << "] Username: " << u.username 
                     << " | Tipo: " << u.type << endl;
            } else {
                cout << "Usuario nao encontrado." << endl;
            }
            break;
        }
        
        case 4: { //Remover Usuário
            cout << "ID do usuario para excluir: ";
            cin >> id;
            if (userDao.remove(id)) {
                cout << "Usuario excluido com sucesso." << endl;
            } else {
                cout << "Erro ao excluir." << endl;
            }
            break;
        }
    }
}

void menuPhone(PhoneDAO &phoneDao) {
    int option, userId, phoneId;

    cout << "\n--- GERENCIAR TELEFONES ---" << endl;
    cout << "1. Adicionar Telefone a um Usuario" << endl;
    cout << "2. Listar Telefones de um Usuario" << endl;
    cout << "3. Remover Telefone Especifico" << endl;
    cout << "0. Sair" << endl;
    cout << "Escolha: ";
    cin >> option;

    switch (option) {
        case 1: {//Cadastrar Telefone
            Phone p;
            cout << "ID do Usuario dono: "; cin >> p.user_id;
            cout << "Codigo Pais (ex 55): "; cin >> p.country_code;
            cout << "DDD (ex 31): "; cin >> p.area_code;
            cin.ignore();
            cout << "Numero: "; cin.getline(p.number, 15);
            
            if (phoneDao.create(p)) cout << "Sucesso! Telefone ID: " << p._id << endl;
            break;
        }
        case 2: {//Listar Telefone
            cout << "ID do Usuario: "; cin >> userId;
            phoneDao.listAll(userId);
            break;
        }
        case 3: {//Remover Telefone
            cout << "ID do Telefone a excluir: "; cin >> phoneId;
            if (phoneDao.remove(phoneId)) cout << "Removido com sucesso!" << endl;
            else cout << "Erro ao remover." << endl;
            break;
        }
    }
}

void menuEmail(EmailDAO &emailDao) {
    int option, userId, emailId;

    cout << "\n--- GERENCIAR EMAILS ---" << endl;
    cout << "1. Adicionar Email a um Usuario" << endl;
    cout << "2. Listar Emails de um Usuario" << endl;
    cout << "3. Remover Email Especifico" << endl;
    cout << "0. Sair" << endl;
    cout << "Escolha: ";
    cin >> option;

    switch (option) {
        case 1: {//Cadastrar Email
            Email e;
            cout << "ID do Usuario dono: "; cin >> e.user_id;
            cin.ignore();
            cout << "Endereco de Email: "; cin.getline(e.email_address, 50);
            
            if (emailDao.create(e)) cout << "Sucesso! Email ID: " << e._id << endl;
            break;
        }
        case 2: {//Listar Email
            cout << "ID do Usuario: "; cin >> userId;
            emailDao.listAll(userId);
            break;
        }
        case 3: {//Remover Email
            cout << "ID do Email a excluir: "; cin >> emailId;
            if (emailDao.remove(emailId)) cout << "Removido com sucesso!" << endl;
            else cout << "Erro ao remover." << endl;
            break;
        }
    }
}

int main() {
    StudentDAO sDao;
    TeacherDAO tDao;
    UserDAO uDao;
    PhoneDAO pDao;
    EmailDAO eDao;

    int option;
    do {
        cout << "\n====================================" << endl;
        cout << "       SISTEMA DE MATRICULA" << endl;
        cout << "====================================" << endl;
        cout << "1. Gerenciar Usuarios" << endl;
        cout << "2. Gerenciar Estudantes" << endl;
        cout << "3. Gerenciar Professores" << endl;
        cout << "4. Gerenciar Telefones" << endl;
        cout << "5. Gerenciar Emails" << endl;
        cout << "0. Sair" << endl;
        cout << "Escolha: ";
        cin >> option;

        switch (option) {
            case 1: menuUser(uDao); break;
            case 2: menuStudent(sDao); break;
            case 3: menuTeacher(tDao); break;
            case 4: menuPhone(pDao); break;
            case 5: menuEmail(eDao); break;
            case 0: cout << "Saindo..." << endl; break;
            default: cout << "Opcao invalida!" << endl;
        }
    } while (option != 0);

    return 0;
}