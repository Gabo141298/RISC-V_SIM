#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor
{
private:
    int pc;
    std::vector<int> registers;
    int rl;
public:
    Processor();

    inline void addi(unsigned destinationRegister, unsigned sourceRegister, int immediate)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister] + immediate;
    }
    inline void add(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] + this->registers[sourceRegister2];
    }
    inline void sub(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] - this->registers[sourceRegister2];
    }
    inline void mul(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] * this->registers[sourceRegister2];
    }
    inline void div(unsigned destinationRegister, unsigned sourceRegister1, unsigned sourceRegister2)
    {
        this->registers[destinationRegister] = this->registers[sourceRegister1] / this->registers[sourceRegister2];
    }

    void beq(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);
    void bne(unsigned sourceRegister1, unsigned sourceRegister2, int immediate);

    void jal(unsigned destinationRegister, int immediate);
    void jalr(unsigned destinationRegister, unsigned sourceRegister, int immediate);
};

#endif // PROCESSOR_H
