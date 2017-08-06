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

	myChip8.loadProgram("./pong.rom");

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
		}

		myChip8.emulateCycle();
		if (myChip8.drawFlag)
		{
			drawPixels(renderer);
			myChip8.drawFlag = false;
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);
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