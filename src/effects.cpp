#include "effects.h"

std::mt19937 eff_r_engine(std::time(0));
EffectManager effect_man;

void EffectManager::update()
{
	for(int i=0;i<smoketrail_eff.size();i++)
	{
		smoketrail_eff[i]->timer --;
		if(smoketrail_eff[i]->timer <= 0)
		{
			delete smoketrail_eff[i];
			smoketrail_eff.erase(smoketrail_eff.begin()+i);
		}
	}
	
	for(int i=0;i<smoke_eff.size();i++)
	{
		SmokeTrail* smtrail = new SmokeTrail;
		smtrail->timer = SMOKETRAIL_TIMER;
		smtrail->x = smoke_eff[i]->x;
		smtrail->y = smoke_eff[i]->y;
		smoke_eff[i]->update();
		smoketrail_eff.push_back(smtrail);
		if(smoke_eff[i]->timer <= 0)
		{
			delete smoke_eff[i];
			smoke_eff.erase(smoke_eff.begin()+i);
		}
	}
	
	for(int i=0;i<fireball_eff.size();i++)
	{
		fireball_eff[i]->update();
		if(fireball_eff[i]->timer <= 0)
		{
			delete fireball_eff[i];
			fireball_eff.erase(fireball_eff.begin()+i);
		}
	}
}

void EffectManager::make_smoke(int x,int y, unsigned int timer)
{
	Smoke* smoke = new Smoke;
	smoke->x = x;
	smoke->y = y;
	smoke->timer = timer + genrand(0,timer/2);
	smoke_eff.push_back(smoke);
}

void EffectManager::make_fireball(int x, int y, unsigned int timer)
{
	FireBall* fireball = new FireBall;
	fireball->x = x;
	fireball->y = y;
	fireball->dir = (float)genrand(0,628) / 100.0f;
	fireball->timer = timer + genrand(0,timer/2);
	fireball_eff.push_back(fireball);
}

void EffectManager::make_smoke_puff(int x, int y, unsigned int num_smoke)
{
	if(EFFECT_SOUNDS)
		if(camera.in_view(x,y))
			audio.play("furnace.wav");
	for(int i=0;i<num_smoke;i++)
		make_smoke(x, y, 10);
}

void EffectManager::make_explosion(int x, int y,unsigned int num_smoke, unsigned int num_fireball, unsigned int smoke_timer, unsigned int fireball_timer)
{
	if(EFFECT_SOUNDS)
		if(camera.in_view(x, y))
			audio.play("explode.wav");
	
	for(int i=0;i<num_smoke;i++)
		make_smoke(x,y,smoke_timer);
	for(int i=0;i<num_fireball;i++)
		make_fireball(x,y,fireball_timer);
}

void EffectManager::draw()
{
	for(int i=0;i<smoketrail_eff.size();i++)
	{
		unsigned char sym = 0xb2;
		unsigned int timer = smoketrail_eff[i]->timer;
		if(timer < 40) sym = 0xb1;
		if(timer < 20) sym = 0xb0;
		rend.set(smoketrail_eff[i]->x - camera.x,
		smoketrail_eff[i]->y - camera.y, sym, 0x78);
	}
	for(int i=0;i<smoke_eff.size();i++)
	{
		rend.set(smoke_eff[i]->x - camera.x,
		smoke_eff[i]->y - camera.y, 0xb2, 0x78);
	}
	for(int i=0;i<fireball_eff.size();i++)
	{
		rend.set(fireball_eff[i]->x - camera.x,
		fireball_eff[i]->y - camera.y, 0xb0, 0xb9);
	}
}

EffectManager::~EffectManager()
{
	for(int i=0;i<smoke_eff.size();i++)
		if(smoke_eff[i] != nullptr) delete smoke_eff[i];
	for(int i=0;i<fireball_eff.size();i++)
		if(fireball_eff[i] != nullptr) delete fireball_eff[i];
}

void Smoke::update()
{
	int dir = genrand(0,3);
	if(dir == 0)
		x ++;
	if(dir == 1)
		y --;
	if(dir == 2)
		x --;
	if(dir == 3)
		y ++;
	timer --;
}

void FireBall::update()
{
	Tile* curr_tile = world.get_tile(x,y);
	
	if(curr_tile != nullptr)
		curr_tile->tile_id = 7; // char
	
	for(int i=0;i<2;i++) // fireball damage
	{
		for(int j=0;j<2;j++)
		{
			Tile* tile = world.get_tile(x-j,y-i);
			if(tile == nullptr) continue;
			for(int i=0;i<tile->structures.size();i++)
			{
				if(struct_man.get(tile->structures[i]->id)->size == 1 && (i != 0 || j != 0)) continue;
				tile->structures[i]->health -= FIREBALL_DAM;
			}
		}
	}
	
	// reused worm code
	float gx = std::cos(dir);
	float gy = std::sin(dir);
	
	float prob_x = 100.0f*(std::fabs(gx)/(std::fabs(gx)+std::fabs(gy)));
	int random = genrand(0,100);
	if(random <= prob_x) // go in x dir
	{
		int dx = gx / std::fabs(gx);
		x += dx;
	} else { // go in y dir
		int dy = gy / std::fabs(gy);
		y += dy;
	}
	
	timer --;
}