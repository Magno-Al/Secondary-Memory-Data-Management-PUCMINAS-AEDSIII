#ifndef STUDENT_DAO_CPP
#define STUDENT_DAO_CPP

#include <fstream>
#include "../models/Student.h"
#include "../indexes/btree/BPlusTree.h"
#include "../utils/CrudTemplate.h"
#include "../utils/FileManager.h"
#include "../indexes/btree/BPlusTree.cpp"

class StudentDAO : public CrudTemplate<Student> {
    std::string indexPath;
    BPlusTree index;

public:
    StudentDAO()
        : CrudTemplate<Student>("data/students.dat"),
          indexPath("data/students.idx"),
          index(indexPath) {}

    bool create(Student &s) {
        int lastId = FileManager::getLastId(path);
        s._id = lastId + 1;
        s.removed = false;

        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (!file.is_open()) return false;

        long offset = file.tellp();
        file.write(reinterpret_cast<char*>(&s), sizeof(Student));
        file.close();

        FileManager::updateLastId(path, s._id);
        index.inserir(s._id, offset);
        return true;
    }

    Student read(int id) {
        long offset = index.buscar(id);
        if (offset == -1) {
            Student notFound;
            notFound._id = -1;
            return notFound;
        }
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            Student err;
            err._id = -1;
            return err;
        }
        file.seekg(offset);
        Student s;
        file.read(reinterpret_cast<char*>(&s), sizeof(Student));
        file.close();
        if (s.removed) {
            s._id = -1;
        }
        return s;
    }

    void listAllOrderedById() {
        long leafOffset = index.getPrimeiroFolhaOffset();
        if (leafOffset == -1) {
            std::cout << "\nNenhum estudante cadastrado.\n";
            return;
        }

        std::fstream arqIdx(indexPath, std::ios::binary | std::ios::in);
        std::ifstream arqDat(path, std::ios::binary);

        std::cout << "\n--- ESTUDANTES ORDENADOS POR ID (Arvore B+) ---\n";
        while (leafOffset != -1) {
            NoBPlus no = index.lerNoExterno(arqIdx, leafOffset);
            for (int i = 0; i < no.numChaves; i++) {
                arqDat.seekg(no.ponteirosDados[i]);
                Student s;
                arqDat.read(reinterpret_cast<char*>(&s), sizeof(Student));
                if (!s.removed) {
                    std::cout << "ID: " << s._id
                              << " | Nome: " << s.name
                              << " | Nascimento: " << s.birth_date << "\n";
                }
            }
            leafOffset = no.proximaFolha;
        }

        arqIdx.close();
        arqDat.close();
    }

    // Mesma travessia ordenada da Arvore B+, mas retornando os estudantes
    // num vetor (em vez de imprimir) — usado pela interface grafica.
    std::vector<Student> listOrderedById() {
        std::vector<Student> ordered;
        long leafOffset = index.getPrimeiroFolhaOffset();
        if (leafOffset == -1)
            return ordered;

        std::fstream arqIdx(indexPath, std::ios::binary | std::ios::in);
        std::ifstream arqDat(path, std::ios::binary);

        while (leafOffset != -1) {
            NoBPlus no = index.lerNoExterno(arqIdx, leafOffset);
            for (int i = 0; i < no.numChaves; i++) {
                arqDat.seekg(no.ponteirosDados[i]);
                Student s;
                arqDat.read(reinterpret_cast<char*>(&s), sizeof(Student));
                if (!s.removed)
                    ordered.push_back(s);
            }
            leafOffset = no.proximaFolha;
        }

        arqIdx.close();
        arqDat.close();
        return ordered;
    }
};

#endif // STUDENT_DAO_CPP
