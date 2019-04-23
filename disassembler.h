#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <iostream>

class Disassembler
{
public:
    Disassembler();
    std::string dis_opcode(uint16_t opcode) const;
};

#endif // DISASSEMBLER_H
