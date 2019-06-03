#include "instructioncache.h"

#include <cstring>

InstructionCache::InstructionCache()
{

}

void InstructionCache::fetch(const int &pc, int instruction[4])
{
    int blockInMemory = pc / 16;
    int blockInCache = blockInMemory % 4;
    if(blockID[blockInCache] != blockInMemory || state[blockInCache] == invalid)
        solveFail(blockInMemory);
    memcpy(instruction, cacheMem[blockInCache], 4 * sizeof(char));
}

void InstructionCache::solveFail(int blockInMemory)
{
    int victimBlock = blockInMemory % 4;
    if(state[victimBlock] == modified)
        //copyToMem();
        (void)victimBlock;
    //memcpy(cacheMem[victimBlock], );
}
