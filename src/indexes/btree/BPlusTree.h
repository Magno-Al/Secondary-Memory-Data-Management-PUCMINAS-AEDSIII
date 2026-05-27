#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <fstream>
#include <string>


const int ORDEM = 4;

struct NoBPlus {
    bool isFolha;
    int numChaves;
    
    // Armazena as chaves (IDs dos estudantes)
    int chaves[ORDEM - 1];
    
    // Se for NÓ INTERNO: guarda os offsets (posições em bytes) dos nós filhos no arquivo .idx
    long filhos[ORDEM];
    
    // Se for NÓ FOLHA: guarda os offsets (posições em bytes) de onde o Student está no arquivo .dat
    long ponteirosDados[ORDEM - 1];
    

    long proximaFolha;

    // Inicializa o nó
    NoBPlus() {
        isFolha = false;
        numChaves = 0;
        proximaFolha = -1;
        for (int i = 0; i < ORDEM; i++) filhos[i] = -1;
        for (int i = 0; i < ORDEM - 1; i++) {
            chaves[i] = -1;
            ponteirosDados[i] = -1;
        }
    }
};

class BPlusTree {
private:
    std::string arquivoIndiceNome;
    long raizOffset;

  
    void gravarNo(std::fstream& arq, long offset, const NoBPlus& no);
    NoBPlus lerNo(std::fstream& arq, long offset);
    long alocarNovoNo(std::fstream& arq);
    
    
    void inserirNaFolha(NoBPlus& folha, int id, long offsetDados);

    void dividirFolha(std::fstream& arq, long folhaOffset, NoBPlus& folha, int id, long offsetDados, int& chavePromovida, long& novoNoOffset);
    
    void dividirInterno(std::fstream& arq, long internoOffset, NoBPlus& interno, int indiceFilho, int chaveFilhoPromovida, long filhoNovoOffset, int& chavePromovida, long& novoNoOffset);
    
    void inserirNoPai(std::fstream& arq, long noFilhoOffset, int chavePromovida, long noNovoFilhoOffset);

    // Método para inicializar o cabeçalho do arquivo de índices
    void inicializarArquivoIndice();

public:
    // Recebe o caminho do arquivo de índices (ex: students.idx)
    BPlusTree(std::string nomeArquivoIndice);
    
    // Insere um ID e a posição física do Student correspondente no arquivo de dados
    void inserir(int id, long offsetDados);
    
    // Busca por um ID e retorna a posição em bytes do registro no arquivo de dados (.dat)
    long buscar(int id);
    
    // Retorna o offset do primeiro nó folha (o menor ID) para a listagem ordenada do DAO
    long getPrimeiroFolhaOffset();

    // Permite que o StudentDAO leia um nó folha diretamente do arquivo aberto por ele
    NoBPlus lerNoExterno(std::fstream& arq, long offset) {
        return lerNo(arq, offset);
    }
};

#endif 