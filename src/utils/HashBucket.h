#ifndef HASH_BUCKET_H
#define HASH_BUCKET_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

const int MAX_RECORDS = 3;

struct HashRecord
{
    int moduleId;
    int64_t registrationOffset;
};

struct Bucket
{
    int localDepth;
    int currentSize;
    int64_t overflowAddress;
    HashRecord records[MAX_RECORDS];

    Bucket()
    {
        localDepth = 0; 
        currentSize = 0;
        overflowAddress = -1;
        for (int i = 0; i < MAX_RECORDS; i++)
        {
            records[i].moduleId = -1;
            records[i].registrationOffset = -1;
        }
    }

    bool isFull()
    {
        return currentSize == MAX_RECORDS;
    }
};

class BucketManager
{
private:
    std::string path;

public:
    BucketManager(std::string filePath) : path(filePath)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            std::ofstream newFile(path, std::ios::binary);
            Bucket emptyBucket;
            newFile.write(reinterpret_cast<char *>(&emptyBucket), sizeof(Bucket));
            newFile.close();
        }
        else
        {
            file.close();
        }
    }

    Bucket readBucket(int64_t address)
    {
        Bucket b;
        std::ifstream file(path, std::ios::binary);
        if (file.is_open())
        {
            file.seekg(address);
            file.read(reinterpret_cast<char *>(&b), sizeof(Bucket));
            file.close();
        }
        return b;
    }

    void writeBucket(int64_t address, Bucket &b)
    {
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
        if (file.is_open())
        {
            file.seekp(address);
            file.write(reinterpret_cast<char *>(&b), sizeof(Bucket));
            file.close();
        }
    }

    int64_t appendBucket(Bucket &b)
    {
        std::ofstream file(path, std::ios::binary | std::ios::app);

        int64_t newAddress = file.tellp();
        file.write(reinterpret_cast<char *>(&b), sizeof(Bucket));
        file.close();

        return newAddress;
    }
};

#endif