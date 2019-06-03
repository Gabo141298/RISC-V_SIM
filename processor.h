#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include "cache.h"
#include "instructioncache.h"
#include "datacache.h"
#include "pcb.h"

class Processor
{

    typedef struct
    {
        short state;
        char processor[3];
    }directoryBlock;

private:
    int pc;
    std::vector<int> registers;
    std::vector<int> instructionMemory;
    std::vector<int> dataMemory;
    std::vector<directoryBlock> directory;
    // En el futuro va a haber que cambiarlo por las respectivas clases virutales
    InstructionCache instructionsCache;
    DataCache dataCache;
    // Esto despues lo cambiamos por la lista enlazada circular con nodo centinela :V
    std::vector<Pcb> pcbQueue;
    int rl;

public:
    Processor();

    friend class Cache;

    inline void addi(unsigned destinationRegister, unsigned sourceRegister, int immediate)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister] + immediate;
    }
    inline void add(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] + this->registers[sourceRegister2];
    }
    inline void sub(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] - this->registers[sourceRegister2];
    }
    inline void mul(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] * this->registers[sourceRegister2];
    }
    inline void div(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] / this->registers[sourceRegister2];
    }

    void beq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);
    void bne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);

    void jal(unsigned destinationRegister, int immediate);
    void jalr(unsigned destinationRegister, unsigned sourceRegister, int immediate);
};

#endif // PROCESSOR_H
