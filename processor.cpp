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
                this->pc+=4;
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

    pthread_barrier_wait(barrier);

    if(processorId == 0)
    {
        qDebug() << "Generating results....";
        std::vector<int> completeDataMem(0);
        std::vector<QString> dataCacheResults(0);
        std::vector<QString> hilillos(0);

        QString processorsData = "Mem: ";
        QString hilillosData = "";

        for(size_t currentProcessor = 0; currentProcessor < processors.size(); ++currentProcessor)
        {
            completeDataMem.insert(completeDataMem.end(), processors[currentProcessor]->dataMemory.begin(), processors[currentProcessor]->dataMemory.end());
            //qDebug() << currentProcessor << " Memory: " << processors[currentProcessor]->dataMemory;
            //qDebug() << currentProcessor << " Cache: ";
            dataCacheResults.push_back(processors[currentProcessor]->dataCache.toString());
        }

        for (size_t index = 0; index < completeDataMem.size(); ++index)
        {
            processorsData += "M["+ QString::number(index) + "]:"+ QString::number(completeDataMem[index]) + " ";
        }
        processorsData += "\n\n";

        qDebug() << completeDataMem;

        for (size_t index = 0; index < dataCacheResults.size(); ++index)
        {
            //qDebug() << "Processor " << index;
            processorsData += "Processor " + QString::number(index) + ": \n";
            processorsData += dataCacheResults[index] + "\n";
        }

        for(size_t currentProcessor = 0; currentProcessor < processors.size(); ++currentProcessor)
        {
            qDebug() << "Processor [" + QString::number(currentProcessor) + "]" ;
            hilillosData += "Processor [" + QString::number(currentProcessor) + "]:\n";

            while (!processors[currentProcessor]->pcbFinishedQueue.empty())
            {
                   qDebug() << "\tHilillo" << processors[currentProcessor]->pcbFinishedQueue.front()->getID()   << " Registers " <<
                               processors[currentProcessor]->pcbFinishedQueue.front()->registers;
                   hilillosData += QString("\tHilillo") +
                   QString::number(processors[currentProcessor]->pcbFinishedQueue.front()->getID() )  + ": ";

                   for(size_t index = 0; index < processors[currentProcessor]->pcbFinishedQueue.front()->registers.size(); ++index)
                   {
                       hilillosData += "X" + QString::number(index)+ ":" +
                               QString::number(processors[currentProcessor]->pcbFinishedQueue.front()->registers.at(index)) + "| ";
                   }

                   hilillosData += '\n';
                   // Ciclos que tardo su ejecucion
                   // El valor del reloj cuando comenzo y termino
                   processors[currentProcessor]->pcbFinishedQueue.pop();
            }
        }

        qDebug() << "Emiting signal";
        emit emitResults(processorsData, hilillosData);

    }
}


void Processor::execute(int instruction[])
{
    //qDebug() << this->processorId << ": " << instruction[0] << "on cycle" << this->clock;
    switch(instruction[0])
    {
        case addi:
            if(instruction[1] == 16 && (registers[instruction[2]] + instruction[3] == 264))
                qDebug() << "addi x16, x0, 264";
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
            qDebug() << "fin in processor" << this->processorId << "and pc" << this->pc;
            currentState = contextSwitch;
            break;
        default:
            break;
    }
    if(instruction[0] != fin)
    {
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
            //qDebug() <<"lr";
            // Hace un load normal
            registers[instruction[1]] = dataCache.getDataAt(this, registers[instruction[2]]);
            // Le cambia el valor de rl a -1
            //qDebug() << this->rl;
            this->rl = registers[instruction[2]];// + instruction[3];
            //qDebug() << this->rl;
            break;
        case sc:

            // Calcula la posicion de memoria
            int mempos = registers[instruction[1]] + instruction[3];

            // Verifica si el rl es igual a la direccion de memoria de adonde voy a guardar
            if (this->rl == mempos )
            {
                //qDebug() << "rl save";
                dataCache.storeDataAt(this, registers[instruction[1]] + instruction[3], registers[instruction[2]]);
                // Entonces guardo en memoria....
                // El store en memoria tambien tiene que verificar si el RL del procesador
                // afecta el proceso
            }else {
                // Guardo un  0 en x2
                registers[instruction[2]] = 0;
                //qDebug() << "Couldnt get the lock";
                // y no escribe
            }
            break;
      //  default:
        //    break;
    }
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

    #ifdef STEP
    //if (this->processorId == 0)
        //qDebug() << "Clock: " << clock;
    #endif

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
            //qDebug() << "RL block value when invalidating processor " << this->processorId << ": " << rlBlock;

            if (rlBlock == currentMessage.blockToChangeState )
            {
                //qDebug() << "Changing rl state" << "on processor" << this->processorId;
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
        emit contextChange(this->processorId, oldPcb->getID() );


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
        emit contextChange(this->processorId, oldPcb->getID() );


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
