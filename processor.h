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
    typedef struct
    {
        char opcode;
        int block;
    } message;

    typedef struct
    {
        DirectoryStates state;
        char processor[3];
    } directoryBlock;

    enum ProcessorStates
    {
        instructionFetch,
        dataFetch,
        execution,
        contextSwitch,
    };

    enum MessageTypes
    {
        invalidate,
        leaveAsShared,
        ack,
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
    std::vector<int> registers;
    std::vector<int> instructionMemory;
    std::vector<int> dataMemory;

    std::vector<directoryBlock> directory;
    Processor* processors[3];

    std::queue<message> messages;
    QMutex messagesMutex;

    pthread_barrier_t* barrier;
    size_t clock;
    ProcessorStates currentState;

    InstructionCache instructionsCache;
    DataCache dataCache;
    // Esto despues lo cambiamos por la lista enlazada circular con nodo centinela :V
    std::vector<Pcb> pcbQueue;
    int rl;

public:
    Processor(const size_t id);

    friend class InstructionCache;
    friend class DataCache;



    inline bool isMemoryInstruction(int& instructionCode)
    {
        return instructionCode == lw
                || instructionCode == sw
                || instructionCode == lr
                || instructionCode == sc;
    }

    void execute(int instruction[4]);

    void advanceClockCycle();

    void init_barrier(pthread_barrier_t* barrier);

    void sendMessage(MessageTypes messageType);

    void processAcks(const size_t& waitingAcks);

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
