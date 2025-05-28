#include "timer.h"

void Timer::init(unsigned int _tick_limit)
{
	tick_limit = _tick_limit;
	ticking = false;
}

void Timer::start()
{
	ticking = true;
}

void Timer::update()
{
	if(ticking)
		ticks ++;
}

bool Timer::done()
{
	if(ticks >= tick_limit)
	{
		ticks = 0;
		return true;
	}
	return false;
}

void Timer::stop()
{
	ticking = false;
}

void Timer::reset()
{
	ticks = 0;
}

Delay fps_timer;
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
	{
		SDL_Delay(ms_per_frame - (t2-t1));
		deltaTime = (float)ms_per_frame;
	}
	else
		deltaTime = (float)(t2-t1);
}