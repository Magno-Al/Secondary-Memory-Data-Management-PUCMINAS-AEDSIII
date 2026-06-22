#ifndef SEARCH_INTERFACE_H
#define SEARCH_INTERFACE_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "../../dao/StudentDAO.cpp"
#include "../../dao/ModuleDAO.cpp"
#include "../../utils/pattern/KMP.h"
#include "../../utils/pattern/BoyerMoore.h"

inline std::string toLowerStr(const std::string &s)
{
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return out;
}

inline bool patternMatch(int algo, const std::string &text, const std::string &pattern)
{
    std::string t = toLowerStr(text);
    std::string p = toLowerStr(pattern);
    return (algo == 2) ? BoyerMoore::contains(t, p) : KMP::contains(t, p);
}

inline void SearchMenu()
{
    int entity, algo;
    std::cout << "\n--- PESQUISA POR PADRAO (KMP / Boyer-Moore) ---" << std::endl;
    std::cout << "Pesquisar em:" << std::endl;
    std::cout << "1. Nome do Aluno" << std::endl;
    std::cout << "2. Nome do Curso (Modulo)" << std::endl;
    std::cout << "Escolha: ";
    std::cin >> entity;
    std::cin.ignore();
    if (entity != 1 && entity != 2)
    {
        std::cout << "Opcao invalida." << std::endl;
        return;
    }

    std::cout << "Algoritmo:" << std::endl;
    std::cout << "1. KMP (Knuth-Morris-Pratt)" << std::endl;
    std::cout << "2. Boyer-Moore (bad character)" << std::endl;
    std::cout << "Escolha: ";
    std::cin >> algo;
    std::cin.ignore();
    if (algo != 1 && algo != 2)
    {
        std::cout << "Opcao invalida." << std::endl;
        return;
    }

    std::cout << "Padrao a pesquisar: ";
    std::string pattern;
    std::getline(std::cin, pattern);
    if (pattern.empty())
    {
        std::cout << "Padrao vazio." << std::endl;
        return;
    }

    const std::string algoNome = (algo == 2) ? "Boyer-Moore" : "KMP";
    int encontrados = 0;

    if (entity == 1)
    {
        StudentDAO dao;
        std::cout << "\nAlunos cujo nome contem \"" << pattern << "\" (" << algoNome << "):" << std::endl;
        for (const Student &s : dao.readAll())
        {
            if (patternMatch(algo, s.name, pattern))
            {
                std::cout << "  ID " << s._id << " | " << s.name << std::endl;
                encontrados++;
            }
        }
    }
    else
    {
        ModuleDAO dao;
        std::cout << "\nCursos cujo nome contem \"" << pattern << "\" (" << algoNome << "):" << std::endl;
        for (const Module &m : dao.readAll())
        {
            if (patternMatch(algo, m.name, pattern))
            {
                std::cout << "  ID " << m._id << " | " << m.name << std::endl;
                encontrados++;
            }
        }
    }

    std::cout << "Total encontrado: " << encontrados << std::endl;
}

#endif
