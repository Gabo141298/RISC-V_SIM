#include "datacache.h"
#include "processor.h"

DataCache::DataCache()
{

}

int DataCache::getDataAt(Processor* processor, int dataPosition)
{
    int blockInMemory = dataPosition / 16;
    int blockInCache = blockInMemory % 4;
    int wordInBlock = (dataPosition / 4) % 4;

    qDebug() << "Load: " << dataPosition << " Block in memory: " << blockInMemory << " in cache: " << blockInCache << " word: " << wordInBlock;

    if(!( isWordInCache(blockInMemory, blockInCache) && (state[blockInCache] == shared || state[blockInCache] == modified) ) )
        solveFail(processor, blockInMemory, blockInCache, load);
    return cacheMem[blockInCache][wordInBlock];
}

void DataCache::storeDataAt(Processor *processor, int dataPosition, int word)
{
    int blockInMemory = dataPosition / 16;
    int blockInCache = blockInMemory % 4;
    int wordInBlock = (dataPosition / 4) % 4;

    qDebug() << "Store: " << dataPosition << " Block in memory: " << blockInMemory << " in cache: " << blockInCache << " word: " << wordInBlock;

    if(!( isWordInCache(blockInMemory, blockInCache) && (state[blockInCache] == modified) ) )
        solveFail(processor, blockInMemory, blockInCache, store);
    cacheMem[blockInCache][wordInBlock] = word;
}

void DataCache::solveFail(Processor* processor, const int &blockInMemory, const int &victimBlock, MemoryOperation operation)
{
    size_t victimDirectoryHome = static_cast<size_t>(blockID[victimBlock]) / 8;

    // En caso de que esté inválido, no se necesita consultar el directorio del bloque víctima
    if(state[victimBlock] == shared || state[victimBlock] == modified)
    {
        while(!processor->processors[victimDirectoryHome]->directoryMutex.tryLock())
        {
            processor->advanceClockCycle();
            processor->processMessages();
        }

        if(state[victimBlock] == modified)
            copyBlockToMem(processor, blockID[victimBlock], victimBlock);
        changeDirectoryState(processor, blockID[victimBlock], invalid);
        processor->processors[victimDirectoryHome]->directoryMutex.unlock();
    }

    obtainBlock(processor, blockInMemory, victimBlock, operation);

    if(operation == store)
        state[victimBlock] = modified;
    else if(operation == load)
        state[victimBlock] = shared;
    blockID[victimBlock] = blockInMemory;
}

void DataCache::copyBlockToMem(Processor *processor, const int &blockInMemory, const int &modifiedBlock, bool copyToAnotherCache, int* otherCacheBlock)
{
    size_t directoryHome = static_cast<size_t>(blockInMemory) / 8;
    size_t blockInProcessor = static_cast<size_t>(blockInMemory) % 8;
    size_t clocksPerCycle = 4;
    if(processor->processorId != directoryHome || copyToAnotherCache)
        clocksPerCycle *= 2;

    for(size_t currentWord = 0; currentWord < 4; ++currentWord)
    {
        // Multiplicar por 16 nos da la dirección de la primera palabra de ese bloque.
        processor->processors[directoryHome]->dataMemory[static_cast<size_t>(blockInProcessor) * 4 + currentWord] = cacheMem[modifiedBlock][currentWord];
        if(copyToAnotherCache && otherCacheBlock)
            otherCacheBlock[currentWord] = cacheMem[modifiedBlock][currentWord];

        // Si está en la memoria local, clocksPerCycle va a ser 4, por lo que se ejecuta advanceClockCycle 16 veces.
        // Si está en memoria remota, clocksPerCycle va a ser 8, por lo que se ejecuta advanceClockCycle 32 veces.
        for(size_t cycles = 0; cycles < clocksPerCycle; ++cycles)
            processor->advanceClockCycle();
    }
}

