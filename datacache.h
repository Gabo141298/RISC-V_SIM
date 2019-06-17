#ifndef DATACACHE_H
#define DATACACHE_H

#include "cache.h"
#include "directory.h"

class Processor;

class DataCache : public Cache
{
private:
    int cacheMem[4][4] = {{0}};
public:
    DataCache();
    int getDataAt();
    void solveFail(Processor* processor, const int &blockInMemory, const int &victimBlock) override;
    void copyBlockToMem(Processor* processor, const int &blockInMemory, const int &modifiedBlock);
    void changeDirectoryState(Processor* processor, const int &blockInMemory, states blockNewState);
    void obtainBlock(Processor* processor, const int &blockInMemory, const int &victimBlock);
};

#endif // DATACACHE_H
