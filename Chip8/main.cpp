#include "chip8.h"
#include <SDL.h>
#include <iostream>

// Display size
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

Chip8 myChip8;
int modifier = 10;

// Window size
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

void drawPixels(SDL_Renderer* renderer);
void keydown(const SDL_KeyboardEvent& key);
void keyup(const SDL_KeyboardEvent& key);

#undef main


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: myChip8.exe chip8application\n\n");
		return 1;
	}

	// Load game
	if (!myChip8.loadProgram(argv[1]))
		return 1;

	// myChip8.loadProgram("./pong.rom");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL ERROR: " << SDL_GetError() << std::endl;
		return 2;
	}

	auto window = SDL_CreateWindow("Chip 8", 100, 100, display_width, display_height, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "SDL ERROR: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 2;
	}

	auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		SDL_DestroyWindow(window);
		std::cout << "SDL ERROR: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 2;
	}

	while (true)
	{
		SDL_Event e;
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) break;

			if (e.type == SDL_KEYDOWN)
			{
				auto key = e.key;
				keydown(key);
			}
			else if (e.type == SDL_KEYUP)
			{
				auto key = e.key;
				keyup(key);
			}
		}

		myChip8.emulateCycle();
		if (myChip8.drawFlag)
		{
			drawPixels(renderer);
			myChip8.drawFlag = false;
		}

		SDL_RenderPresent(renderer);
		// SDL_Delay(1000 / 60);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void drawPixels(SDL_Renderer* renderer)
{

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (myChip8.gfx[(y * 64) + x] == 0)
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			else 
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

			SDL_Rect rect;
			rect.x = (x * modifier);
			rect.y = (y * modifier);
			rect.w = modifier;
			rect.h = modifier;
			SDL_RenderFillRect(renderer, &rect);
		}
	}
}

void keydown(const SDL_KeyboardEvent& key)
{
	switch (key.keysym.scancode)
	{
	case SDL_SCANCODE_ESCAPE:
		exit(0);

	case SDL_SCANCODE_1:
		myChip8.key[0x1] = 1;
		break;
	case SDL_SCANCODE_2:
		myChip8.key[0x2] = 1;
		break;
	case SDL_SCANCODE_3:
		myChip8.key[0x3] = 1;
		break;
	case SDL_SCANCODE_4:
		myChip8.key[0xC] = 1;
		break;
	case SDL_SCANCODE_Q:
		myChip8.key[0x4] = 1;
		break;
	case SDL_SCANCODE_W:
		myChip8.key[0x5] = 1;
		break;
	case SDL_SCANCODE_E:
		myChip8.key[0x6] = 1;
		break;
	case SDL_SCANCODE_R:
		myChip8.key[0xD] = 1;
		break;
	case SDL_SCANCODE_A:
		myChip8.key[0x7] = 1;
		break;
	case SDL_SCANCODE_S:
		myChip8.key[0x8] = 1;
		break;
	case SDL_SCANCODE_D:
		myChip8.key[0x9] = 1;
		break;
	case SDL_SCANCODE_F:
		myChip8.key[0xE] = 1;
		break;
	case SDL_SCANCODE_Z:
		myChip8.key[0xA] = 1;
		break;
	case SDL_SCANCODE_X:
		myChip8.key[0x0] = 1;
		break;
	case SDL_SCANCODE_C:
		myChip8.key[0xB] = 1;
		break;
	case SDL_SCANCODE_V:
		myChip8.key[0xF] = 1;
		break;

	default:
		break;
	}
}

void keyup(const SDL_KeyboardEvent& key)
{
	switch (key.keysym.scancode)
	{

	case SDL_SCANCODE_1:
		myChip8.key[0x1] = 0;
		break;
	case SDL_SCANCODE_2:
		myChip8.key[0x2] = 0;
		break;
	case SDL_SCANCODE_3:
		myChip8.key[0x3] = 0;
		break;
	case SDL_SCANCODE_4:
		myChip8.key[0xC] = 0;
		break;
	case SDL_SCANCODE_Q:
		myChip8.key[0x4] = 0;
		break;
	case SDL_SCANCODE_W:
		myChip8.key[0x5] = 0;
		break;
	case SDL_SCANCODE_E:
		myChip8.key[0x6] = 0;
		break;
	case SDL_SCANCODE_R:
		myChip8.key[0xD] = 0;
		break;
	case SDL_SCANCODE_A:
		myChip8.key[0x7] = 0;
		break;
	case SDL_SCANCODE_S:
		myChip8.key[0x8] = 0;
		break;
	case SDL_SCANCODE_D:
		myChip8.key[0x9] = 0;
		break;
	case SDL_SCANCODE_F:
		myChip8.key[0xE] = 0;
		break;
	case SDL_SCANCODE_Z:
		myChip8.key[0xA] = 0;
		break;
	case SDL_SCANCODE_X:
		myChip8.key[0x0] = 0;
		break;
	case SDL_SCANCODE_C:
		myChip8.key[0xB] = 0;
		break;
	case SDL_SCANCODE_V:
		myChip8.key[0xF] = 0;
		break;

	default: break;
	}
}