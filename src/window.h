#pragma once

#include <SDL2/SDL.h>
#include "config.h"

class Window
{
public:
	void init();
	~Window();

	SDL_Window* win;
	SDL_Renderer* rend;
	
	unsigned int W, H;
};