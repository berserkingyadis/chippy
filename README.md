# chippy
Chip-8 Emulator in C++ 

Since Java sucks for low level unsigned operations, I will port my Chip-8 Emulator to C++

## Dependencies

You will need SDL2 and cmake to compile on Windows or Linux. For Linux, ncurses is optional for debugging machine state.

```bash
mkdir build && cd build
cmake .. [-DWITH_CURSES](optional)
make
```

## Running the emulator

```bash
./chippy <path to ROM>
```

## Compiling under Linux

After installing SDL2 (and ncurses) you can just follow the steps under #Dependencies, cmake will find the dependencies.

## Compiling under Windows

You will need to download the [SDL2 develop libraries](https://www.libsdl.org/download-2.0.php). For cmake to find it follow [this tutorial](https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/).