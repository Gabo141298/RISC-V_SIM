#include "processor.h"
#include <QDebug>

Processor::Processor(const size_t id, const size_t quatum):
    processorId{id},
    pc {0},
    loopCondition{true},
    registers{std::vector<int>(32, 0)},
    instructionMemory{std::vector<int>(64 * 4, 0)}, // Cada instrucción está compuesta por cuatro enteros para efectos de la simulación
    dataMemory{std::vector<int>(32, 1)},
    clock{0},
    currentState{instructionFetch},
    rl {-1},
    maxQuatum{quatum}

{
    processors.resize(3);
    directory.resize(8);
}

void Processor::run()
{
    int instruction[4] = {0};
    while(loopCondition) // Hay que poner que mientras hay al menos uno corriendo
    {
        processMessages();

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
                accessMemory(instruction);
                ++currentQuatum;
                advanceClockCycle();
                break;
            case execution:
                execute(instruction);
                ++currentQuatum;
                advanceClockCycle();
                break;
            case contextSwitch:
                makeContextSwitch(instruction);
             break;

                // Incremente la barrera
        case finish:
                advanceClockCycle();
                break;
            // No se ocupa default porque Qt se pone en varas, ya que sí estamos poniendo los casos de todo el enum.
        }
    }

    while (!this->pcbFinishedQueue.empty())
    {
       qDebug() << this->processorId <<
                   this->pcbFinishedQueue.front()->getID() <<
                   this->pcbFinishedQueue.front()->pc <<
                   this->pcbFinishedQueue.front()->rl <<
                   this->pcbFinishedQueue.front()->state <<
                   this->pcbFinishedQueue.front()->registers;
       this->pcbFinishedQueue.pop();
    }

    pthread_barrier_wait(barrier);

    if(processorId == 0)
    {
        for(size_t currentProcessor = 0; currentProcessor < processors.size(); ++currentProcessor)
        {
            /*for(size_t currentPosition = 0; currentPosition < processors[currentProcessor]->dataMemory.size(); ++currentPosition)
            {
                qDebug() << currentProcessor * 32 + currentPosition << ": " << processors[currentProcessor]->dataMemory[currentPosition];
            }*/
            qDebug() << currentProcessor << " Memory: " << processors[currentProcessor]->dataMemory;
            qDebug() << currentProcessor << " Cache: ";
            processors[currentProcessor]->dataCache.toString();
        }
    }
}


void Processor::execute(int instruction[])
{
    //qDebug() << this->processorId << ": " << instruction[0] << "on cycle" << this->clock;
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

void Processor::accessMemory(int instruction[4])
{
    //qDebug() << this->processorId << ": " << instruction[0] << "on cycle" << this->clock;
    switch(instruction[0])
    {
        case lw:
            registers[instruction[1]] = dataCache.getDataAt(this, registers[instruction[2]] + instruction[3]);
            break;
        case sw:
            dataCache.storeDataAt(this, registers[instruction[1]] + instruction[3], registers[instruction[2]]);
            break;
        case lr:
            // Hace un load normal
            registers[instruction[1]] = dataCache.getDataAt(this, registers[instruction[2]] + instruction[3]);
            // Le cambia el valor de rl a -1
            this->rl = registers[instruction[2]] + instruction[3];
            break;
        case sc:

            // Calcula la posicion de memoria
            int mempos = registers[instruction[2]] + instruction[3];
            qDebug() << "Sc o n mem pos " << mempos;
            // Verifica si el rl es igual a la direccion de memoria de adonde voy a guardar
            if (this->rl == mempos )
            {
                qDebug() << "rl save";
                dataCache.storeDataAt(this, registers[instruction[1]] + instruction[3], registers[instruction[2]]);
                // Entonces guardo en memoria....
                // El store en memoria tambien tiene que verificar si el RL del procesador
                // afecta el proceso
            }else {
                // Guardon un  0 en x2
                registers[instruction[1]] = 0;
                qDebug() << "Couldnt get the lock";
                // y no escribe
            }
            break;
      //  default:
        //    break;
    }
    this->pc += 4;
    currentState = instructionFetch;
}

void Processor::advanceClockCycle()
{
    if (currentQuatum == maxQuatum)
    {
        currentState = contextSwitch;
        currentQuatum = 0;
    }

    ++this->clock;
    pthread_barrier_wait(this->barrier);

    bool breakLoop = false;

    for (size_t index = 0; index < processors.size(); ++index)
    {
        if (processors.at(index)->currentState != finish)
            breakLoop = true;

    }
    this->loopCondition = breakLoop;


    while(!this->mail.empty())
    {
        messagesToProcess.push( this->mail.front());
        this->mail.pop();
    }



    pthread_barrier_wait(this->barrier);
}

void Processor::processMessages(size_t* waitingAcks)
{
    while ( !this->messagesToProcess.empty() )
    {
        //qDebug() << this->processorId << ": " << "Processing " << messagesToProcess.size() << " messages";
        Message currentMessage = messagesToProcess.front();
        messagesToProcess.pop();

        int blockInCache = currentMessage.blockToChangeState % 4;

        if(currentMessage.opcode == ack && waitingAcks)
        {
            --*waitingAcks;
        }
        else if (currentMessage.opcode == invalidate)
        {
            int rlBlock = this->rl/16;


            if (rlBlock == currentMessage.blockToChangeState)
            {
                qDebug() << "Changing rl state" << "on processor" << this->processorId;
                this->rl = -1;
            }

            if(dataCache.state[blockInCache] == modified)
                dataCache.copyBlockToMem(this, currentMessage.blockToChangeState, blockInCache, true, currentMessage.otherCacheBlock);
            dataCache.state[blockInCache] = invalid;
        }
        else if(currentMessage.opcode == leaveAsShared)
        {
            if(dataCache.state[blockInCache] == modified)
                dataCache.copyBlockToMem(this, currentMessage.blockToChangeState, blockInCache, true, currentMessage.otherCacheBlock);
            dataCache.state[blockInCache] = shared;
        }

        if(currentMessage.opcode == invalidate || currentMessage.opcode == leaveAsShared)
            sendMessage(Message(ack), currentMessage.sendAckTo);
    }
}

void Processor::init_barrier(pthread_barrier_t *barrier)
{
    this->barrier = barrier;
}

void Processor::sendMessage(Message message, size_t processorId)
{
    processors[processorId]->messagesMutex.lock();
    processors[processorId]->mail.push(message);
    processors[processorId]->messagesMutex.unlock();
}

void Processor::processAcks(size_t* waitingAcks)
{
    while(*waitingAcks > 0)
    {
        processMessages(waitingAcks);
        advanceClockCycle();
    }
}

void Processor::makeContextSwitch(int instruction[])
{
    this->rl = -1;
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
