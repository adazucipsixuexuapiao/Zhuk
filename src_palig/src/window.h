#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include "config.h"

class Window
{
public:
	void init(Configuration& config);
	~Window();

	SDL_Window* win;
	SDL_Renderer* rend;
	
	unsigned int W, H;
};

#endif