#include "processor.h"

Processor::Processor():
    pc {0},
    clock{0},
    currentState{fetch},
    rl {-1}
{
    registers.resize(32);
    instructionMemory.resize(64);
    dataMemory.resize(32);
    directory.resize(8);
}

void Processor::run()
{
    int instruction[4];
    while(1) // Hay que poner que mientras hay al menos uno corriendo
    {
        if(!messages.empty())
            (void)this; // Process message

        switch(currentState)
        {
            case instructionCacheCheck:
                if(this->instructionsCache.isInstructionInCache(this->pc))
                    currentState = fetch;
                else
                    currentState = instructionCacheFail;
                break;
            case instructionCacheFail:
                this->instructionsCache.solveFail(this->pc);
                break;

            case fetch:
                instructionsCache.fetch(this->pc, instruction);
                if(isMemoryInstruction(instruction[0]))
                    currentState = dataCacheCheck;
                else
                    currentState = execution;
                break;

            case dataCacheCheck:
                break;
            case dataCacheFail:
                break;
            case execution:
                break;
            case contextSwitch:
                break;
            // No se ocupa default porque Qt se pone en varas, ya que sí estamos poniendo los casos de todo el enum.
        }

        pthread_barrier_wait(this->barrier);
    }
}

void Processor::beq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate)
{
    if(this->registers[sourceRegister1] == this->registers[sourceRegister2])
        this->pc += immediate * 4;
}

void Processor::bne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate)
{
    if(this->registers[sourceRegister1] != this->registers[sourceRegister2])
        this->pc += immediate * 4;
}

void Processor::jal(unsigned destinationRegister, int immediate)
{
    this->registers[destinationRegister] = this->pc;
    this->pc += immediate;
}

void Processor::jalr(unsigned destinationRegister, unsigned sourceRegister, int immediate)
{
    this->registers[destinationRegister] = this->pc;
    this->pc = this->registers[sourceRegister] + immediate;
}
