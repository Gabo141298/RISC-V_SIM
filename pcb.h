#ifndef PCB_H
#define PCB_H
#include <vector>

class Pcb
{


public:

    /// Different states of a pcb
    enum PcbStates
    {
        wait,
        ready,
        running,
        finished
    };
    private:

    public:

    /// Stores the pc, registers, state of the hilillo
    int pc;
    std::vector<int> registers;
    int rl;
    int id;
    int firstCycle;
    int lastCycle;
    PcbStates state;

    /// Default constructor
    Pcb();
    /**
     * @brief Pcb initializes a pc with an id and a pc
     * @param pc program counter
     * @param id the id of the hilillo
     */
    Pcb(int pc, int id);
    inline int getID(){return this->id;}
    // ID, pc, registros, estado. (no se para que el estado)
    /**
     * @brief saveState saves the state of a pcb given the paramaters, when a context
     * switch occurs or when a program ends its execution
     * @param pc
     * @param state
     * @param id
     * @param rl
     * @param registers
     */
    void saveState(const int pc, PcbStates state, int id, int rl,  std::vector<int>& registers, int lastCycle);

    inline void setBegginingClock(int clock) { this->firstCycle = clock;}
    /**
     * @brief restore restores the state of a PCB
     * @param pc
     * @param rl
     * @param registers
     */
    void restore(int& pc,int& rl, std::vector<int>& registers );
};

#endif // PCB_H
