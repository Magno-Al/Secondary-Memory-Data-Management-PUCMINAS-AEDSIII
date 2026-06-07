#ifndef BACKUP_MANAGER_H
#define BACKUP_MANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <algorithm>

#include "Huffman.h"
#include "LZW.h"

namespace fs = std::filesystem;

class BackupManager
{
public:
    enum class Algorithm
    {
        HUFFMAN,
        LZW
    };

    struct Stats
    {
        bool ok = false;
        uint64_t originalSize = 0;
        uint64_t compressedSize = 0;
        std::string outputPath;
    };

    static const std::string DATA_DIR;    
    static const std::string COMPRESS_DIR; 
    static const std::string HUFFMAN_BACKUP;
    static const std::string LZW_BACKUP;

    static std::string backupPath(Algorithm algo)
    {
        return algo == Algorithm::HUFFMAN ? HUFFMAN_BACKUP : LZW_BACKUP;
    }

    static Stats compress(Algorithm algo)
    {
        Stats stats;
        uint64_t originalSize = 0;
        std::vector<unsigned char> blob = packDataFolder(originalSize);

        if (blob.empty())
        {
            std::cerr << "Nenhum arquivo de dados encontrado em " << DATA_DIR << std::endl;
            return stats;
        }

        std::vector<unsigned char> compressed =
            (algo == Algorithm::HUFFMAN) ? Huffman::compress(blob) : LZW::compress(blob);

        fs::create_directories(COMPRESS_DIR);

        std::string out = backupPath(algo);
        std::ofstream file(out, std::ios::binary | std::ios::trunc);
        if (!file.is_open())
        {
            std::cerr << "Nao foi possivel criar o arquivo de backup: " << out << std::endl;
            return stats;
        }
        file.write(reinterpret_cast<const char *>(compressed.data()),
                   static_cast<std::streamsize>(compressed.size()));
        file.close();

        stats.ok = true;
        stats.originalSize = originalSize;
        stats.compressedSize = compressed.size();
        stats.outputPath = out;
        return stats;
    }

    static bool restore(Algorithm algo)
    {
        std::string in = backupPath(algo);
        std::ifstream file(in, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Arquivo de backup nao encontrado: " << in << std::endl;
            return false;
        }
        std::vector<unsigned char> compressed(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        file.close();

        std::vector<unsigned char> blob =
            (algo == Algorithm::HUFFMAN) ? Huffman::decompress(compressed)
                                         : LZW::decompress(compressed);

        return unpack(blob);
    }

private:
    static void putU32(std::vector<unsigned char> &out, uint32_t v)
    {
        for (int i = 0; i < 4; i++)
            out.push_back(static_cast<unsigned char>((v >> (8 * i)) & 0xFF));
    }
    static void putU64(std::vector<unsigned char> &out, uint64_t v)
    {
        for (int i = 0; i < 8; i++)
            out.push_back(static_cast<unsigned char>((v >> (8 * i)) & 0xFF));
    }
    static uint32_t getU32(const std::vector<unsigned char> &in, size_t &pos)
    {
        uint32_t v = 0;
        for (int i = 0; i < 4; i++)
            v |= static_cast<uint32_t>(in[pos + i]) << (8 * i);
        pos += 4;
        return v;
    }
    static uint64_t getU64(const std::vector<unsigned char> &in, size_t &pos)
    {
        uint64_t v = 0;
        for (int i = 0; i < 8; i++)
            v |= static_cast<uint64_t>(in[pos + i]) << (8 * i);
        pos += 8;
        return v;
    }

    static bool isBackupArtifact(const std::string &name)
    {
        return name == fs::path(HUFFMAN_BACKUP).filename().string() ||
               name == fs::path(LZW_BACKUP).filename().string();
    }

    static std::vector<unsigned char> packDataFolder(uint64_t &originalSize)
    {
        std::vector<unsigned char> blob;
        originalSize = 0;

        if (!fs::exists(DATA_DIR) || !fs::is_directory(DATA_DIR))
            return blob;

        std::vector<fs::path> files;
        for (const auto &entry : fs::directory_iterator(DATA_DIR))
        {
            if (!entry.is_regular_file())
                continue;
            if (isBackupArtifact(entry.path().filename().string()))
                continue;
            files.push_back(entry.path());
        }
        std::sort(files.begin(), files.end());

        putU32(blob, static_cast<uint32_t>(files.size()));

        for (const auto &p : files)
        {
            std::string name = p.filename().string();

            std::ifstream f(p, std::ios::binary);
            std::vector<unsigned char> content(
                (std::istreambuf_iterator<char>(f)),
                std::istreambuf_iterator<char>());
            f.close();

            originalSize += content.size();

            putU32(blob, static_cast<uint32_t>(name.size()));
            blob.insert(blob.end(), name.begin(), name.end());
            putU64(blob, static_cast<uint64_t>(content.size()));
            blob.insert(blob.end(), content.begin(), content.end());
        }

        return blob;
    }

    static bool unpack(const std::vector<unsigned char> &blob)
    {
        if (blob.size() < 4)
        {
            std::cerr << "Backup vazio ou corrompido." << std::endl;
            return false;
        }

        fs::create_directories(DATA_DIR);

        size_t pos = 0;
        uint32_t numFiles = getU32(blob, pos);

        for (uint32_t i = 0; i < numFiles; i++)
        {
            if (pos + 4 > blob.size())
                return false;
            uint32_t nameLen = getU32(blob, pos);
            if (pos + nameLen > blob.size())
                return false;
            std::string name(blob.begin() + pos, blob.begin() + pos + nameLen);
            pos += nameLen;

            if (pos + 8 > blob.size())
                return false;
            uint64_t contentLen = getU64(blob, pos);
            if (pos + contentLen > blob.size())
                return false;

            std::string outPath = (fs::path(DATA_DIR) / name).string();
            std::ofstream out(outPath, std::ios::binary | std::ios::trunc);
            if (!out.is_open())
            {
                std::cerr << "Nao foi possivel restaurar: " << outPath << std::endl;
                return false;
            }
            if (contentLen > 0)
                out.write(reinterpret_cast<const char *>(blob.data() + pos),
                          static_cast<std::streamsize>(contentLen));
            out.close();
            pos += contentLen;
        }
        return true;
    }
};

inline const std::string BackupManager::DATA_DIR = "data";
inline const std::string BackupManager::COMPRESS_DIR = "data-compress";
inline const std::string BackupManager::HUFFMAN_BACKUP = "data-compress/backup_huffman.huff";
inline const std::string BackupManager::LZW_BACKUP = "data-compress/backup_lzw.lzw";

#endif
