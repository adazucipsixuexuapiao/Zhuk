#include "window.h"

void Window::init()
{
	unsigned int w, h;
	// set to config file settings
	if(!config.get_bool("windowed"))
	{
		w = config.get_int("fwidth");
		h = config.get_int("fheight");
	} else {
		w = config.get_int("wwidth");
		h = config.get_int("wheight");
	}
	
	// use screen resolution
	if(!config.get_bool("custom_fullscreen") && !config.get_bool("windowed"))
	{
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		w = DM.w;
		h = DM.h;
	}
	
	W = w; H = h;
	
	win = SDL_CreateWindow("Zhuk", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, w, h, ((!config.get_bool("windowed"))?SDL_WINDOW_FULLSCREEN:0));
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
}

Window::~Window()
{
	if(win != nullptr) SDL_DestroyWindow(win);
	if(rend != nullptr) SDL_DestroyRenderer(rend);
}