#include "processor.h"
#include <QDebug>

Processor::Processor(const size_t id):
    processorId{id},
    pc {0},
    clock{0},
    currentState{instructionFetch},
    rl {-1}
{
    registers.resize(32);
    instructionMemory.resize(64 * 4); // Cada instrucción está compuesta por cuatro enteros para efectos de la simulación
    dataMemory.resize(32);
    directory.resize(8);
}

void Processor::run()
{
    qDebug() << this->processorId;
    int instruction[4] = {0};
    while(1) // Hay que poner que mientras hay al menos uno corriendo
    {
        if(!messages.empty())
            (void)this; // Process message

        switch(currentState)
        {
            case instructionFetch:
                instructionsCache.fetch(this, instruction);
                if(isMemoryInstruction(instruction[0]))
                    currentState = dataFetch;
                else
                    currentState = execution;
                break;
            // Al igual que en el fetch de instrucciones, en datos se resuelven los fallos
            case dataFetch:
                break;
            case execution:
                execute(instruction);
                break;
            case contextSwitch:
                break;
            // No se ocupa default porque Qt se pone en varas, ya que sí estamos poniendo los casos de todo el enum.
        }

        advanceClockCycle();
    }
}

void Processor::execute(int instruction[])
{
    switch(instruction[0])
    {
        case addi:
            execAddi(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), instruction[3]);
            break;
        case add:
            execAdd(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), static_cast<unsigned>(instruction[3]));
            break;
        case sub:
            execSub(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), static_cast<unsigned>(instruction[3]));
            break;
        case mul:
            execMul(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), static_cast<unsigned>(instruction[3]));
            break;
        case div:
            execDiv(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), static_cast<unsigned>(instruction[3]));
            break;
        case beq:
            execBeq(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), instruction[3]);
            break;
        case bne:
            execBne(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), instruction[3]);
            break;
        case jal:
            execJal(static_cast<unsigned>(instruction[1]), instruction[3]);
            break;
        case jalr:
            execJalr(static_cast<unsigned>(instruction[1]), static_cast<unsigned>(instruction[2]), instruction[3]);
            break;
        case fin:
            currentState = contextSwitch;
            break;
        default:
            break;
    }
    if(instruction[0] != fin)
        currentState = instructionFetch;
}

void Processor::advanceClockCycle()
{
    ++this->clock;
    pthread_barrier_wait(this->barrier);
}

void Processor::sendMessage(Processor::MessageTypes messageType)
{
    (void)messageType;
}

void Processor::processAcks(const size_t& waitingAcks)
{
    while(waitingAcks > 0)
    {
        // Wait for acks.
    }
}

void Processor::execBeq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate)
{
    if(this->registers[sourceRegister1] == this->registers[sourceRegister2])
        this->pc += immediate * 4;
}

void Processor::execBne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate)
{
    if(this->registers[sourceRegister1] != this->registers[sourceRegister2])
        this->pc += immediate * 4;
}

void Processor::execJal(unsigned destinationRegister, int immediate)
{
    this->registers[destinationRegister] = this->pc;
    this->pc += immediate;
}

void Processor::execJalr(unsigned destinationRegister, unsigned sourceRegister, int immediate)
{
    this->registers[destinationRegister] = this->pc;
    this->pc = this->registers[sourceRegister] + immediate;
}
