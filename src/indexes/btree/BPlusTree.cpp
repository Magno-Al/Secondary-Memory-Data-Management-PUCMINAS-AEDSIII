#include "BPlusTree.h"
#include <algorithm>

// Construtor: Inicializa o nome do arquivo e gerencia o cabeçalho do índice
BPlusTree::BPlusTree(std::string nomeArquivoIndice) {
    this->arquivoIndiceNome = nomeArquivoIndice;
    this->raizOffset = -1;
    inicializarArquivoIndice();
}

// Inicializa o arquivo de índice se ele não existir, lendo/escrevendo o offset da raiz no início (bytes 0 a 7)
void BPlusTree::inicializarArquivoIndice() {
    std::fstream arq(arquivoIndiceNome, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!arq.is_open()) {
        // Se o arquivo não existe, cria um novo
        std::ofstream novoArq(arquivoIndiceNome, std::ios::binary);
        long raizInicial = -1;
        novoArq.write(reinterpret_cast<char*>(&raizInicial), sizeof(long));
        novoArq.close();
        this->raizOffset = -1;
    } else {
        // Se já existe, lê a posição da raiz que está salva nos primeiros bytes
        arq.seekg(0, std::ios::beg);
        arq.read(reinterpret_cast<char*>(&this->raizOffset), sizeof(long));
        arq.close();
    }
}

// Escreve um nó em uma posição física (offset) específica do arquivo de índices
void BPlusTree::gravarNo(std::fstream& arq, long offset, const NoBPlus& no) {
    arq.seekp(offset, std::ios::beg);
    arq.write(reinterpret_cast<const char*>(&no), sizeof(NoBPlus));
}

// Lê um nó de uma posição física (offset) específica do arquivo de índices
NoBPlus BPlusTree::lerNo(std::fstream& arq, long offset) {
    NoBPlus no;
    if (offset == -1) return no;
    arq.seekg(offset, std::ios::beg);
    arq.read(reinterpret_cast<char*>(&no), sizeof(NoBPlus));
    return no;
}

// Aloca espaço no fim do arquivo para um novo nó e retorna o seu offset
long BPlusTree::alocarNovoNo(std::fstream& arq) {
    arq.seekp(0, std::ios::end);
    return arq.tellp();
}

// Retorna o offset do primeiro nó folha da árvore (o mais à esquerda)
long BPlusTree::getPrimeiroFolhaOffset() {
    if (this->raizOffset == -1) return -1;

    std::fstream arq(arquivoIndiceNome, std::ios::binary | std::ios::in);
    long atualOffset = this->raizOffset;
    NoBPlus no = lerNo(arq, atualOffset);

    // Desce sempre pelo primeiro filho (filhos[0]) até interceptar uma folha
    while (!no.isFolha && atualOffset != -1) {
        atualOffset = no.filhos[0];
        no = lerNo(arq, atualOffset);
    }
    arq.close();
    return atualOffset;
}

// Busca por um ID na árvore e retorna o offset dele no arquivo de dados (.dat)
long BPlusTree::buscar(int id) {
    if (this->raizOffset == -1) return -1; // Árvore vazia

    std::fstream arq(arquivoIndiceNome, std::ios::binary | std::ios::in);
    long atualOffset = this->raizOffset;

    while (atualOffset != -1) {
        NoBPlus no = lerNo(arq, atualOffset);

        if (no.isFolha) {
            // Procura a chave sequencialmente na folha
            for (int i = 0; i < no.numChaves; i++) {
                if (no.chaves[i] == id) {
                    arq.close();
                    return no.ponteirosDados[i]; // Retorna a posição em bytes no arquivo de dados
                }
            }
            arq.close();
            return -1; // Não encontrou na folha
        } else {
            // Nó Interno: Navega para o filho correto com base no intervalo do ID
            int i = 0;
            while (i < no.numChaves && id >= no.chaves[i]) {
                i++;
            }
            atualOffset = no.filhos[i];
        }
    }

    arq.close();
    return -1;
}

// Insere uma nova chave de forma ordenada em um nó folha que possui espaço
void BPlusTree::inserirNaFolha(NoBPlus& folha, int id, long offsetDados) {
    int i = folha.numChaves - 1;
    while (i >= 0 && folha.chaves[i] > id) {
        folha.chaves[i + 1] = folha.chaves[i];
        folha.ponteirosDados[i + 1] = folha.ponteirosDados[i];
        i--;
    }
    folha.chaves[i + 1] = id;
    folha.ponteirosDados[i + 1] = offsetDados;
    folha.numChaves++;
}

