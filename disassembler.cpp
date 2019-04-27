#include "disassembler.h"
#include <iomanip>
#include <sstream>

#ifndef HEX
#define HEX( x , s ) "0x" << std::setw(s) << std::setfill('0') << std::hex << (unsigned)(x) << std::dec
#endif

Disassembler::Disassembler()
{


}
std::string Disassembler::dis_opcode(uint16_t opcode) const
{
	uint16_t nnn = opcode & 0x0FFF;
	uint8_t nn = opcode & 0x00FF;
	uint8_t n = opcode & 0x000F;
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;

	//ss << HEX(opcode, 4) << "     # ";


	std::stringstream ss;

	switch (opcode & 0xF000) {

		/*
				0NNN	Execute machine language subroutine at address NNN
						This instruction is only used on the old computers on which Chip-8 was originally implemented. It is ignored by modern interpreters.
				00E0	Clear the screen
				00EE	Return from a subroutine
						The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
				 */
	case 0x0000:
		switch (opcode & 0x00FF) {
		case 0xE0:
			ss << "00E0 - clear the screen";
			break;
		case 0xEE:
			ss << "00EE - return from subroutine";
			break;
		default:
			ss << "not supported (data?)";
			break;
		}
		break;

		/*
			   1NNN	Jump to address NNN
					   The interpreter sets the program counter to nnn.
				*/
	case 0x1000:
		ss << "1NNN - jump to address " << HEX(nnn, 4);
		break;

		/*
			   2NNN	Execute subroutine starting at address NNN
					   The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
				*/
	case 0x2000:
		ss << "2NNN - execute subcoutine starting at adress " << HEX(nnn, 4);
		break;

		/*
			3XNN	Skip the following instruction if the value of register VX equals NN
			 */
	case 0x3000:
		ss << "3XNN - skip next instruction if register V" << HEX(x, 2) << " equals " << HEX(nn, 2);
		break;

		/*
			4XNN	Skip the following instruction if the value of register VX is not equal to NN
			*/
	case 0x4000:
		ss << "4XNN - skip next instruction if register V" << HEX(x, 2) << " not equals " << HEX(nn, 2);
		break;

		/*
			5XY0	Skip the following instruction if the value of register VX is equal to the value of register VY
			 */
	case 0x5000:
		ss << "5XY0 - skip next instruction if register V" << HEX(x, 2) << " not equals registerV " << HEX(y, 2);
		break;
		/*
		   6XNN	Store number NN in register VX
			*/
	case 0x6000:
		ss << "6XNN - store number " << HEX(nn, 2) << " in register V" << HEX(x, 2);
		break;
		/*
			7XNN	Add the value NN to register VX
			 */
	case 0x7000:
		ss << "7XNN - add " << HEX(nn, 2) << " to register V" << HEX(x, 2);
		break;
		/*
			8XY0	Store the value of register VY in register VX
			8XY1	Set VX to VX OR VY
			8XY2	Set VX to VX AND VY
			8XY3	Set VX to VX XOR VY
			8XY4	Add the value of register VY to register VX
					Set VF to 01 if a carry occurs
					Set VF to 00 if a carry does not occur
			8XY5	Subtract the value of register VY from register VX
				  8XY0  Set VF to 00 if a borrow occurs
					Set VF to 01 if a borrow does not occur
			8XY6	Store the value of register VY shifted right one bit in register VX
					Set register VF to the least significant bit prior to the shift
			8XY7    Set register VX to the value of VY minus VX
					Set VF to 00 if a borrow occurs
					Set VF to 01 if a borrow does not occur
			8XYE	Store the value of register VY shifted left one bit in register VX
					Set register VF to the most significant bit prior to the shift
			 */
	case 0x8000:
		switch (n) {
		case 0:
			ss << "8XY0 - store the value of register V" << HEX(y, 2) << " in register V" << HEX(x, 2);
			break;
		case 1:
			ss << "8XY1 - set register V" << HEX(x, 2) << " to register V" << HEX(x, 2) << " OR register V" << HEX(y, 2);
			break;
		case 2:
			ss << "8XY2 - set register V" << HEX(x, 2) << " to register V" << HEX(x, 2) << " AND register V" << HEX(y, 2);
			break;
		case 3:
			ss << "8XY3 - set register V" << HEX(x, 2) << " to register V" << HEX(x, 2) << " XOR register V" << HEX(y, 2);
			break;
		case 4:
			ss << "8XY4 - add the value of register V" << HEX(y, 2) << " to register V" << HEX(x, 2) << " (register VF set to 1 if carry)";
			break;
		case 5:
			ss << "8XY5 - subtract the value of register V" << HEX(y, 2) << " from register V" << HEX(x, 2) << " (register 0xF set to 1 if not borrow)";
			break;
		case 6:
			ss << "8XY6 - store the value of register V" << HEX(y, 2) << " shifted right 1 bit in register V" << HEX(x, 2) << " (register 0xF = LSB)";
			break;
		case 7:
			ss << "8XY7 - subtract the value of register " << HEX(x, 2) << " from register " << HEX(y, 2) << " (register 0xF set to 1 if not borrow)";
			break;
		case 0xE:
			ss << "8XYE - store the value of register " << HEX(y, 2) << " shifted left 1 bit in register " << HEX(x, 2) << " (register 0xF = MSB)";
			break;

		}
		break;
		/*
			   9XY0	Skip the following instruction if the value of register VX is not equal to the value of register VY
			*/
	case 0x9000:
		ss << "9XY0 - skip the next instruction if values of registers " << HEX(x, 2) << " and " << HEX(y, 2) << " differ";
		break;
		/*
			ANNN	Store memory address NNN in register I
			 */
	case 0xA000:
		ss << "ANNN - store address " << HEX(nnn, 4) << " in register I";
		break;
		/*
		   BNNN	Jump to address NNN + V0
			*/
	case 0xB000:
		ss << "BNNN - jump to adress " << HEX(nnn, 4) << " + value of register 0x00";
		break;
		/*
				CXNN	Set VX to a random number with a mask of NN
				 */
	case 0xC000:
		ss << "CXNN - set register " << HEX(x, 2) << " to a random number with bitmask " << HEX(nn, 2);
		break;
		/*
			DXYN	Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
					Set VF to 01 if any set pixels are changed to unset, and 00 otherwise

					If the sprite is to be visible on the screen,
					the VX register must contain a value between 00 and 3F, (0-63)
					and the VY register must contain a value between 00 and 1F. (0-31)
			 */
	case 0xD000:
		ss << "DXYN - draw a sprite at screen position x = register " << HEX(x, 2) << " y = register " << HEX(y, 2) <<
			" with " << HEX(n, 2) << " bytes of sprite data at adress I";
		break;
		/*
		   EX9E	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
		   EXA1	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
			*/
	case 0xE000:
		switch (opcode & 0x000F) {

		case 0xE:
			ss << "EX9E - skip next instruction if key in register " << HEX(x, 2) << " is pressed";
			break;
		case 0x1:
			ss << "EXA1 - skip next instruction if key in register " << HEX(x, 2) << " is not pressed";
			break;
		}
		break;

		/*
		   FX07	Store the current value of the delay timer in register VX
		   FX0A	Wait for a keypress and store the result in register VX
		   FX15	Set the delay timer to the value of register VX
		   FX18	Set the sound timer to the value of register VX
		   FX1E	Add the value stored in register VX to register I
		   FX29	Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
		   FX33	Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I+1, and I+2
		   FX55	Store the values of registers V0 to VX inclusive in memory starting at address I
				   I is set to I + X + 1 after operation
		   FX65	Fill registers V0 to VX inclusive with the values stored in memory starting at address I
				   I is set to I + X + 1 after operation
			*/
	case 0xF000:
		switch (opcode & 0x00FF) {
		case 0x07:
			ss << "FX07 - store the value of the delay timer in register " << HEX(x, 2);
			break;
		case 0x0A:
			ss << "FX0A - wait for a keypress and store the result in register " << HEX(x, 2);
			break;
		case 0x15:
			ss << "FX15 - set the delay timer to the value of register " << HEX(x, 2);
			break;
		case 0x18:
			ss << "FX18 - set the sound timer to the value of register " << HEX(x, 2);
			break;
		case 0x1E:
			ss << "FX1E - add the value of register " << HEX(x, 2) << " to register I";
			break;
		case 0x29:
			ss << "FX29 - set I to the adress of the sprite data of register " << HEX(x, 2);
			break;
		case 0x33:
			ss << "FX33 - store number from register " << HEX(x, 2) << " in I, I+1 and I+3";
			break;
			/*
			FX55	Store the values of registers V0 to VX inclusive in memory starting at address I
					I is set to I + X + 1 after operation
					*/
		case 0x55:
			ss << "FX55 - store the values of registers 0x00 to " << HEX(x, 2) << " in memory starting at I";
			break;
			/*
				   FX65	Fill registers V0 to VX inclusive with the values stored in memory starting at address I
				   I is set to I + X + 1 after operation
				   */
		case 0x65:
			ss << "FX65 - fill registers 0x00 to " << HEX(x, 2) << "with values stored in memory startint at I";
			break;

		}
		break;

	}

	std::string temp = ss.str();
	return temp.c_str();
}
