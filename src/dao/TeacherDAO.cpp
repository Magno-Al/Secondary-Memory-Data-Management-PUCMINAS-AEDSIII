#include <iostream>
#include <fstream>

#include "../models/Teacher.h"
#include "../utils/FileManager.h"

class TeacherDAO{
    private:
        std::string path = "..\\data\\teachers.dat";  

    public:
        TeacherDAO(){
            FileManager::initFile(path); 
        }
        
        bool create(Teacher &t){
            int lastId = FileManager::getLastId(path);
            t._id = lastId + 1;
            t.removed = false;

            std::ofstream file(path, std::ios::binary | std::ios::app);
            if(!file.is_open()) return false;

            file.write(reinterpret_cast<char*>(&t), sizeof(Teacher));
            file.close();

            FileManager::updateLastId(path, t._id);
            return false;
        }

        Teacher read(int id){
            std::ifstream file(path, std::ios::binary);
            file.seekg(sizeof(int));

            Teacher t;
            while(file.read(reinterpret_cast<char*>(&t), sizeof(Teacher))){
                if(t._id == id && !t.removed){
                    file.close();
                    return t;
                }
            }

            file.close();
            return Teacher();
        }

        bool update(Teacher &t_new) {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open()) return false;

        file.seekg(sizeof(int));

        Teacher t_temp;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char*>(&t_temp), sizeof(Teacher))) {
            if (t_temp._id == t_new._id && !t_temp.removed) {
                file.seekp(pos);
                file.write(reinterpret_cast<char*>(&t_new), sizeof(Teacher));
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

        Teacher t;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char*>(&t), sizeof(Teacher))) {
            if (t._id == id && !t.removed) {
                t.removed = true;
                file.seekp(pos);
                file.write(reinterpret_cast<char*>(&t), sizeof(Teacher));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }


    void listAll() {
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open()) {
        std::cout << "Erro ao abrir o arquivo de professores." << std::endl;
        return;
        }

        file.seekg(sizeof(int));

        Teacher t;
        std::cout << "\n--- LISTA DE PROFESSORES ATIVOS ---\n";
        while (file.read(reinterpret_cast<char*>(&t), sizeof(Teacher))) {
            if (!t.removed) {
                std::cout << "ID: " << t._id << " | Nome: " << t.name 
                          << " | Nascimento: " << t.birth_date << std::endl << " | Expertise: " << t.expertise << std::endl;
            }
        }
        file.close();
    }
};

