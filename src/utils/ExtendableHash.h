#ifndef EXTENDABLE_HASH_H
#define EXTENDABLE_HASH_H

#include "HashDirectory.h"
#include "HashBucket.h"
#include <vector>

class ExtendableHash
{
private:
    HashDirectory directory;
    BucketManager bucketManager;

    void splitAndInsert(int key, int64_t offset, int64_t bucketAddr, Bucket &b)
    {
        if (b.localDepth == directory.getGlobalDepth())
        {
            directory.doubleDirectory();
            directory.save();
        }

        Bucket b0, b1;
        int newDepth = b.localDepth + 1;
        b0.localDepth = newDepth;
        b1.localDepth = newDepth;

        int64_t b0Addr = bucketAddr;
        int64_t b1Addr = bucketManager.appendBucket(b1);

        int dirSize = directory.getSize();
        for (int i = 0; i < dirSize; i++)
        {
            if (directory.getBucketAddress(i) == bucketAddr)
            {
                if ((i & (1 << b.localDepth)) != 0)
                {
                    directory.updateAddress(i, b1Addr);
                }
            }
        }
        directory.save();

        std::vector<HashRecord> tempRecords;
        for (int i = 0; i < b.currentSize; i++)
        {
            tempRecords.push_back(b.records[i]);
        }
        tempRecords.push_back({key, offset});

        bucketManager.writeBucket(b0Addr, b0);

        for (size_t i = 0; i < tempRecords.size(); i++)
        {
            insert(tempRecords[i].moduleId, tempRecords[i].registrationOffset);
        }
    }

public:
    ExtendableHash(std::string dirPath, std::string bucketsPath) : directory(dirPath), bucketManager(bucketsPath) {}

    // ---------------------------------------------------------
    // INSERÇÃO
    // ---------------------------------------------------------
    void insert(int key, int64_t offset)
    {
        int hash = key & ((1 << directory.getGlobalDepth()) - 1);
        int64_t bucketAddr = directory.getBucketAddress(hash);
        Bucket b = bucketManager.readBucket(bucketAddr);

        int64_t currentAddr = bucketAddr;
        Bucket currentBucket = b;

        while (true)
        {
            if (!currentBucket.isFull())
            {
                currentBucket.records[currentBucket.currentSize] = {key, offset};
                currentBucket.currentSize++;
                bucketManager.writeBucket(currentAddr, currentBucket);
                return;
            }

            if (currentBucket.overflowAddress != -1)
            {
                currentAddr = currentBucket.overflowAddress;
                currentBucket = bucketManager.readBucket(currentAddr);
            }
            else
            {
                break;
            }
        }

        bool allSame = true;
        for (int i = 0; i < MAX_RECORDS; i++)
        {
            if (currentBucket.records[i].moduleId != key)
            {
                allSame = false;
                break;
            }
        }

        if (allSame)
        {
            Bucket overflowB;
            overflowB.localDepth = currentBucket.localDepth;
            overflowB.records[0] = {key, offset};
            overflowB.currentSize = 1;

            currentBucket.overflowAddress = bucketManager.appendBucket(overflowB);
            bucketManager.writeBucket(currentAddr, currentBucket);
            return;
        }

        splitAndInsert(key, offset, bucketAddr, b);
    }

    // ---------------------------------------------------------
    // BUSCA RÁPIDA O(1)
    // ---------------------------------------------------------
    std::vector<int64_t> search(int key)
    {
        std::vector<int64_t> results;
        int hash = key & ((1 << directory.getGlobalDepth()) - 1);
        int64_t bucketAddr = directory.getBucketAddress(hash);

        Bucket b = bucketManager.readBucket(bucketAddr);

        while (true)
        {
            for (int i = 0; i < b.currentSize; i++)
            {
                if (b.records[i].moduleId == key)
                {
                    results.push_back(b.records[i].registrationOffset);
                }
            }

            if (b.overflowAddress != -1)
            {
                b = bucketManager.readBucket(b.overflowAddress);
            }
            else
            {
                break;
            }
        }
        return results;
    }
};

#endif