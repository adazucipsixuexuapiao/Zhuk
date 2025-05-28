#pragma once

#include <vector>
#include <ctime>

#include "gamelogic.h"
#include <cmath>

// randomizer only for client side effects if there are any
extern std::mt19937 eff_r_engine;
#define grand_eff(a,b) std::uniform_int_distribution<int>(a,b)(eff_r_engine)

#define SMOKE_TIMER 60
#define FIREBALL_TIMER 4
#define SMOKETRAIL_TIMER 60

#define FIREBALL_DAM 17

#define EFFECT_SOUNDS true

class Smoke
{
public:
	unsigned int timer;
	int x, y;
	void update();
	void draw();
};

class SmokeTrail
{
public:
	int x, y;
	unsigned int timer;
};

class FireBall
{
public:
	unsigned int timer;
	float dir; // in radians
	int x, y;
	void update();
	void draw();
};

class EffectManager
{
public:
	void make_smoke(int x, int y, unsigned int timer);
	void make_fireball(int x, int y, unsigned int timer);
	void make_explosion(int x, int y,unsigned int num_smoke, unsigned int num_fireball, unsigned int smoke_timer, unsigned int fireball_timer);
	void make_smoke_puff(int x, int y, unsigned int num_smoke);
	void update();
	void draw();
	std::vector<Smoke*> smoke_eff;
	std::vector<SmokeTrail*> smoketrail_eff;
	std::vector<FireBall*> fireball_eff;
	~EffectManager();
};

extern EffectManager effect_man;