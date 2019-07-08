#include "pcb.h"

Pcb::Pcb():
    pc{0},
    rl{-1},
    id{-1},
    state{ready}
{
    this->registers.resize(32);
}


Pcb::Pcb(int pc, int id):
    pc{pc},
    rl{-1},
    id{id},
    state{ready}
{
    this->registers.resize(32);
}

void Pcb::saveState(const int pc, PcbStates state, int id, int rl,  std::vector<int>& registers)
{
    this->pc = pc;
    this->state = state;
    this->id = id;

    size_t index = 0;
    for (std::vector<int>::iterator begin = registers.begin(); begin != registers.end(); ++begin )
    {
        this->registers[index] = registers[index];
        ++index;
    }
}



void Pcb::restore(int &pc, int &rl, std::vector<int> &registers)
{
    pc = this->pc;
    //rl = this->rl;
    size_t index = 0;

    for (std::vector<int>::iterator begin = registers.begin(); begin != registers.end(); ++begin )
    {
        registers[index] = this->registers[index];
        ++index;
    }
}



