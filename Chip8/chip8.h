#pragma once

#include <string>

class Chip8
{
private:
	unsigned short opcode;
	unsigned char memory[4096]; // 4KB memory
	// 16 registers, V0-VF
	unsigned char V[16];
	// Index register and Program counter
	unsigned short I;
	unsigned short pc;
	// Graphics array
	unsigned char gfx[64 * 32];
	// Both timers, when set, count down to zero at 60HZ
	unsigned char delay_timer;
	// The system’s buzzer sounds whenever the sound timer reaches zero.
	unsigned char sound_timer;

	unsigned short stack[16];
	unsigned short sp; // Stack pointer

	// Chip 8 uses a hex based keypad
	unsigned char key[16];

public:

	void initialize();
	void emulateCycle();
	void loadGame(char* filename);
};