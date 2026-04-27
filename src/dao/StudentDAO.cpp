#include <iostream>
#include <fstream>
#include <vector>

#include "../models/Student.h"
#include "../utils/FileManager.h"

class StudentDAO {
private:
    std::string path = "C:/ProjetoAEDIII/TP2/data/students.dat";

public:
    StudentDAO() {
        FileManager::initFile(path);
    }

    bool create(Student &s) {
        int lastId = FileManager::getLastId(path);
        s._id = lastId + 1;
        s.removed = false;

        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open()) return false;

        file.write(reinterpret_cast<char*>(&s), sizeof(Student));
        file.close();

        FileManager::updateLastId(path, s._id);
        return true;
    }

    Student read(int id) {
        std::ifstream file(path, std::ios::binary);
        file.seekg(sizeof(int));

        Student s;
        while (file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            if (s._id == id && !s.removed) {
                file.close();
                return s;
            }
        }
        file.close();
        return Student();
    }

    bool update(Student &s_new) {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open()) return false;

        file.seekg(sizeof(int));

        Student s_temp;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char*>(&s_temp), sizeof(Student))) {
            if (s_temp._id == s_new._id && !s_temp.removed) {
                file.seekp(pos);
                file.write(reinterpret_cast<char*>(&s_new), sizeof(Student));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    bool remove(int id) {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        file.seekg(sizeof(int));

        Student s;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            if (s._id == id && !s.removed) {
                s.removed = true;
                file.seekp(pos);
                file.write(reinterpret_cast<char*>(&s), sizeof(Student));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }


    void listAll() {
        std::ifstream file(path, std::ios::binary);
        file.seekg(sizeof(int));

        Student s;
        std::cout << "\n--- LISTA DE ESTUDANTES ATIVOS ---\n";
        while (file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            if (!s.removed) {
                std::cout << "ID: " << s._id << " | Nome: " << s.name 
                          << " | Nascimento: " << s.birth_date << std::endl;
            }
        }
        file.close();
    }
};