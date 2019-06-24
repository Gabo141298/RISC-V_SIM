#ifndef PCB_H
#define PCB_H
#include <vector>

class Pcb
{


public:
    enum PcbStates
    {
        wait,
        ready,
        running,
        finished
    };
    private:

    public:
    int pc;
    std::vector<int> registers;
    int rl;
    int id;
    PcbStates state;
        Pcb();
        Pcb(size_t pc, int id);
        inline int getID(){return this->id;}
        // ID, pc, registros, estado. (no se para que el estado)
        void saveState(const int pc, PcbStates state, int id, int rl,  std::vector<int>& registers);
        void restoreState(int& pc, PcbStates &state, int &id, int &rl,  std::vector<int>& registers);
        void restore(int& pc,int& rl, std::vector<int>& registers );
};

#endif // PCB_H
