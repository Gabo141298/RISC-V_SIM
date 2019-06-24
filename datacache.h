#ifndef DATACACHE_H
#define DATACACHE_H

#include "cache.h"
#include "directory.h"

class Processor;

class DataCache : public Cache
{
    enum MemoryOperation
    {
        load,
        store,
    };

private:
    int cacheMem[4][4] = {{0}};
public:
    friend Processor;

    DataCache();
    int getDataAt(Processor* processor, int dataPosition);
    void storeDataAt(Processor* processor, int dataPosition, int word);
    void solveFail(Processor* processor, const int &blockInMemory, const int &victimBlock, MemoryOperation operation);
    void copyBlockToMem(Processor* processor, const int &blockInMemory, const int &modifiedBlock, bool copyToAnotherCache = false, int* otherCacheBlock = nullptr);
    void copyBlockFromMem(Processor *processor, const int &blockInMemory, const int &blockInCache);
    void changeDirectoryState(Processor* processor, const int &blockInMemory, states blockNewState);
    void obtainBlock(Processor* processor, const int &blockInMemory, const int &blockInCache, MemoryOperation operation);
    inline bool isWordInCache(const int &blockInMemory, const int &blockInCache)
    {
        return blockID[blockInCache] == blockInMemory;
    }
};

#endif // DATACACHE_H
