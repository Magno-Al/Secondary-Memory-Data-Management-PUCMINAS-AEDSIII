#ifndef HASH_DIRECTORY_H
#define HASH_DIRECTORY_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdint>

class HashDirectory
{
private:
    std::string path;
    int globalDepth;
    std::vector<int64_t> bucketAddresses;

public:
    HashDirectory(std::string filePath) : path(filePath)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open())
        {
            globalDepth = 0;
            bucketAddresses.push_back(0);
            save();
        }
        else
        {
            load();
        }
    }

    void load()
    {
        std::ifstream file(path, std::ios::binary);
        file.read(reinterpret_cast<char *>(&globalDepth), sizeof(int));

        int size = std::pow(2, globalDepth);
        bucketAddresses.resize(size);
        file.read(reinterpret_cast<char *>(bucketAddresses.data()), size * sizeof(int64_t));
        file.close();
    }

    void save()
    {
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<char *>(&globalDepth), sizeof(int));
        file.write(reinterpret_cast<char *>(bucketAddresses.data()), bucketAddresses.size() * sizeof(int64_t));
        file.close();
    }

    int64_t getBucketAddress(int key)
    {
        int hash = key % static_cast<int>(std::pow(2, globalDepth));
        return bucketAddresses[hash];
    }

    void updateAddress(int index, int64_t newAddress)
    {
        if (index < bucketAddresses.size())
        {
            bucketAddresses[index] = newAddress;
        }
    }

    void doubleDirectory()
    {
        globalDepth++;
        int oldSize = bucketAddresses.size();
        bucketAddresses.resize(oldSize * 2);

        for (int i = 0; i < oldSize; i++)
        {
            bucketAddresses[i + oldSize] = bucketAddresses[i];
        }
    }

    int getGlobalDepth() { return globalDepth; }
    int getSize() { return bucketAddresses.size(); }
};

#endif