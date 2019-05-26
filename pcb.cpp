#include "pcb.h"

Pcb::Pcb():
    pc{0},
    rl{-1},
    id{-1},
    state{-1}
{
    this->registers.resize(32);
}
