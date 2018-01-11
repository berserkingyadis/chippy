//
// Created by bers on 11/26/16.
//

#ifndef CHIPPY_CPU_H
#define CHIPPY_CPU_H

#include <stack>
#include <cstdint>

#include <SDL2/SDL.h>

#include "beeper.h"

#ifndef WITHOUT_CURSES
#include <curses.h>
#include "disassembler.h"
#endif

#define OPERATIONS_PER_SECOND 100


#define MEM_SIZE 4096
#define AMOUNT_REGISTERS 16
#define INSERT_ROM_ADDRESS 0x200
#define STACK_MAX_SIZE 16

#define SCALE 10
#define WIDTH 64
#define HEIGHT 32
#define DISPLAY_WIDTH WIDTH * SCALE
#define DISPLAY_HEIGHT HEIGHT * SCALE

#define CPU_RUNMODE_STEP 0

//for printing debug information
#ifndef HEX
#define HEX( x , s ) "0x" << std::setw(s) << std::setfill('0') << std::hex << (unsigned)(x) << std::dec
#endif



#define WINDOW_TITLE "chippy chip8-emulator"



class CPU {
public:
    CPU();
    ~CPU();
    bool load_rom(const char* file);
    void start();

    void dump_machine() const;
    void dump_cpu() const;
    void dump_memory() const;
    void dump_memory(unsigned int offset, unsigned int length) const;
private:
    void init();
    void process();

    void set_variables();

#ifndef WITHOUT_CURSES
    void init_curses();
    void update_curses();
    void destroy_curses();

     Disassembler dis;
#endif

    void draw_logo();
    bool draw_sprite(uint8_t sprite, uint8_t pos_x, uint8_t pos_y);
    void handle_events();

    uint8_t last_pressed;
    bool wait_press = false;

    bool draw = false;

    void process_key_press(SDL_Keycode pressed);
    void process_timers();

    void render() const;

    void beep() const;

    void cleanup_exit();

    SDL_Window* gWindow = NULL;

    SDL_Surface* gScreenSurface = NULL;

    SDL_Renderer* gRenderer = NULL;

    SDL_RWops *logo_data  = NULL;

    SDL_Surface* gLogo = NULL;

    Beeper beeper;

    void sleep_for_ms(uint32_t ms);
    void flush_screen();

    uint8_t font_data[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, //0
            0x20, 0x60, 0x20, 0x20, 0x70, //1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
            0x90, 0x90, 0xF0, 0x10, 0x10, //4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
            0xF0, 0x10, 0x20, 0x40, 0x40, //7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
            0xF0, 0x90, 0xF0, 0x90, 0x90, //A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
            0xF0, 0x80, 0x80, 0x80, 0xF0, //C
            0xE0, 0x90, 0x90, 0x90, 0xE0, //D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
            0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };

    uint8_t memory[MEM_SIZE];
    uint8_t registers[AMOUNT_REGISTERS];

    bool display[WIDTH][HEIGHT];

    uint16_t I;
    uint8_t sound_timer;
    uint8_t delay_timer;

    uint16_t program_counter;

    uint16_t opcode;

    uint8_t n;
    uint8_t nn;
    uint16_t nnn;

    uint8_t x;
    uint8_t y;

    uint8_t v_x;
    uint8_t v_y;

    std::stack<uint16_t> _stack;

    uint32_t ops = OPERATIONS_PER_SECOND;

    uint32_t file_size;
};


#endif //CHIPPY_CPU_H
