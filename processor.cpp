#include "processor.h"

Processor::Processor():
    pc {0},
    rl {-1}
{
    registers.resize(32);
    instructionMemory.resize(64);
    dataMemory.resize(32);
    directory.resize(8);
}

void Processor::beq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate)
{
    if(this->registers[sourceRegister1] == this->registers[sourceRegister2])
        this->pc += immediate * 4;
}

void Processor::bne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate)
{
    if(this->registers[sourceRegister1] != this->registers[sourceRegister2])
        this->pc += immediate * 4;
}

void Processor::jal(unsigned destinationRegister, int immediate)
{
    this->registers[destinationRegister] = this->pc;
    this->pc += immediate;
}

void Processor::jalr(unsigned destinationRegister, unsigned sourceRegister, int immediate)
{
    this->registers[destinationRegister] = this->pc;
    this->pc = this->registers[sourceRegister] + immediate;
}
