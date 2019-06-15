#include "instructioncache.h"

#include <cstring>

InstructionCache::InstructionCache()
{

}

bool InstructionCache::isInstructionInCache(const int &pc)
{
    int blockInMemory = pc / 16;
    int blockInCache = blockInMemory % 4;
    return blockID[blockInCache] == blockInMemory;
}

void InstructionCache::fetch(const int &pc, int instruction[4])
{
    int blockInMemory = pc / 16;
    int blockInCache = blockInMemory % 4;
    memcpy(instruction, cacheMem[blockInCache], 4 * sizeof(int));
}

void InstructionCache::solveFail(const int& pc)
{
    int blockInMemory = pc / 16;
    int blockInCache = blockInMemory % 4;
    int victimBlock = blockInMemory % 4;
    //memcpy(cacheMem[victimBlock], );
}
