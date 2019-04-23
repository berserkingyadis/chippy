#include <iostream>

#include "CPU.h"

#define VERSION "0.2.1"

static const char* OP_HELP = "--help";
static const char* OP_HELP_S = "-h";

static const char* OP_VERSION = "--version";
static const char* OP_VERSION_S = "-v";

//not yet implemented
//static const char* OP_DISASSEMBLE = "--disassemble";
//static const char* OP_DISASSEMBLE_S = "-d";


void print_help();
void print_version();

int main(int argc, char *argv[]) {

	if (argc == 1) {
		std::cerr << "Error: Path to ROM-file expected." << std::endl << std::endl;
		print_help();
		exit(1);
	}
	if (argc > 2) {
		std::cerr << "Error: Too many arguments." << std::endl << std::endl;
		print_help();
		exit(1);
	}

	// parse program arguments and if applicable print info and exit
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], OP_HELP) == 0 || strcmp(argv[i], OP_HELP_S) == 0) {
			print_help();
			return 0;
		}
		else if (strcmp(argv[i], OP_VERSION) == 0 || strcmp(argv[i], OP_VERSION_S) == 0) {
			print_version();
			return 0;
		}
	}

	CPU cpu;
	std::cout << "rom path is '" << argv[1] << "'." << std::endl;
	if (cpu.load_rom(argv[1]))cpu.start();
	std::cout << "End of main reached. Have a nice day." << std::endl;
	return 0;
}


void print_help() {
	std::cout << "chippy is a chip8 emulator, you can call it with:" << std::endl;
	std::cout << "./chippy <Name of ROM-File>" << std::endl;
	std::cout << std::endl;
	std::cout << "Optional parameters: " << std::endl;
	std::cout << "-v or --v to display the version of the emulator" << std::endl;
	std::cout << "-h or --h to display this message" << std::endl;
}
void print_version() {
	std::cout << "chippy version " << VERSION << std::endl;
}