// Divide um nó folha cheio criando um novo nó à direita e gerenciando o encadeamento lateral
void BPlusTree::dividirFolha(std::fstream& arq, long folhaOffset, NoBPlus& folha, int id, long offsetDados, int& chavePromovida, long& novoNoOffset) {
    // Cria arrays temporários para comportar o elemento excedente antes da partição
    int tempChaves[ORDEM];
    long tempDados[ORDEM];
    
    int i = 0;
    while (i < folha.numChaves && folha.chaves[i] < id) {
        tempChaves[i] = folha.chaves[i];
        tempDados[i] = folha.ponteirosDados[i];
        i++;
    }
    tempChaves[i] = id;
    tempDados[i] = offsetDados;
    
    while (i < folha.numChaves) {
        tempChaves[i + 1] = folha.chaves[i];
        tempDados[i + 1] = folha.ponteirosDados[i];
        i++;
    }

    // Cria a nova folha à direita em disco
    NoBPlus novaFolha;
    novaFolha.isFolha = true;
    novoNoOffset = alocarNovoNo(arq);

    // Distribui os elementos (Política B+: o elemento que sobe também permanece na folha direita)
    folha.numChaves = ORDEM / 2;
    novaFolha.numChaves = ORDEM - (ORDEM / 2);

    for (i = 0; i < folha.numChaves; i++) {
        folha.chaves[i] = tempChaves[i];
        folha.ponteirosDados[i] = tempDados[i];
    }
    for (i = 0; i < novaFolha.numChaves; i++) {
        novaFolha.chaves[i] = tempChaves[folha.numChaves + i];
        novaFolha.ponteirosDados[i] = tempDados[folha.numChaves + i];
    }

    // Limpa os slots vazios do nó antigo
    for (i = folha.numChaves; i < ORDEM - 1; i++) {
        folha.chaves[i] = -1;
        folha.ponteirosDados[i] = -1;
    }

    // --- CRUCIAL PARA A LISTAGEM ORDENADA ---
    // A nova folha herda o próximo da antiga, e a antiga aponta para a nova folha
    novaFolha.proximaFolha = folha.proximaFolha;
    folha.proximaFolha = novoNoOffset;

    chavePromovida = novaFolha.chaves[0]; // Na Árvore B+, a menor chave da direita sobe para o pai

    gravarNo(arq, folhaOffset, folha);
    gravarNo(arq, novoNoOffset, novaFolha);
}

// Divide um nó interno cheio jogando o elemento do meio estritamente para o pai
void BPlusTree::dividirInterno(std::fstream& arq, long internoOffset, NoBPlus& interno, int indiceFilho, int chaveFilhoPromovida, long filhoNovoOffset, int& chavePromovida, long& novoNoOffset) {
    int tempChaves[ORDEM];
    long tempFilhos[ORDEM + 1];

    for (int i = 0; i < interno.numChaves; i++) tempChaves[i] = interno.chaves[i];
    for (int i = 0; i < interno.numChaves + 1; i++) tempFilhos[i] = interno.filhos[i];

    int i = interno.numChaves - 1;
    while (i >= 0 && tempChaves[i] > chaveFilhoPromovida) {
        tempChaves[i + 1] = tempChaves[i];
        tempFilhos[i + 2] = tempFilhos[i + 1];
        i--;
    }
    tempChaves[i + 1] = chaveFilhoPromovida;
    tempFilhos[i + 2] = filhoNovoOffset;

    NoBPlus novoInterno;
    novoInterno.isFolha = false;
    novoNoOffset = alocarNovoNo(arq);

    // Divide equilibradamente
    interno.numChaves = (ORDEM / 2);
    novoInterno.numChaves = ORDEM - 1 - (ORDEM / 2);

    // Carrega o nó interno antigo
    for (i = 0; i < interno.numChaves; i++) {
        interno.chaves[i] = tempChaves[i];
        interno.filhos[i] = tempFilhos[i];
    }
    interno.filhos[interno.numChaves] = tempFilhos[interno.numChaves];

    // Elemento do meio exato é isolado para subir ao pai (não fica no nó interno da direita)
    chavePromovida = tempChaves[interno.numChaves];

    // Carrega o novo nó interno
    for (i = 0; i < novoInterno.numChaves; i++) {
        novoInterno.chaves[i] = tempChaves[interno.numChaves + 1 + i];
        novoInterno.filhos[i] = tempFilhos[interno.numChaves + 1 + i];
    }
    novoInterno.filhos[novoInterno.numChaves] = tempFilhos[interno.numChaves + 1 + novoInterno.numChaves];

    // Limpa os slots não utilizados do nó original
    for (i = interno.numChaves; i < ORDEM - 1; i++) interno.chaves[i] = -1;
    for (i = interno.numChaves + 1; i < ORDEM; i++) interno.filhos[i] = -1;

    gravarNo(arq, internoOffset, interno);
    gravarNo(arq, novoNoOffset, novoInterno);
}

