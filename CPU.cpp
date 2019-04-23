#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>

#include "CPU.h"
#include "logo.h"
#include "timer.h"

CPU::CPU() {
	if (!init())exit(1);
}

CPU::~CPU() {}

bool CPU::init() {

	//Reset everything
	for (size_t i = 0; i < AMOUNT_REGISTERS; ++i) {
		registers[i] = 0;
	}
	for (size_t i = 0; i < MEM_SIZE; ++i) {
		memory[i] = 0;
	}

	erase_screen();

	//storing the font data in the memory from 0x000 on
	for (size_t i = 0; i < 80; ++i) {
		memory[i] = font_data[i];
	}
	I = 0;
	sound_timer = 0;
	delay_timer = 0;

	program_counter = INSERT_ROM_ADDRESS;
	opcode = 0;
	n = 0;
	nn = 0;
	nnn = 0;
	x = 0;
	y = 0;
	v_x = 0;
	v_y = 0;

	_stack.empty();

	//seed the random number generator
	srand(time(NULL));

	//init VIDEO
	std::cout << "initiating sdl..";
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Cannot init SDL_VIDEO, exiting." << std::endl;
		return false;
	}

	//init AUDIO
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "Cannot init SDL_AUDIO"
			<< ", exiting." << std::endl;
		return false;
	}

	gWindow = SDL_CreateWindow(WINDOW_TITLE, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	//load and scale logo to window resolution
	logo_data = SDL_RWFromConstMem(IMAGE_DATA, sizeof(IMAGE_DATA));
	gLogo = SDL_LoadBMP_RW(logo_data, 1);

	SDL_Rect stretchRect;
	stretchRect.x = 0;
	stretchRect.y = 0;
	stretchRect.w = DISPLAY_WIDTH;
	stretchRect.h = DISPLAY_HEIGHT;

	//display logo
	SDL_BlitScaled(gLogo, NULL, gScreenSurface, &stretchRect);

	draw = true;

	return true;
}

void CPU::dump_machine() const
{
	std::cout << std::endl << "Dumping the emulator state:" << std::endl;
	dump_cpu();
	dump_memory();
}

#ifdef WITH_CURSES
void CPU::update_curses()
{
	clear();
	std::stringstream ss;

	ss << "=== chippy machine state " << std::endl << std::endl;
	ss << "== program counter: " << HEX(program_counter, 4) << std::endl;
	ss << "== opcode: " << HEX(opcode, 4) << std::endl;
	ss << dis.dis_opcode(opcode).c_str();
	ss << std::endl;

	//if opcode is drawing related, get the sprite data from memeory to display
	//the sprite for debug purposes

	std::stack<uint16_t> stack_c = _stack;
	uint16_t stack_contents[_stack.size()];
	for (size_t i = 0; i < _stack.size(); i++) {
		stack_contents[i] = stack_c.top();
		stack_c.pop();
	}

	ss << "== cpu registers:                  == stack " << std::endl;
	for (size_t i = 0; i < AMOUNT_REGISTERS; i++) {
		ss << " register[" << HEX(i, 2) << "] = " << HEX(registers[i], 2);
		if (i < _stack.size()) {
			ss << "              ";

			ss << HEX(stack_contents[i], 4);
		}
		ss << std::endl;
	}
	ss << std::endl;
	ss << " register I = " << HEX(I, 4) << std::endl;
	ss << " delay timer = " << HEX(delay_timer, 2) << std::endl;
	ss << " sound timer = " << HEX(sound_timer, 2) << std::endl;

	ss << std::endl;
	ss << "== (rom)memory: (" << (file_size / 2) << " operations) " << std::endl;

	float amount_adr = file_size / 2;
	int num_cols = 4;

	int num_rows = std::ceil(amount_adr / num_cols);

	for (size_t i = 0; i < num_rows; i++) {
		for (size_t col = 0; col < num_cols; col++) {

			uint16_t offset = i + (num_rows*col);
			if (offset < amount_adr) {
				uint16_t offset_adr = INSERT_ROM_ADDRESS + (offset * 2);
				uint16_t op = (memory[offset_adr] << 8) | memory[offset_adr + 1];
				ss << "[$" << HEX(offset_adr, 4) << "]";
				if (offset_adr == program_counter) ss << "=PC=";
				else ss << "====";
				ss << HEX(op, 4) << "   ";

			}

		}
		ss << std::endl;
	}
	if (CPU_RUNMODE_STEP)ss << std::endl << "press a key for next cpu cycle .. ";
	printw(ss.str().c_str());
	if (CPU_RUNMODE_STEP) {
		SDL_Event event;
		while (SDL_WaitEvent(&event)) {
			if (event.type == SDL_QUIT)cleanup_exit();
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE)cleanup_exit();
				else {
					process_key_press(event.key.keysym.sym);
					break;

				}
			}
		}
	}



	refresh();

}
void CPU::init_curses()
{
	initscr();
	(void)echo();
}

