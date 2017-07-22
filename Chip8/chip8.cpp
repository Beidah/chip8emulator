#include "chip8.h"
#include <fstream>

unsigned char chip8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::initialize()
{
	// Initialize registers and memory once
	pc		= 0x200; // Program counter starts at 0x200
	opcode	= 0;
	I		= 0;
	sp		= 0;

	// Clear display
	for (int i = 0; i < 64 * 32; ++i)
		gfx[i] = 0;
	// Clear stack
	for (int i = 0; i < 16; ++i)
		stack[i] = 0;
	// Clear registers V0-VF
	for (int i = 0; i < 0xF; ++i)
		V[i] = 0;
	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;

	// Load fontset
	for (auto i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];
	

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;
}


void Chip8::emulateCycle()
{
	// Fetch Opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	// Decode Opcode
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x00E0: clears the screen
			for (auto i = 0; i < 64 * 32; ++i)
				gfx[i] = 0;
			pc += 2;
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			// TODO
			break;

		default:
			printf("Unkown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;

	case 0x1000: // 0x1NNN: Jump to address NNN
		pc = opcode & 0x0FFF;
		break;

	case 0x2000: // 0x2NNN: Calls subroutine at address NNN
		stack[sp++] = pc;
		pc = opcode & 0x0FFF;
		break;

	case 0x3000: // 0x3XNN: skips next instruction if VX = NN
		if (V[(opcode & 0x0F00) >> 8] == opcode & 0x00FF)
			pc += 4;
		else
			pc += 2;
		break;

	case 0x4000: // 0x4XNN Skips the next instuction if VX != NN
		if (V[(opcode & 0x0F00) >> 8] != opcode & 0x00FF)
			pc += 4;
		else
			pc += 2;
		break;

	case 0x5000: // 0x5XY0: Skips the next instruction if VX = VY
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else
			pc += 2;
		break;

	case 0x6000: // 0x6XNN Sets VX to NN
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7000: // 0x7XNN: Adds NN to VX
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x8XY0 Sets VX to the value of VY
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0001: // 0x8XY1: Sets VX to VX | VY
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0002: // 0x8XY2: Sets VX to VX & VY
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0003: // 0x8XY3: Sets VX to VX ^ VY
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0004: // 0x8XY4: Adds values of register VY to VX. Register VF is set to 1 if there is a carry
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1; // carry
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0005: // 0x8XY5: Subtracts VY from VX. VF is set to 1 if there is a borrow, else 0
			if (V[(opcode & 0x00f0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 1;
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x0001;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] >> 1;
			pc += 2;
			break;

		case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is again the borrow.
			if (V[(opcode & 0x00f0) >> 4] < V[(opcode & 0x0F00) >> 8])
				V[0xF] = 1;
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x8000;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] << 1;
			pc += 2;
			break;

		default:
			printf("Unkown opcode [0x8000]: 0x%X\n", opcode);
		}
		break;

	case 0x9000: // 0x9XY0: Skips the next instruction if VX != VY
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else
			pc += 2;
		break;

	case 0xA000: // 0xANNN: Sets I to the adress NNN
		I = opcode & 0x0FFF;
		pc += 2;
		break;

	case 0xB000: // 0xBNNN Jumps to address NNN plus V0
		pc = (opcode & 0x0FFF) + V[0];
		break;

	case 0xC000: // 0xCXNN: Sets VX to the result of bitwise and op of a random (0-255) and NN
		// TODO: Random number setup
		pc += 2;
		break;

		/**
		 * 0xDXYN:
		 * Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
		 * Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t 
		 * change after the execution of this instruction. As described above, VF is set to 1 if any 
		 * screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t 
		 * happen
		 */
	case 0xD000:
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; x < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[x + xline + ((y + yline) * 64)] == 1)
						V[0xF] = 1;
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;
	}
	break;

	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E: // 0xEX9E:  skips the next instuction if the key stored in VX is pressed
			if (key[V[(opcode & 0x0F00) >> 8]])
				pc += 4;
			else
				pc += 2;
			break;

		case 0x00A1: // 0xEXA1: skips the next instuction if the key stored in VX is pressed
			if (!key[V[(opcode & 0x0F00) >> 8]])
				pc += 4;
			else
				pc += 2;
			break;

		default:
			printf("Unkown opcode [0xE000]: 0x%X\n", opcode);

		}

	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: // 0xFX07: Sets VX to value of delay timer
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;

		case 0x000A: // 0xFX0A A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
			// TODO
			break;

		case 0x0015: // 0xFX15 Sets value of delay timer to VX
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0018: // 0xFX18: Sets value of sound timer to VX
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x001E: // 0xFX1E: adds value of VX to I
			I += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0029: // 0xFX29: Sets I to the location of the sprite for the character in VX.
			// TODO
			break;

		case 0x0033: // 0xFX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
			pc += 2;
			break;

		case 0x0055: // 0xFX55: Stores V0 to VX (including VX) in memory starting at address I.
			for (int i = 0; i <= (opcode & 0x0F00) >> 8; ++i)
				memory[I + i] = V[i];
			pc += 2;
			break;

		case 0x0065: // 0xFX65: Fills V0 to VX (includeing VX) from memory starting at address I.
			for (int i = 0; i <= (opcode & 0x0F00) >> 8; ++i)
				V[i] = memory[I + i];
			pc += 2;
			break;
		}

	default:
		printf("Unkown opcode: 0x%X\n", opcode);
	}

	// Update Timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	}
}

void Chip8::loadGame(char* filename)
{

	auto file = std::ifstream(filename, std::ios::binary);
	if (file)
	{
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);
		auto *buffer = new char[length];

		file.read(buffer, 1);
		for (auto i = 0; i < length; ++i)
		{
			memory[512 + i] = buffer[i];
		}
	}

}

