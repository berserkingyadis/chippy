#!/bin/bash

x86_64-w64-mingw32-g++ -Wall -o chip8.exe main.cpp CPU.cpp disassembler.cpp beeper.cpp -lmingw32 -lSDL2main -lSDL2 -DWITHOUT_CURSES
