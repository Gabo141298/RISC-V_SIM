#ifndef INSTRUCTIONCACHE_H
#define INSTRUCTIONCACHE_H

#include "cache.h"

class InstructionCache : public Cache
{
public:
    InstructionCache();
    bool isInstructionInCache(const int& pc);
    void fetch(const int& pc, int instruction[4]);
    void solveFail(const int& pc) override;
};

#endif // INSTRUCTIONCACHE_H
