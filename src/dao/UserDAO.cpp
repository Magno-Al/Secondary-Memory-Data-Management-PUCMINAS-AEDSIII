#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../models/User.h"
#include "../utils/FileManager.h"

class UserDAO
{
private:
    std::string path = "..\\data\\users.dat";

public:
    UserDAO()
    {
        FileManager::initFile(path);
    }

    bool create(User &u)
    {
        int LastId = FileManager::getLastId(path);
        u._id = LastId = 1;
        u.removed = false;

        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open())
            return false;

        file.write(reinterpret_cast<char *>(&u), sizeof(User));
        file.close();

        FileManager::updateLastId(path, u._id);
        return true;
    }

    User login(const char *username, const char *password)
    {
        std::ifstream file(path, std::ios::binary);
        file.seekg(sizeof(int));

        User u;
        while (file.read(reinterpret_cast<char *>(&u), sizeof(User)))
        {
            if (!u.removed &&
                strcmp(u.username, username) == 0 &&
                strcmp(u.password, password) == 0)
            {
                file.close();
                return u;
            }
        }
        file.close();
        return User(); // Retorna user com ID -1 se falhar
    }

    User read(int id)
    {
        std::ifstream file(path, std::ios::binary);
        file.seekg(sizeof(int));

        User u;
        while (file.read(reinterpret_cast<char *>(&u), sizeof(User)))
        {
            if (u._id == id && !u.removed)
            {
                file.close();
                return u;
            }
        }
        file.close();
        return User();
    }

    bool update(User &u_new)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
            return false;

        file.seekg(sizeof(int));

        User u_temp;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char *>(&u_temp), sizeof(User)))
        {
            if (u_temp._id == u_new._id && !u_temp.removed)
            {
                file.seekp(pos);
                file.write(reinterpret_cast<char *>(&u_new), sizeof(User));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    bool remove(int id)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        file.seekg(sizeof(int));

        User u;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char *>(&u), sizeof(User)))
        {
            if (u._id == id && !u.removed)
            {
                u.removed = true;
                file.seekp(pos);
                file.write(reinterpret_cast<char *>(&u), sizeof(User));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    void listAll()
    {
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open())
        {
            std::cout << "Erro ao abrir o arquivo de usuários." << std::endl;
            return;
        }

        file.seekg(sizeof(int));

        User u;
        std::cout << "\n--- LISTA DE USUARIOS ATIVOS ---\n";

        while (file.read(reinterpret_cast<char *>(&u), sizeof(User)))
        {
            if (!u.removed)
            {
                std::cout << "ID: " << u._id
                          << " | Usuário: " << u.username
                          << " | Tipo: [" << u.type << "]" << std::endl;
            }
        }

        file.close();
    }
};