// Insere de baixo para cima a chave promovida no nó pai correspondente
void BPlusTree::inserirNoPai(std::fstream& arq, long noFilhoOffset, int chavePromovida, long noNovoFilhoOffset) {
    // Caso Especial: O nó dividido era a própria Raiz. Logo, cria-se uma nova Raiz no topo.
    if (noFilhoOffset == this->raizOffset) {
        NoBPlus novaRaiz;
        novaRaiz.isFolha = false;
        novaRaiz.chaves[0] = chavePromovida;
        novaRaiz.filhos[0] = this->raizOffset;
        novaRaiz.filhos[1] = noNovoFilhoOffset;
        novaRaiz.numChaves = 1;

        long novaRaizOffset = alocarNovoNo(arq);
        gravarNo(arq, novaRaizOffset, novaRaiz);

        this->raizOffset = novaRaizOffset;
        arq.seekp(0, std::ios::beg);
        arq.write(reinterpret_cast<char*>(&this->raizOffset), sizeof(long));
        return;
    }

    // Caso contrário, precisamos localizar o nó pai varrendo a árvore desde a raiz
    long atualOffset = this->raizOffset;
    long paiOffset = -1;

    while (atualOffset != -1) {
        NoBPlus no = lerNo(arq, atualOffset);
        if (no.isFolha) break;

        // Verifica se algum dos caminhos deste nó aponta para o nosso filho
        bool achouFilho = false;
        for (int i = 0; i <= no.numChaves; i++) {
            if (no.filhos[i] == noFilhoOffset) {
                paiOffset = atualOffset;
                achouFilho = true;
                break;
            }
        }

        if (achouFilho) break;

        // Se não achou de forma direta, navega para o próximo nível
        int i = 0;
        while (i < no.numChaves && chavePromovida >= no.chaves[i]) i++;
        atualOffset = no.filhos[i];
    }

    if (paiOffset != -1) {
        NoBPlus pai = lerNo(arq, paiOffset);

        // Se o pai tem espaço livre, insere ordenadamente
        if (pai.numChaves < ORDEM - 1) {
            int i = pai.numChaves - 1;
            while (i >= 0 && pai.chaves[i] > chavePromovida) {
                pai.chaves[i + 1] = pai.chaves[i];
                pai.filhos[i + 2] = pai.filhos[i + 1];
                i--;
            }
            pai.chaves[i + 1] = chavePromovida;
            pai.filhos[i + 2] = noNovoFilhoOffset;
            pai.numChaves++;
            gravarNo(arq, paiOffset, pai);
        } else {
            // Se o pai também está cheio, executa uma cisão em cascata (Split de Nó Interno)
            int novaChavePromovida;
            long novoInternoOffset;
            dividirInterno(arq, paiOffset, pai, noFilhoOffset, chavePromovida, noNovoFilhoOffset, novaChavePromovida, novoInternoOffset);
            inserirNoPai(arq, paiOffset, novaChavePromovida, novoInternoOffset);
        }
    }
}

// Método Principal Público: Insere a dupla (ID, Offset do arquivo .dat) no índice
void BPlusTree::inserir(int id, long offsetDados) {
    std::fstream arq(arquivoIndiceNome, std::ios::binary | std::ios::in | std::ios::out);

    // Caso Inicial: A árvore está 100% vazia
    if (this->raizOffset == -1) {
        NoBPlus raiz;
        raiz.isFolha = true;
        raiz.chaves[0] = id;
        raiz.ponteirosDados[0] = offsetDados;
        raiz.numChaves = 1;

        this->raizOffset = alocarNovoNo(arq);
        gravarNo(arq, this->raizOffset, raiz);

        // Atualiza a posição estável da raiz nos primeiros 8 bytes do arquivo
        arq.seekp(0, std::ios::beg);
        arq.write(reinterpret_cast<char*>(&this->raizOffset), sizeof(long));
        arq.close();
        return;
    }

    // Desce pela árvore até achar o nó folha ideal para a inserção
    long atualOffset = this->raizOffset;
    long paiOffset = -1;
    NoBPlus no = lerNo(arq, atualOffset);

    while (!no.isFolha) {
        paiOffset = atualOffset;
        int i = 0;
        while (i < no.numChaves && id >= no.chaves[i]) i++;
        atualOffset = no.filhos[i];
        no = lerNo(arq, atualOffset);
    }

    // Cenário 1: A folha tem espaço livre disponível
    if (no.numChaves < ORDEM - 1) {
        inserirNaFolha(no, id, offsetDados);
        gravarNo(arq, atualOffset, no);
    } else {
        // Cenário 2: A folha está cheia, dispara a lógica de Split da folha
        int chavePromovida;
        long novoNoOffset;
        dividirFolha(arq, atualOffset, no, id, offsetDados, chavePromovida, novoNoOffset);
        
        // Propaga o elemento promovido para as estruturas acima (nós pais)
        inserirNoPai(arq, atualOffset, chavePromovida, novoNoOffset);
    }

    arq.close();
}