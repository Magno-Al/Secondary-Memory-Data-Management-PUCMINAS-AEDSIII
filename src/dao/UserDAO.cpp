#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

#include "../models/User.h"
#include "../utils/FileManager.h"

class UserDAO
{
private:
    std::string path = "data/users.dat";
    const char CHAVE_SECRETA = 'K';

    void aplicarXOR(char *dado, size_t tamanho)
    {
        for (size_t i = 0; i < tamanho && dado[i] != '\0'; i++)
        {
            dado[i] = dado[i] ^ CHAVE_SECRETA;
        }
    }

    void processarCriptografia(User &u)
    {
        aplicarXOR(u.username, sizeof(u.username));
        aplicarXOR(u.password, sizeof(u.password));
    }

    // --- ALGORITMO KMP ---
    void computeLPS(const std::string& pattern, std::vector<int>& lps) {
        int len = 0;
        lps[0] = 0;
        int i = 1;
        while (i < pattern.length()) {
            if (pattern[i] == pattern[len]) {
                len++;
                lps[i] = len;
                i++;
            } else {
                if (len != 0) len = lps[len - 1];
                else { lps[i] = 0; i++; }
            }
        }
    }

    bool matchKMP(const std::string& text, const std::string& pattern) {
        if (pattern.empty()) return true;
        int M = pattern.length();
        int N = text.length();
        std::vector<int> lps(M);
        computeLPS(pattern, lps);
        int i = 0, j = 0;
        while (i < N) {
            if (pattern[j] == text[i]) { j++; i++; }
            if (j == M) return true; 
            else if (i < N && pattern[j] != text[i]) {
                if (j != 0) j = lps[j - 1];
                else i = i + 1;
            }
        }
        return false;
    }

    // --- ALGORITMO BOYER-MOORE (Bad Character) ---
    bool matchBoyerMoore(const std::string& text, const std::string& pattern) {
        int m = pattern.length();
        int n = text.length();
        if (m > n) return false;

        int badchar[256];
        for (int i = 0; i < 256; i++) badchar[i] = -1;
        for (int i = 0; i < m; i++) badchar[(int)pattern[i]] = i;

        int s = 0;
        while (s <= (n - m)) {
            int j = m - 1;
            while (j >= 0 && pattern[j] == text[s + j]) j--;
            if (j < 0) return true; 
            else s += std::max(1, j - badchar[(int)text[s + j]]);
        }
        return false;
    } 

public:
    UserDAO()
    {
        FileManager::initFile(path);
    }

    bool create(User &u)
    {
        int LastId = FileManager::getLastId(path);
        u._id = LastId + 1;
        u.removed = false;

        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open())
            return false;

        processarCriptografia(u);

        file.write(reinterpret_cast<char *>(&u), sizeof(User));
        file.close();

        processarCriptografia(u);

        FileManager::updateLastId(path, u._id);
        return true;
    }

    User login(const char *username, const char *password)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            return User();
        file.seekg(sizeof(int));

        User u;
        while (file.read(reinterpret_cast<char *>(&u), sizeof(User)))
        {
            processarCriptografia(u);

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
        if (!file.is_open())
            return User();
        file.seekg(sizeof(int));

        User u;
        while (file.read(reinterpret_cast<char *>(&u), sizeof(User)))
        {
            processarCriptografia(u); // Descriptografa para ler os dados reais

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
            processarCriptografia(u_temp); // Descriptografa para conseguir comparar o ID e o status

            if (u_temp._id == u_new._id && !u_temp.removed)
            {
                file.seekp(pos);

                processarCriptografia(u_new); // Criptografa os novos dados antes de salvar por cima do registro

                file.write(reinterpret_cast<char *>(&u_new), sizeof(User));

                processarCriptografia(u_new); // Descriptografa u_new de volta para não estragar a variável na memória

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

        User u;
        std::streampos pos;

        while (pos = file.tellg(), file.read(reinterpret_cast<char *>(&u), sizeof(User)))
        {
            processarCriptografia(u);

            if (u._id == id && !u.removed)
            {
                u.removed = true;
                file.seekp(pos);

                processarCriptografia(u); // Re-criptografa antes de salvar o estado atualizado (u.removed = true)

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
            processarCriptografia(u); // Descriptografa para exibir legível na tela

            if (!u.removed)
            {
                std::cout << "ID: " << u._id
                          << " | Usuário: " << u.username
                          << " | Tipo: [" << u.type << "]" << std::endl;
            }
        }

        file.close();
    }

    void searchByPattern(const std::string& pattern, int algorithmOption) {
        std::ifstream file(path, std::ios::binary);
        
        if (!file.is_open()) {
            std::cout << "Erro ao abrir o arquivo para pesquisa." << std::endl;
            return;
        }

        file.seekg(sizeof(int)); // Pula o cabeçalho de LastId

        User u;
        bool foundAny = false;
        std::cout << "\n--- RESULTADOS DA PESQUISA ---" << std::endl;

        while (file.read(reinterpret_cast<char*>(&u), sizeof(User))) {
            
            processarCriptografia(u); // Descriptografa temporariamente para conseguir ler o nome real

            if (!u.removed) {
                std::string textName(u.username);
                bool isMatch = false;

                if (algorithmOption == 1) {
                    isMatch = matchKMP(textName, pattern);
                } else if (algorithmOption == 2) {
                    isMatch = matchBoyerMoore(textName, pattern);
                }

                if (isMatch) {
                    std::cout << "ID: " << u._id 
                              << " | Usuário: " << u.username 
                              << " | Tipo: [" << u.type << "]" << std::endl;
                    foundAny = true;
                }
            }
        }

        if (!foundAny) {
            std::cout << "Nenhum usuário encontrado com o padrão '" << pattern << "'." << std::endl;
        }
        file.close();
    }
    
};