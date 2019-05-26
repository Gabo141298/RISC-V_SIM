#ifndef PCB_H
#define PCB_H
#include <vector>

class Pcb
{
public:
    private:
        int pc;
        std::vector<int> registers;
        int rl;
        int id;
        int state;
    public:
        Pcb();
        void saveState();
        void restoreState();
};

#endif // PCB_H
