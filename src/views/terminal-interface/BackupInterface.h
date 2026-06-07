#ifndef BACKUP_INTERFACE_H
#define BACKUP_INTERFACE_H

#include <iostream>
#include <iomanip>

#include "../../utils/compression/BackupManager.h"

using namespace std;

inline void printStats(const BackupManager::Stats &s, const string &algoName)
{
    if (!s.ok)
    {
        cout << "Falha ao gerar o backup com " << algoName << "." << endl;
        return;
    }

    double ratio = s.originalSize > 0 ? static_cast<double>(s.compressedSize) / static_cast<double>(s.originalSize) : 0.0;
    double saving = (1.0 - ratio) * 100.0;

    cout << "\n=== Compressao " << algoName << " ===" << endl;
    cout << "Arquivo gerado:        " << s.outputPath << endl;
    cout << "Tamanho original:      " << s.originalSize << " bytes" << endl;
    cout << "Tamanho comprimido:    " << s.compressedSize << " bytes" << endl;
    cout << fixed << setprecision(2);
    cout << "Taxa (comp/orig):      " << (ratio * 100.0) << " %" << endl;
    cout << "Economia (1 - taxa):   " << saving << " %" << endl;
    cout.unsetf(ios::fixed);
}

inline void BackupMenu()
{
    int option;

    do
    {
        cout << "\n--- BACKUP / COMPRESSAO ---" << endl;
        cout << "1. Compactar dados (Huffman)" << endl;
        cout << "2. Compactar dados (LZW)" << endl;
        cout << "3. Restaurar backup (Huffman)" << endl;
        cout << "4. Restaurar backup (LZW)" << endl;
        cout << "5. Comparar taxas (Huffman vs LZW)" << endl;
        cout << "0. Voltar ao Menu Principal" << endl;
        cout << "Escolha: ";
        cin >> option;
        cin.ignore();

        switch (option)
        {
        case 1:
        {
            BackupManager::Stats s = BackupManager::compress(BackupManager::Algorithm::HUFFMAN);
            printStats(s, "Huffman");
            break;
        }
        case 2:
        {
            BackupManager::Stats s = BackupManager::compress(BackupManager::Algorithm::LZW);
            printStats(s, "LZW");
            break;
        }
        case 3:
        {
            cout << "ATENCAO: os arquivos atuais em data/ serao sobrescritos." << endl;
            if (BackupManager::restore(BackupManager::Algorithm::HUFFMAN))
                cout << "Backup Huffman restaurado com sucesso." << endl;
            else
                cout << "Falha ao restaurar o backup Huffman." << endl;
            break;
        }
        case 4:
        {
            cout << "ATENCAO: os arquivos atuais em data/ serao sobrescritos." << endl;
            if (BackupManager::restore(BackupManager::Algorithm::LZW))
                cout << "Backup LZW restaurado com sucesso." << endl;
            else
                cout << "Falha ao restaurar o backup LZW." << endl;
            break;
        }
        case 5:
        {
            BackupManager::Stats h = BackupManager::compress(BackupManager::Algorithm::HUFFMAN);
            BackupManager::Stats l = BackupManager::compress(BackupManager::Algorithm::LZW);
            printStats(h, "Huffman");
            printStats(l, "LZW");

            if (h.ok && l.ok)
            {
                cout << "\n=== Comparativo ===" << endl;
                if (h.compressedSize < l.compressedSize)
                    cout << "Huffman gerou o menor arquivo." << endl;
                else if (l.compressedSize < h.compressedSize)
                    cout << "LZW gerou o menor arquivo." << endl;
                else
                    cout << "Ambos geraram arquivos do mesmo tamanho." << endl;
            }
            break;
        }
        case 0:
            break;
        default:
            cout << "Opcao invalida. Tente novamente." << endl;
        }
    } while (option != 0);
}

#endif
