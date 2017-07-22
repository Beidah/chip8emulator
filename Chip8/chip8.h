#pragma once

#include <string>

class Chip8
{
public:
	Chip8();
	~Chip8();

	void initialize();

	void emulateCycle();
	void loadProgram(char* filename);

	bool drawFlag;
private:
	unsigned short opcode;
	unsigned char memory[4096]; // 4KB memory
	// 16 registers, V0-VF
	unsigned char V[16];
	// Index register and Program counter
	unsigned short I;
	unsigned short pc;
	
	// Both timers, when set, count down to zero at 60HZ
	unsigned char delay_timer;
	// The system’s buzzer sounds whenever the sound timer reaches zero.
	unsigned char sound_timer;

	unsigned short stack[16];
	unsigned short sp; // Stack pointer

	

public:
	// Graphics array
	unsigned char gfx[64 * 32];
	// Chip 8 uses a hex based keypad
	unsigned char key[16];
};