#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <pthread.h>
#include <queue>
#include <QMutex>
#include <vector>
#include <QThread>

#include "cache.h"
#include "instructioncache.h"
#include "datacache.h"
#include "pcb.h"
#include "directory.h"



class Processor: public QThread
{
    Q_OBJECT

    enum MessageTypes
    {
        invalidate,
        leaveAsShared,
        ack,
    };
public:
    struct Message
    {
        MessageTypes opcode;
        int blockToChangeState;
        int* otherCacheBlock;
        size_t sendAckTo;
        Message(MessageTypes opcode, int blockToChangeState = -1, int* otherCacheBlock = nullptr, size_t sendAckTo = 0)
            : opcode{opcode}
            , blockToChangeState{blockToChangeState}
            , otherCacheBlock{otherCacheBlock}
            , sendAckTo{sendAckTo}
        {
        }
    };
private:
    enum PcbStates
    {
        wait,
        ready,
        running,
        finished
    };

    typedef struct
    {
        DirectoryStates state = dirUncached;
        char processor[3];
    } directoryBlock;

    enum ProcessorStates
    {
        instructionFetch,
        dataFetch,
        execution,
        contextSwitch,
        finish
    };

    enum Instructions
    {
        addi = 19,
        add = 71,
        sub = 83,
        mul = 72,
        div = 56,
        lw = 5,
        sw = 37,
        beq = 99,
        bne = 100,
        lr = 51,
        sc = 52,
        jal = 111,
        jalr = 103,
        fin = 999,
    };
protected:
    void run() override;
private:
    size_t processorId;
    int pc;
    bool loopCondition;
    std::vector<int> registers;
    std::vector<int> instructionMemory;
    std::vector<int> dataMemory;

    std::vector<directoryBlock> directory;
    QMutex directoryMutex;

    std::queue<Message> mail;
    std::queue<Message> messagesToProcess;
    QMutex messagesMutex;

    pthread_barrier_t* barrier;
    size_t clock;
    ProcessorStates currentState;

    InstructionCache instructionsCache;
    DataCache dataCache;
    // Esto despues lo cambiamos por la lista enlazada circular con nodo centinela :V
    std::queue<Pcb*> pcbRunningQueue;
    std::queue<Pcb*> pcbFinishedQueue;

    int rl;
    size_t currentQuatum;
    size_t maxQuatum;

public:
    Processor(const size_t id, const size_t quatum);

    friend class InstructionCache;
    friend class DataCache;
    std::vector<Processor*> processors;
    inline void pushPcb(Pcb* pcb) { pcbRunningQueue.push(pcb);}

    inline bool isMemoryInstruction(int& instructionCode)
    {
        return instructionCode == lw
                || instructionCode == sw
                || instructionCode == lr
                || instructionCode == sc;
    }

    void execute(int instruction[4]);

    void accessMemory(int instruction[4]);

    void advanceClockCycle();
    void processMessages(size_t* waitingAcks = nullptr);

    void init_barrier(pthread_barrier_t* barrier);

    void sendMessage(Message message, size_t processorId);

    void processAcks(size_t* waitingAcks);

    void makeContextSwitch(int instruction[4]);

    inline void execAddi(unsigned destinationRegister, unsigned sourceRegister, int immediate)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister] + immediate;
    }
    inline void execAdd(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] + this->registers[sourceRegister2];
    }
    inline void execSub(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] - this->registers[sourceRegister2];
    }
    inline void execMul(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] * this->registers[sourceRegister2];
    }
    inline void execDiv(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] / this->registers[sourceRegister2];
    }

    inline std::vector<int>* getInstructionMemory() {return &this->instructionMemory;}

    void execBeq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);
    void execBne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);

    void execJal(unsigned destinationRegister, int immediate);
    void execJalr(unsigned destinationRegister, unsigned sourceRegister, int immediate);
};

#endif // PROCESSOR_H
