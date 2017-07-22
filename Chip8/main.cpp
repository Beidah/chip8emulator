#include "chip8.h"

Chip8 myChip;

void main()
{
	myChip.initialize();
	while (true)
	{
		myChip.emulateCycle();
	}
}