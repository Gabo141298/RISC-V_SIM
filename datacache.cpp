#include "datacache.h"
#include "processor.h"

DataCache::DataCache()
{

}

int DataCache::getDataAt()
{
    return 0;
}

void DataCache::solveFail(Processor* processor, const int &blockInMemory, const int &victimBlock)
{
    int directoryHome = blockInMemory / 8;

    while(!processor->processors[directoryHome]->messagesMutex.tryLock())
        processor->advanceClockCycle();

    if(state[victimBlock] == modified)
        copyBlockToMem(processor, blockID[victimBlock], victimBlock);
    else if(state[victimBlock] == shared)
        changeDirectoryState(processor, blockID[victimBlock], invalid);

    processor->processors[directoryHome]->messagesMutex.unlock();

    obtainBlock(processor, blockInMemory, victimBlock);
}

void DataCache::copyBlockToMem(Processor *processor, const int &blockInMemory, const int &modifiedBlock)
{
    int directoryHome = blockInMemory / 8;
    size_t clocksPerCycle = 4;
    if(processor->processorId != directoryHome)
        clocksPerCycle *= 2;

    for(size_t currentWord = 0; currentWord < 4; ++currentWord)
    {
        // Multiplicar por 16 nos da la dirección de la primera palabra de ese bloque.
        processor->dataMemory[static_cast<size_t>(blockInMemory) * 16 + currentWord] = cacheMem[modifiedBlock][currentWord];

        // Si está en la memoria local, clocksPerCycle va a ser 4, por lo que se ejecuta advanceClockCycle 16 veces.
        // Si está en memoria remota, clocksPerCycle va a ser 8, por lo que se ejecuta advanceClockCycle 32 veces.
        for(size_t cycles = 0; cycles < clocksPerCycle; ++cycles)
            processor->advanceClockCycle();
    }


}

void DataCache::changeDirectoryState(Processor *processor, const int &blockInMemory, states blockNewState)
{
    size_t directoryHome = static_cast<size_t>(blockInMemory) / 8;
    size_t blockInProcessor = static_cast<size_t>(blockInMemory) / 3;

    Processor::directoryBlock* directory = &processor->processors[directoryHome]->directory[blockInProcessor];
    switch(blockNewState)
    {
        case invalid:
            directory->processor[processor->processorId] = 0;
            if(directory->processor[0] == 0 && directory->processor[1] == 0 && directory->processor[2] == 0)
                directory->state = dirUncached;
            break;
        case modified:
        {
            // Por alguna razón se tuvo que poner llaves para que no se quejara de crear una variable dentro del switch.
            size_t invalidations = 0;
            for(int index = 0; index < 3; ++index)
            {
                if(index != processor->processorId && directory->processor[index] == 1)
                {
                    ++invalidations;
                    processor->sendMessage(Processor::MessageTypes::invalidate);
                }
            }

            // Se encarga de procesar todos los acks, incluyendo el avance de ciclos de reloj.
            processor->processAcks(invalidations);

            directory->state = dirModified;
            directory->processor[processor->processorId] = 1;
            break;
        }

        case shared:
            if(directory->state == dirUncached)
                directory->state = dirShared;
            directory->processor[processor->processorId] = 1;
            break;
    }
}

void DataCache::obtainBlock(Processor *processor, const int &blockInMemory, const int &victimBlock)
{
    int directoryHome = blockInMemory / 8;
    size_t clocksPerCycle = 4;
    if(processor->processorId != directoryHome)
        clocksPerCycle *= 2;

    // Se gastan ciclos mientras se intenta obtener el directorio.
    while(!processor->processors[directoryHome]->messagesMutex.tryLock())
        processor->advanceClockCycle();

    for(size_t currentWord = 0; currentWord < 4; ++currentWord)
    {
        cacheMem[victimBlock][currentWord] = processor->dataMemory[static_cast<size_t>(blockInMemory) * 16 + currentWord];
                                                    // Multiplicar por 16 nos da la dirección de la primera palabra de ese bloque.

        // Si está en la memoria local, clocksPerCycle va a ser 4, por lo que se ejecuta advanceClockCycle 16 veces.
        // Si está en memoria remota, clocksPerCycle va a ser 8, por lo que se ejecuta advanceClockCycle 32 veces.
        for(size_t cycles = 0; cycles < clocksPerCycle; ++cycles)
            processor->advanceClockCycle();
    }

    processor->processors[directoryHome]->messagesMutex.unlock();
}
