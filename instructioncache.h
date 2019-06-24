#ifndef INSTRUCTIONCACHE_H
#define INSTRUCTIONCACHE_H

#include "cache.h"

class Processor;

class InstructionCache : public Cache
{
private:
    int cacheMem[4][4][4] = {{{0}}};
public:
    InstructionCache();
    inline bool isInstructionInCache(const int &blockInMemory, const int &blockInCache)
    {
        return blockID[blockInCache] == blockInMemory;
    }
    void fetch(Processor* processor, int instruction[4]);
    void solveFail(Processor* processor, const int &blockInMemory, const int &blockInCache);
};

#endif // INSTRUCTIONCACHE_H
