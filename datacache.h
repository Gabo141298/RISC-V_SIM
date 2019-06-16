#ifndef DATACACHE_H
#define DATACACHE_H

#include "cache.h"

class Processor;

class DataCache : public Cache
{
private:
    int cacheMem[4][4] = {{0}};
public:
    DataCache();
    int getDataAt();
    void solveFail(Processor* processor, const int &blockInMemory, const int &blockInCache) override;
};

#endif // DATACACHE_H