void DataCache::copyBlockFromMem(Processor *processor, const int &blockInMemory, const int &blockInCache)
{
    size_t directoryHome = static_cast<size_t>(blockInMemory) / 8;
    size_t blockInProcessor = static_cast<size_t>(blockInMemory) % 8;
    size_t clocksPerCycle = 4;
    if(processor->processorId != directoryHome)
        clocksPerCycle *= 2;
    for(size_t currentWord = 0; currentWord < 4; ++currentWord)
    {
        cacheMem[blockInCache][currentWord] = processor->processors[directoryHome]->dataMemory[static_cast<size_t>(blockInProcessor) * 4 + currentWord];
                                                    // Multiplicar por 16 nos da la dirección de la primera palabra de ese bloque.

        // Si está en la memoria local, clocksPerCycle va a ser 4, por lo que se ejecuta advanceClockCycle 16 veces.
        // Si está en memoria remota, clocksPerCycle va a ser 8, por lo que se ejecuta advanceClockCycle 32 veces.
        for(size_t cycles = 0; cycles < clocksPerCycle; ++cycles)
            processor->advanceClockCycle();
    }
}

void DataCache::changeDirectoryState(Processor *processor, const int &blockInMemory, states blockNewState)
{
    if(blockInMemory >= 0)
    {
        size_t directoryHome = static_cast<size_t>(blockInMemory) / 8;
        size_t blockInProcessor = static_cast<size_t>(blockInMemory) % 8;

        Processor::directoryBlock* directory = &processor->processors[directoryHome]->directory[blockInProcessor];
        switch(blockNewState)
        {
            case invalid:
                directory->processor[processor->processorId] = 0;
                if(directory->processor[0] == 0 && directory->processor[1] == 0 && directory->processor[2] == 0)
                    directory->state = dirUncached;
                break;
            case modified:
                directory->state = dirModified;
                directory->processor[processor->processorId] = 1;
                break;
            case shared:
                directory->state = dirShared;
                directory->processor[processor->processorId] = 1;
                break;
        }
    }
}

void DataCache::obtainBlock(Processor *processor, const int &blockInMemory, const int &blockInCache, MemoryOperation operation)
{
    size_t directoryHome = static_cast<size_t>(blockInMemory) / 8;
    size_t blockInProcessor = static_cast<size_t>(blockInMemory) % 8;
    Processor::directoryBlock* directory = &processor->processors[directoryHome]->directory[blockInProcessor];

    // Se gastan ciclos mientras se intenta obtener el directorio.
    while(!processor->processors[directoryHome]->directoryMutex.tryLock())
    {
        processor->advanceClockCycle();
        processor->processMessages();
    }

    if( operation == load && (directory->state == dirShared || directory->state == dirUncached) )
    {
        copyBlockFromMem(processor, blockInMemory, blockInCache);
        changeDirectoryState(processor, blockInMemory, shared);
    }
    else if( operation == store && directory->state == dirUncached )
    {
        copyBlockFromMem(processor, blockInMemory, blockInCache);
        changeDirectoryState(processor, blockInMemory, modified);
    }
    else
    {
        size_t messagesToSend = 0;
        for(size_t index = 0; index < 3; ++index)
        {
            if(index != processor->processorId && directory->processor[index] == 1)
            {
                ++messagesToSend;

                // En este caso no se debe cambiar a cero, porque el otro procesador va a seguir teniendo el bloque, nada más que compartido.
                if(operation == load)
                    processor->sendMessage(Processor::Message(Processor::MessageTypes::leaveAsShared, blockInMemory, cacheMem[blockInCache], processor->processorId), index);
                // En este caso sí se cambia a cero, porque se tiene que representar solamente a quien tiene el bloque modificado. El resto lo debe tener inválido.
                else if(operation == store)
                {
                    directory->processor[index] = 0;
                    processor->sendMessage(Processor::Message(Processor::MessageTypes::invalidate, blockInMemory, cacheMem[blockInCache], processor->processorId), index);
                }
            }
        }

        // Se encarga de procesar todos los acks, incluyendo el avance de ciclos de reloj.
        processor->processAcks(&messagesToSend);

        if( operation == store && directory->state == dirShared )
            copyBlockFromMem(processor, blockInMemory, blockInCache);

        if( operation == store )
            changeDirectoryState(processor, blockInMemory, modified);
        else if(operation == load )
            changeDirectoryState(processor, blockInMemory, shared);
    }

    processor->processors[directoryHome]->directoryMutex.unlock();
}
