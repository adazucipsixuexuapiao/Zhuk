#include "timer.h"



void Delay::init(const unsigned int _fps)
{
	fps = _fps;
	ms_per_frame = 1000/fps;
}

void Delay::start()
{
	t1 = SDL_GetTicks();
}

void Delay::stop()
{
	Uint32 t2 = SDL_GetTicks();
	if(t2 - t1 < ms_per_frame)
		SDL_Delay(ms_per_frame - (t2-t1));
}