#endif
void CPU::dump_memory() const
{
	std::cout << std::endl << "== dumping the memory state:" << std::endl;

	for (uint32_t i = 0x200; i < (file_size + 0x200); i += 2) {
		for (int col = 0; col < 4; col++) {
			uint16_t op = (memory[i] << 8) | memory[i + 1];
			std::cout << "[$" << HEX(i, 4) << "] "
				<< HEX(op, 4);
			if (op == opcode) std::cout << " <-- program counter ";
		}
		std::cout << std::endl;
	}
}

void CPU::dump_cpu() const
{
	std::cout << std::endl << "== dumping the cpu state" << std::endl;
	std::cout << "== program counter: " << HEX(program_counter, 4) << std::endl;
	std::cout << "== opcode:" << std::endl;
	std::cout << "== registers:" << std::endl;
	for (int i = 0; i < AMOUNT_REGISTERS; i++) {
		std::cout << "register V" << i << " = " << HEX(registers[i], 2) << std::endl;
	}
	std::cout << "== special registers:" << std::endl;
	std::cout << "register I = " << HEX(I, 4) << std::endl;
}

bool CPU::load_rom(const char* file_name) {

	std::ifstream file(file_name, std::ios::in | std::ios::binary);

	if (file.is_open()) {
		file.seekg(0, std::ios::end);
		file_size = file.tellg();

		//is the rom too big for the RAM?
		if (file_size < 0xFFF - 0x200) {
			file.seekg(0, std::ios::beg);
			file.read((char*)&memory[INSERT_ROM_ADDRESS], file_size);
			std::cout << file.gcount() << " bytes were written into memory." << std::endl;
		}
		else {
			std::cerr << "rom is " << file_size << " bytes , maximum allowed rom size is " << 0xFFF - 0x200 << " bytes"
				<< std::endl;
			file.close();
			return false;
		}
	}
	else {
		std::cerr << "Error while opening file from '" << file_name << "'" << std::endl;
		file.close();
		return false;
	}
	file.close();
	return true;
}

void CPU::start() {

	//show logo for half a second
	uint32_t cycle_num = 0, cycle_performance = 0;

	bool logo = true;
	SDL_SetWindowTitle(gWindow, WINDOW_TITLE" - welcome!");

	uint32_t elapsed = 0, elapsed_performance = 0, lastFrame = 0;
	uint32_t logo_duration = MICROSECONDS_PER_SECOND / 2; // we want to see the logo half a second (if at all lol)
	timer.reset();
	while (logo) {
		draw_logo();
		elapsed += timer.framedeltaMicroseconds();
		if (elapsed >= logo_duration)logo = false;
	}

#ifdef WITH_CURSES
	init_curses();
#endif

	bool running = true;
	uint32_t timer_freq = MICROSECONDS_PER_SECOND / TIMER_FREQUENCY; //timers have to decrement once every timer_freq (16666 microseconds)
	elapsed = 0;

	lastFrame = 0;


	timer.reset();
	while (running) {
		++cycle_performance;
		++cycle_num;

		//update title bar
		char buf[50];
		sprintf(buf, WINDOW_TITLE" - CPU cycle nr. %d", cycle_num);
		SDL_SetWindowTitle(gWindow, buf);

		//inputs
		if (!handle_events())running = false;

		if (elapsed > timer_freq) {
			process_timers();
			elapsed -= timer_freq;
		}
		process();
		if (draw)render();


		lastFrame = timer.framedeltaMicroseconds();
		elapsed += lastFrame;
		elapsed_performance += lastFrame;
		if (elapsed_performance >= MICROSECONDS_PER_SECOND) {
			amountCirclesPerSecond.push_back(cycle_performance);
			std::cout << "op/sec = " << cycle_performance << std::endl;
			elapsed_performance -= MICROSECONDS_PER_SECOND;
			cycle_performance = 0;
		}
	}

	uint32_t op_per_sec = 0;
	for (long const& value : amountCirclesPerSecond) {

		op_per_sec += value;
	}
	op_per_sec /= amountCirclesPerSecond.size();
	std::cout << "average op/second = " << op_per_sec << std::endl;

	std::cout << "Shutting down emulator.." << std::endl;
	cleanup_exit();
}

