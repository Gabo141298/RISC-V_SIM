#include "processor.h"
#include <QDebug>

Processor::Processor(const size_t id, const size_t quatum):
    processorId{id},
    pc {0},
    clock{0},
    currentState{instructionFetch},
    rl {-1},
    maxQuatum{quatum}

{
    registers.resize(32);
    instructionMemory.resize(64 * 4); // Cada instrucción está compuesta por cuatro enteros para efectos de la simulación
    dataMemory.resize(32);
    directory.resize(8);
}

void Processor::run()
{
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
                ++currentQuatum;
                break;
            case execution:
                execute(instruction);
                ++currentQuatum;
                break;
            case contextSwitch:
                makeContextSwitch(instruction);
             break;

                // Incremente la barrera
        case finish:
                break;
            // No se ocupa default porque Qt se pone en varas, ya que sí estamos poniendo los casos de todo el enum.
        }

        if (currentQuatum == maxQuatum)
        {
            currentState = contextSwitch;
            currentQuatum = 0;
        }

        advanceClockCycle();
    }
}


void Processor::execute(int instruction[])
{
    qDebug() << instruction[0] << "on cycle" << this->clock;
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
    {
        this->pc+=4;
        currentState = instructionFetch;
    }
}

void Processor::advanceClockCycle()
{
    ++this->clock;
    pthread_barrier_wait(this->barrier);
}

void Processor::init_barrier(pthread_barrier_t *barrier)
{
    this->barrier = barrier;
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

void Processor::makeContextSwitch(int instruction[])
{
    // If termino totalmente
    if (instruction[0] == 999)
    {
        Pcb* oldPcb = this->pcbRunningQueue.front();
        oldPcb->saveState(this->pc, oldPcb->finished, oldPcb->getID(),this->rl, this->registers);
        this->pcbRunningQueue.pop();
        this->pcbFinishedQueue.push(oldPcb);

        // Verificar si aún quedan
        this->currentQuatum = 0;

        if (!this->pcbRunningQueue.empty())
        {
            Pcb* current = this->pcbRunningQueue.front();
            current->restore(this->pc,this->rl,this->registers);
            this->currentState = instructionFetch;
        }
        else
        {
            this->currentState = finish;
        }
    }
    // Se le acabo el quatum
    else
    {
        Pcb* oldPcb = this->pcbRunningQueue.front();
        oldPcb->saveState(this->pc, oldPcb->wait, oldPcb->getID(),this->rl, this->registers);
        this->pcbRunningQueue.pop();
        this->pcbRunningQueue.push(oldPcb);

        Pcb* currentPcb = this->pcbRunningQueue.front();
        currentPcb->restore(this->pc, this->rl, this->registers);
        this->currentState = instructionFetch;
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
