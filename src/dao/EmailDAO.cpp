#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../models/Email.h"
#include "../utils/FileManager.h"

class EmailDAO {
private:
    std::string path = "..\\data\\emails.dat";

public:
    EmailDAO() {
        FileManager::initFile(path);
    }

    bool create(Email &e) {
        int lastId = FileManager::getLastId(path);
        e._id = lastId + 1;
        e.removed = false;

        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open()) return false;

        file.write(reinterpret_cast<char*>(&e), sizeof(Email));
        file.close();

        FileManager::updateLastId(path, e._id);
        return true;
    }

    Email read(int id) {
        std::ifstream file(path, std::ios::binary);
        file.seekg(sizeof(int));

        Email e;
        while (file.read(reinterpret_cast<char*>(&e), sizeof(Email))){
            if (e._id == id && !e.removed) {
                file.close();
                return e;
            }
        }
        file.close();
        return Email();
    }

    bool update(Email &e_new) {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open()) return false;

        file.seekg(sizeof(int));

        Email e_temp;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char*>(&e_temp), sizeof(Email))) {
            if (e_temp._id == e_new._id && !e_temp.removed) {
                file.seekp(pos);
                file.write(reinterpret_cast<char*>(&e_new), sizeof(Email));
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

        Email e;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char*>(&e), sizeof(Email))) {
            if (e._id == id && !e.removed) {
                e.removed = true;
                file.seekp(pos);
                file.write(reinterpret_cast<char*>(&e), sizeof(Email));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }
    
    void listAll(int userId) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return;

        file.seekg(sizeof(int));
        Email e;
        bool found = false;

        std::cout << "\nEmails do Usuario [" << userId << "]:\n";
        while (file.read(reinterpret_cast<char*>(&e), sizeof(Email))) {
            if (e.user_id == userId && !e.removed) {
                std::cout << "- " << e.email_address << " (ID: " << e._id << ")\n";
                found = true;
            }
        }
        if (!found) std::cout << "Nenhum email encontrado.\n";
        file.close();
    }
};