void CPU::process_timers() {
	if (delay_timer > 0)delay_timer--;

	if (sound_timer > 0) {
		sound_timer--;
		//pwease dont bee to loud T_T
		beeper.beep(800, 100);
	}
}
bool CPU::handle_events() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		//was a key pressed?
		switch (event.type) {
		case SDL_KEYDOWN:
			return process_key_press(event.key.keysym.sym);
			break;
		case SDL_QUIT:
			std::cout << "SDL_QUIT recieved." << std::endl;
			return false;
			break;
		default:
			break;

		}
	}
	return true;
}

bool CPU::process_key_press(SDL_Keycode pressed)
{

	/*
	 *
	 * The computers which orginally used in the CHIP8 has 16-key gexadecimal keypad:
	 *            _________
	 *           |1|2|3|C|
	 *           |4|5|6|D|
	 *           |7|8|9|E|
	 *           |A|0|B|F|
	 *
	 *  TODO: make keys configurable
	 */


	bool esc_pressed = false;

	switch (pressed) {
	case SDLK_1:
		last_pressed = 0x1;
		wait_press = false;
		break;
	case SDLK_2:
		last_pressed = 0x2;
		wait_press = false;
		break;
	case SDLK_3:
		last_pressed = 0x3;
		wait_press = false;
		break;
	case SDLK_4:
		last_pressed = 0xc;
		wait_press = false;
		break;
	case SDLK_q:
		last_pressed = 0x4;
		wait_press = false;
		break;
	case SDLK_w:
		last_pressed = 0x5;
		wait_press = false;
		break;
	case SDLK_e:
		last_pressed = 0x6;
		wait_press = false;
		break;
	case SDLK_r:
		last_pressed = 0xd;
		wait_press = false;
		break;
	case SDLK_a:
		last_pressed = 0x7;
		wait_press = false;
		break;
	case SDLK_s:
		last_pressed = 0x8;
		wait_press = false;
		break;
	case SDLK_d:
		last_pressed = 0x9;
		wait_press = false;
		break;
	case SDLK_f:
		last_pressed = 0xe;
		wait_press = false;
		break;
	case SDLK_y:
		last_pressed = 0xa;
		wait_press = false;
		break;
	case SDLK_z:
		last_pressed = 0xa;
		wait_press = false;
		break;
	case SDLK_x:
		last_pressed = 0x0;
		wait_press = false;
		break;
	case SDLK_c:
		last_pressed = 0xb;
		wait_press = false;
		break;
	case SDLK_v:
		last_pressed = 0xf;
		wait_press = false;
		break;
	case SDLK_ESCAPE:
		std::cout << "Escape key was pressed." << std::endl;
		esc_pressed = true;
		break;
	default:
		break; //do nothing
	}

	return !esc_pressed;
}

