#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "../models/Phone.h"
#include "../utils/FileManager.h"

class PhoneDAO
{
private:
    std::string path = "..\\data\\phones.dat";

public:
    PhoneDAO()
    {
        FileManager::initFile(path);
    }

    bool create(Phone &p)
    {

        if (p.country_code <= 0)
        {
            std::cout << "Erro: Codigo do pais e obrigatorio e deve ser maior que zero." << std::endl;
            return false;
        }
        if (p.area_code <= 0)
        {
            std::cout << "Erro: DDD (area_code) e obrigatorio e deve ser maior que zero." << std::endl;
            return false;
        }
        if (strlen(p.number) == 0)
        {
            std::cout << "Erro: O numero do telefone nao pode estar vazio." << std::endl;
            return false;
        }
        if (p.user_id == -1)
        {
            std::cout << "Erro: Este telefone precisa estar vinculado a um User ID valido." << std::endl;
            return false;
        }

        int lastId = FileManager::getLastId(path);
        p._id = lastId + 1;
        p.removed = false;

        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open())
            return false;

        file.write(reinterpret_cast<char *>(&p), sizeof(Phone));
        file.close();

        FileManager::updateLastId(path, p._id);
        return true;
    }

    Phone read(int id)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open())
            return Phone();

        file.seekg(sizeof(int));

        Phone p;
        while (file.read(reinterpret_cast<char *>(&p), sizeof(Phone)))
        {
            if (p._id == id && !p.removed)
            {
                file.close();
                return p;
            }
        }

        file.close();
        return Phone();
    }

    bool update(Phone &p_new)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
            return false;

        file.seekg(sizeof(int));

        Phone p_temp;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char *>(&p_temp), sizeof(Phone)))
        {

            if (p_temp._id == p_new._id && !p_temp.removed)
            {
                file.seekp(pos); // Volta para o início do registro
                file.write(reinterpret_cast<char *>(&p_new), sizeof(Phone));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    bool remove(int phoneId)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
            return false;

        file.seekg(sizeof(int));

        Phone p;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char *>(&p), sizeof(Phone)))
        {
            if (p._id == phoneId && !p.removed)
            {
                p.removed = true;
                file.seekp(pos);
                file.write(reinterpret_cast<char *>(&p), sizeof(Phone));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    void listAll(int userId)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            return;

        file.seekg(sizeof(int));
        Phone p;
        bool found = false;

        while (file.read(reinterpret_cast<char *>(&p), sizeof(Phone)))
        {
            if (p.user_id == userId && !p.removed)
            {
                std::cout << "[Telefone ID: " << p._id << "] "
                          << "+" << p.country_code << " (" << p.area_code << ") "
                          << p.number << std::endl;
                found = true;
            }
        }

        if (!found)
        {
            std::cout << "Nenhum telefone ativo para o usuario " << userId << "." << std::endl;
        }
        file.close();
    }
};