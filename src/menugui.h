#pragma once

#include "renderer.h"
#include "random.h"

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

int wrap_number(int x, int a, int b); // wraps x around a and b (a<b)

class GUIWormSegment
{
public:
	int x, y;
};

class GUIWorm
{
public:
	void init(int _w, int _h);
	void draw();
	void update(int _w, int _h);
	std::vector<GUIWormSegment> segments;
	unsigned int movement_timer;
	unsigned int turn_timer;
	float turn_direction;
	
	float direction;
};

class GUIFly
{
public:
	int x, y;
	float direction;
	bool state; // true = wings up, flase = wings down
	void init(int _x, int _y);
	void draw();
	void update(int _w, int _h); // updates position;
	bool canDelete;
	int timer; // countdown till explosion
};

class GUIParticle
{
public:
	float x, y, vx, vy;
	void update(int _w, int _h); // updates position
	void init(float _x, float _y, float _vx, float _vy, char _color);
	bool canDelete;
	char color;
};

// cool effect for the menu
class GUIDirt
{
public:
	void init(int _w, int _h);
	void draw();
	void update(int _w, int _h);
	~GUIDirt(); // free all the allocated memory
	
	void makeSplatter(int x, int y, int num); // make particle boom
	void updateParticles(int _w, int _h);
	void updateFlies(int _w, int _h);
	void makeFly(int x, int y);
	int get_border_fade_intensity(int x, int y); // 0-100 how intense is the border

	unsigned char** dirt_symbol;
	unsigned char** dirt_color;
	
	int w, h;
	
	std::vector<GUIWorm> worm;
	std::vector<GUIFly*> flies;
	std::vector<GUIParticle*> particles;
};