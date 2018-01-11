# chippy
Chip-8 Emulator in C++ 

Since Java sucks for low level unsigned operations, I will port my Chip-8 Emulator to C++

## Dependencies

You will need SDL2, ncurses and cmake to compile.

```bash
mkdir build && cd build
cmake .. [-DWITHOUT_CURSES=1](optional)
make
```

## Running the emulator

```bash
./chippy <path to ROM>
```

## Crosscompiling for Windows

There is a script called ```crosscompile_win64.sh``` in the repo which compiles the emulator for windows. For it to work you will need the following packages installed:

* mingw-w64-gcc
* mingw-w64-sdl2
* mingw-w64-winpthreads
* mingw-w64-pdcurses

For the crosscompiled executable to work, you need to put the dll's from the folder ```redist_win``` in the same folder as the exe.
