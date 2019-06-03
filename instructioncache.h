#ifndef INSTRUCTIONCACHE_H
#define INSTRUCTIONCACHE_H

#include "cache.h"

class InstructionCache : public Cache
{
public:
    InstructionCache();
    void fetch(const int& pc, int instruction[4]);
    void solveFail(int blockInMemory) override;
};

#endif // INSTRUCTIONCACHE_H
