#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <pthread.h>
#include <queue>
#include <QMutex>
#include <vector>
#include <QThread>
#define STEP
#include "cache.h"
#include "instructioncache.h"
#include "datacache.h"
#include "pcb.h"
#include "directory.h"



class Processor: public QThread
{
    Q_OBJECT

    /// CAlificación de cada tipo de mensaje
    enum MessageTypes
    {
        invalidate,
        leaveAsShared,
        ack,
    };

public:

    /**
     * @brief The Message struct es el mecanismo the comunicación
     * utilizado entre los procesadores para mantener coherencia
     * de caches
     */
    struct Message
    {
        /// Tipo de mensaje
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
    /// Estados que puede tener un PCB
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

    /// Codificación de las instrucciones
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
public:
    /// Cola que contiene los hilillos que ya han ido terminando
    std::queue<Pcb*> pcbFinishedQueue;
signals:
     /// Senales que se comunican con simulaton manager, para enviar datos necesarios
     /// a la interfaz gráfica, como desplegar los resultados o los cambios de contexto
     /// entre los hilillos
     void contextChange(const int processor, const int hilillo);
     void emitResults(const QString processorsData, const QString hilillosData);

protected:
     /// Método default para invocar a un thead
    void run() override;

private:
    /// Identificador del procesador
    size_t processorId;
    /// Program counter
    int pc;
    /// Condicion que rompe el loop principal de los hilillos
    bool loopCondition;
    /// Registros
    std::vector<int> registers;
    /// Memoria instrucciones
    std::vector<int> instructionMemory;
    /// Parte que le corresponde de la memoria compartida de datos
    std::vector<int> dataMemory;

    /// Directorio casa de cada procesador
    std::vector<directoryBlock> directory;
    /// Mutex del directorio
    QMutex directoryMutex;

    /// Lista de mensajes por enviar
    std::queue<Message> mail;
    /// Lista de mensajes por procesar
    std::queue<Message> messagesToProcess;
    /// Mutex que protege la comuniación
    QMutex messagesMutex;

    /// Barrera que mantiene a todos en sincronia
    pthread_barrier_t* barrier;
    /// Reloj del procesador
    size_t clock;
    /// Estados actual del procesador
    ProcessorStates currentState;

    /// Caches de instrucciones y de datos
    InstructionCache instructionsCache;
    DataCache dataCache;
    /// Cola que contiene de los pcbs de los programas que aun no han terminado
    std::queue<Pcb*> pcbRunningQueue;

    /// Registro especial RL
    int rl;
    /// Quatum actual del procesador
    size_t currentQuatum;
    /// Quatum máximo permitido
    size_t maxQuatum;

public:

    /**
     * @brief Processor construye un procesador con una identificacion
     * y un quatum maximo asignado por simulation manager
     * @param id
     * @param quatum
     */
    Processor(const size_t id, const size_t quatum);

    friend class InstructionCache;
    friend class DataCache;

    /// Arreglo con punteros a cada uno de los procesadores
    std::vector<Processor*> processors;
    /// Agrega al final de la lista de pcbs corriendo un pcb
    inline void pushPcb(Pcb* pcb) { pcbRunningQueue.push(pcb);}

    /**
     * @brief isMemoryInstruction verifica si el codigo de operacion dado
     * corresponde a una instrucción de memoria
     * @param instructionCode
     * @return
     */
    inline bool isMemoryInstruction(int& instructionCode)
    {
        return instructionCode == lw
                || instructionCode == sw
                || instructionCode == lr
                || instructionCode == sc;
    }

    /// Switch en el que se verifica cual operacion se va a realizar
    void execute(int instruction[4]);
    /// Switch en el que se ejecutan las operaciones correspondientes a memoria
    void accessMemory(int instruction[4]);
    /// COnstruye los resultados en un String, para desplegarlos en la UI
    void makeResults();
    /// Avanza los ciclos de reloj, y ademas procesa mensajes
    void advanceClockCycle();
    /// Método que se encarga de procesar los mensajes que le esten pendientes
    /// en su cola
    void processMessages(size_t* waitingAcks = nullptr);
    ///  Inicialización de la barrera
    void init_barrier(pthread_barrier_t* barrier);
    /// Envia un mensaje, apropiandose del mail
    void sendMessage(Message message, size_t processorId);
    /// Procesamientos de las confirmaciónes recibidas
    void processAcks(size_t* waitingAcks);
    /// Se encarga de hacer un cambio de contexto, ya se si la ejecución
    /// del programa termino por completo, o solo se desea sacar de contexto
    /// porque termino su quantum
    void makeContextSwitch(int instruction[4]);

    /// Implementacion de las operaciones aritmeticas, condionales-branches
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
    void execBeq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);
    void execBne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);

    void execJal(unsigned destinationRegister, int immediate);
    void execJalr(unsigned destinationRegister, unsigned sourceRegister, int immediate);

    /// Devuelve un puntero a la memoria de instrucciones
    inline std::vector<int>* getInstructionMemory() {return &this->instructionMemory;}

};

#endif // PROCESSOR_H