void CPU::render() {

	//draw the background (black)
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	SDL_Rect background = { 0,0, DISPLAY_WIDTH, DISPLAY_HEIGHT };
	SDL_RenderFillRect(gRenderer, &background);

	//draw the set pixels
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
	SDL_Rect rec;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (display[i][j]) {
				rec.x = i * SCALE;
				rec.y = j * SCALE;
				rec.w = SCALE;
				rec.h = SCALE;
				SDL_RenderFillRect(gRenderer, &rec);
			}
		}
	}
	SDL_RenderPresent(gRenderer);
	draw = false;
}

void CPU::sleep_for_ms(uint32_t ms)
{
	SDL_Delay(ms);
}
bool CPU::draw_sprite(uint8_t sprite, uint8_t pos_x, uint8_t pos_y)
{
	//draw sprite bytes at positions pos_x, pos_y , return true if a pixel was erased.
	uint8_t bitmask = 0x01;

	bool erased = false;

	for (int8_t i = 7; i >= 0; i--) {
		if ((sprite&bitmask) > 0) {
			if (display[(pos_x + i)][pos_y] == true)erased = true;
			display[(pos_x + i)][pos_y] ^= true;
		}
		bitmask <<= 1;
	}
	return erased;
}

void CPU::process() {

	set_variables();

#ifdef WITH_CURSES
	update_curses();
#endif

	program_counter += 2;

	bool unset;
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
			//Clear the screen
			erase_screen();
			draw = true;
			break;
		case 0xEE:
			//return from a subroutine
			if (_stack.size() == 0) {
				std::cerr << "There is no subroutine to return to, exiting." << std::endl;
				cleanup_exit();
			}
			program_counter = _stack.top();
			_stack.pop();
			break;
		default:
			//OPCODE NOT SUPPORTED
			std::cerr << "The opcode " << HEX(opcode, 4) << " at $" << HEX(program_counter - 2, 4) << " is not supported. Exiting." << std::endl;
			cleanup_exit();
			break;
		}
		break;

		/*
			   1NNN	Jump to address NNN
					   The interpreter sets the program counter to nnn.
				*/
	case 0x1000:
		program_counter = nnn;
		break;

		/*
			   2NNN	Execute subroutine starting at address NNN
					   The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
				*/
	case 0x2000:
		if (_stack.size() == STACK_MAX_SIZE) {
			std::cerr << "Stack overflow!" << std::endl;
			cleanup_exit();
		}
		_stack.push(program_counter);
		program_counter = nnn;
		break;

		/*
			3XNN	Skip the following instruction if the value of register VX equals NN
			 */
	case 0x3000:
		if (v_x == nn) program_counter += 2;
		break;

		/*
			4XNN	Skip the following instruction if the value of register VX is not equal to NN
			*/
	case 0x4000:
		if (v_x != nn) program_counter += 2;
		break;

		/*
			5XY0	Skip the following instruction if the value of register VX is equal to the value of register VY
			 */
	case 0x5000:
		if (v_x == v_y) program_counter += 2;
		break;
		/*
		   6XNN	Store number NN in register VX
			*/
	case 0x6000:
		registers[x] = nn;
		break;
		/*
			7XNN	Add the value NN to register VX
			 */
	case 0x7000:
		registers[x] += nn;
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
					Set VF to 00 if a borrow occurs
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
			uint16_t temp;
		case 0:
			registers[x] = v_y;
			break;
		case 1:
			registers[x] = v_x | v_y;
			break;
		case 2:
			registers[x] = v_x & v_y;
			break;
		case 3:
			registers[x] = v_x ^ v_y;
			break;
		case 4:
			temp = v_x + v_y;
			if (temp < 255) {
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}
			registers[x] = temp & 0xFF;
			break;
		case 5:
			temp = v_x - v_y;
			if (v_x < v_y) {
				registers[0xF] = 0;
			}
			else {
				registers[0xF] = 1;
			}
			registers[x] = temp & 0xFF;
			break;
		case 6:
			registers[0xF] = v_y & 0x1;
			registers[x] = v_y >> 0x1;
			break;
		case 7:
			temp = v_y - v_x;
			if (v_y < v_x) {
				registers[0xF] = 0;
			}
			else {
				registers[0xF] = 1;
			}
			registers[x] = temp & 0xFF;
			break;
		case 0xE:
			registers[0xF] = v_y & 0x80;
			registers[x] = v_y << 0x1;
			break;

		}
		break;
		/*
			   9XY0	Skip the following instruction if the value of register VX is not equal to the value of register VY
			*/
	case 0x9000:
		if (v_x != v_y)program_counter += 2;
		break;
		/*
			ANNN	Store memory address NNN in register I
			 */
	case 0xA000:
		I = nnn;
		break;
		/*
		   BNNN	Jump to address NNN + V0
			*/
	case 0xB000:
		program_counter = nnn + registers[0];
		break;
		/*
				CXNN	Set VX to a random number with a mask of NN
				 */
	case 0xC000:
		registers[x] = (rand() & 0xFF) & nn;
		break;
		/*
			DXYN	Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
					Set VF to 01 if any set pixels are changed to unset, and 00 otherwise

					If the sprite is to be visible on the screen,
					the VX register must contain a value between 00 and 3F, (0-63)
					and the VY register must contain a value between 00 and 1F. (0-31)
			 */
	case 0xD000:

		for (int16_t i = 0; i < n; i++) {
			unset = draw_sprite(memory[I + i], v_x, v_y + i);
			if (unset)registers[0xF] = 0x01;
			else registers[0xF] = 0x00;
		}
		draw = true;

		break;
		/*
		   EX9E	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
		   EXA1	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
			*/
	case 0xE000:
		switch (opcode & 0x000F) {

		case 0xE:
			if (v_x == last_pressed) {
				program_counter += 2;
			}
			break;
		case 0x1:
			if (v_x != last_pressed) {
				program_counter += 2;
			}
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
			registers[x] = delay_timer;
			break;
		case 0x0A:
			wait_press = true;
			while (wait_press) {
				handle_events();
				sleep_for_ms(100); //TODO maybe another amount
			}
			registers[x] = last_pressed;
			break;
		case 0x15:
			delay_timer = v_x;
			break;
		case 0x18:
			sound_timer = v_x;
			break;
		case 0x1E:
			I += v_x;
			break;
		case 0x29:
			I = v_x * 5;
			break;
		case 0x33:
			memory[I] = v_x / 100;
			memory[I + 1] = (v_x / 10) % 10;
			memory[I + 2] = v_x % 10;
			break;
			/*
			FX55	Store the values of registers V0 to VX inclusive in memory starting at address I
					I is set to I + X + 1 after operation
					*/
		case 0x55:
			for (int i = 0; i <= x; i++) {
				memory[I + i] = registers[i];
			}
			I = I + x + 1;
			break;
			/*
				   FX65	Fill registers V0 to VX inclusive with the values stored in memory starting at address I
				   I is set to I + X + 1 after operation
				   */
		case 0x65:
			for (int i = 0; i <= x; i++) {
				registers[i] = memory[I + i];
			}
			I = I + x + 1;
			break;

		}
		break;

	}
}

void CPU::set_variables()
{
	opcode = (memory[program_counter] << 8) | memory[program_counter + 1];
	nnn = opcode & 0x0FFF;
	nn = opcode & 0x00FF;
	n = opcode & 0x000F;
	x = (opcode & 0x0F00) >> 8;
	y = (opcode & 0x00F0) >> 4;
	v_x = registers[x];
	v_y = registers[y];
}

void CPU::draw_logo()
{
	SDL_UpdateWindowSurface(gWindow);
}

void CPU::erase_screen()
{
	for (uint8_t i = 0; i < WIDTH; ++i) {
		for (uint8_t j = 0; j < HEIGHT; ++j) {
			display[i][j] = false;
		}
	}
}

void CPU::cleanup_exit() {

	std::cout << "Shutting down sdl2.." << std::endl;

	SDL_FreeSurface(gScreenSurface);
	SDL_FreeSurface(gLogo);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);

	gScreenSurface = NULL;
	gLogo = NULL;
	gRenderer = NULL;
	gWindow = NULL;

	SDL_Quit();

#ifdef WITH_CURSES
	std::cout << "Shutting down curses..." << std::endl;
	endwin();
#endif
}
