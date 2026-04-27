#ifndef CRUD_TEMPLATE_H
#define CRUD_TEMPLATE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "FileManager.h"

template <class T>
class CrudTemplate
{
protected:
    std::string path;

public:
    CrudTemplate(std::string filePath) : path(filePath)
    {
        FileManager::initFile(path);
    }

    bool create(T &entity)
    {
        int lastId = FileManager::getLastId(path);
        entity._id = lastId + 1;
        entity.removed = false;

        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open())
            return false;

        file.write(reinterpret_cast<char *>(&entity), sizeof(T));
        file.close();

        FileManager::updateLastId(path, entity._id);
        return true;
    }

    T read(int id)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            T erro;
            erro._id = -1;
            return erro;
        }

        file.seekg(sizeof(int));
        T entity;
        while (file.read(reinterpret_cast<char *>(&entity), sizeof(T)))
        {
            if (entity._id == id && !entity.removed)
            {
                file.close();
                return entity;
            }
        }
        file.close();

        T notMatch;
        notMatch._id = -1;
        return notMatch;
    }

    std::vector<T> readAll()
    {
        std::vector<T> list;
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open())
        {
            return list;
        }

        file.seekg(sizeof(int));

        T entity;
        while (file.read(reinterpret_cast<char *>(&entity), sizeof(T)))
        {
            if (!entity.removed)
            {
                list.push_back(entity);
            }
        }

        file.close();
        return list;
    }

    bool update(T &newEntity)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open())
            return false;

        file.seekg(sizeof(int));
        T entity_temp;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char *>(&entity_temp), sizeof(T)))
        {
            if (entity_temp._id == newEntity._id && !entity_temp.removed)
            {
                file.seekp(pos);
                file.write(reinterpret_cast<char *>(&newEntity), sizeof(T));
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
        if (!file.is_open())
            return false;

        file.seekg(sizeof(int));
        T entity;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char *>(&entity), sizeof(T)))
        {
            if (entity._id == id && !entity.removed)
            {
                entity.removed = true;
                file.seekp(pos);
                file.write(reinterpret_cast<char *>(&entity), sizeof(T));
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }
};

#endif