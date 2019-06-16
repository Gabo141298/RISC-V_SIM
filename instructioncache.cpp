#include "instructioncache.h"
#include "processor.h"

#include <cstring>


InstructionCache::InstructionCache()
{

}

void InstructionCache::fetch(Processor *processor, int instruction[4])
{
    int blockInMemory = processor->pc / 16;
    int blockInCache = blockInMemory % 4;

    // Se aumenta un ciclo por consulta
    processor->advanceClockCycle();
    if(!isInstructionInCache(blockInMemory, blockInCache))
        solveFail(processor, blockInMemory, blockInCache);

    memcpy(instruction, cacheMem[blockInCache], sizeof(cacheMem[blockInCache]));
}

void InstructionCache::solveFail(Processor* processor, const int &blockInMemory, const int &victimBlock)
{
    for(size_t currentInstruction = 0; currentInstruction < 4; ++currentInstruction)
    {
        for(size_t instructionPart = 0; instructionPart < 4; ++instructionPart)
        {
            cacheMem[victimBlock][currentInstruction][instructionPart] =
                    processor->instructionMemory[static_cast<size_t>(blockInMemory) * 16 + instructionPart];
                                                                // Multiplicar por 16 nos da la dirección de la primera palabra de ese bloque.
            // En total son 16 ciclos, por lo que podemos aumentar los ciclos de reloj aquí
            processor->advanceClockCycle();
        }
    }
}
