#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL.h>

class Timer
{
public:
	void update(); // updates the timer

	void set_frames();
	void set_seconds();
	bool done(); // false if not done, true if done
	
	unsigned int frames;
	unsigned int seconds;
	bool donevar;
	bool counting_frames;
private:
	
};

class Delay
{
public:
	void init(const unsigned int _fps);

	void start();
	void stop();
	unsigned int fps;
	unsigned int ms_per_frame; // 1000/fps
	Uint32 t1;
};

#endif