#pragma once

#include <SDL2/SDL.h>

// Timer which counts the number of ticks elapsed
class Timer
{
public:
	void init(unsigned int _tick_limit);
	void start(); // begin ticking
	bool done(); // returns true if reached tick_limit and resets the timer.
	void update(); // ticks ++
	void stop();
	void reset();
	
	unsigned int ticks; // elapsed ticks
	unsigned int tick_limit; // desired tick count
	bool ticking; // is it currently ticking?
};

// Timer which delays the program to keep the fps stable
class Delay
{
public:
	void init(const unsigned int _fps);

	void start();
	void stop();
	unsigned int fps;
	unsigned int ms_per_frame; // 1000/fps
	Uint32 t1;
	
	float deltaTime; // ms in previous frame
};

extern Delay fps_timer;