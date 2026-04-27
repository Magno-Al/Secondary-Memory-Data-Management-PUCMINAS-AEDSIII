#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <fstream>
#include <string>

class FileManager {
public:
    static void initFile(const std::string& path) {
        std::ifstream test(path, std::ios::binary);
        if (!test.good()) {
            std::ofstream out(path, std::ios::binary);
            if (!out) {
                std::cerr << "ERRO: Nao foi possivel criar o arquivo em: " << path << std::endl;
                return;
            }
            int initialId = 0;
            out.write(reinterpret_cast<char*>(&initialId), sizeof(int));
            out.close();
            std::cout << "Arquivo inicializado com sucesso: " << path << std::endl;
        }
    }

    static int getLastId(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        int id = 0;
        if (in.is_open()) {
            in.read(reinterpret_cast<char*>(&id), sizeof(int));
            in.close();
        }
        return id;
    }

    static void updateLastId(const std::string& path, int newId) {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (file.is_open()) {
            file.seekp(0, std::ios::beg);
            file.write(reinterpret_cast<char*>(&newId), sizeof(int));
            file.close();
        }
    }
};

#endif