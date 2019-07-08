#include "instructioncache.h"
#include "processor.h"

#include <cstring>


InstructionCache::InstructionCache()
{

}

void InstructionCache::fetch(Processor *processor, int instruction[4])
{
    int blockInMemory = (processor->pc - 128) / 16;
    int blockInCache = blockInMemory % 4;
    int wordInBlock = ((processor->pc - 128) / 4) % 4;

    if(!isInstructionInCache(blockInMemory, blockInCache))
        solveFail(processor, blockInMemory, blockInCache);

    memcpy(instruction, cacheMem[blockInCache][wordInBlock], sizeof(cacheMem[blockInCache][wordInBlock]));

}

void InstructionCache::solveFail(Processor* processor, const int &blockInMemory, const int &victimBlock)
{
    for(size_t currentInstruction = 0; currentInstruction < 4; ++currentInstruction)
    {
        for(size_t instructionPart = 0; instructionPart < 4; ++instructionPart)
        {
            cacheMem[victimBlock][currentInstruction][instructionPart] =
                    processor->instructionMemory[static_cast<size_t>(blockInMemory) * 16 + currentInstruction * 4 + instructionPart];
                                                                // Multiplicar por 16 nos da la dirección de la primera palabra de ese bloque.
            // En total son 16 ciclos, por lo que podemos aumentar los ciclos de reloj aquí
            processor->advanceClockCycle();
        }
    }
    blockID[victimBlock] = blockInMemory;
}
