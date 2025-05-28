#include "menugui.h"

#define GUI_NUM_WORMS 2
#define PI 3.1415926f

int wrap_number(int x, int a, int b)
{
	int d = x - a;
	if(d > (b-a))
	{
		d %= (b-a+1);
		x = a + d;
	}
	else if(d < 0)
	{
		d %= (b-a+1);
		x = b + d;
	}
	return x;
}

void GUIDirt::init(int _w, int _h)
{
	w = _w; h = _h;
	
	dirt_symbol = new unsigned char*[h];
	dirt_color = new unsigned char*[h];
	
	// generate dirt pattern
	for(int i=0;i<h;i++)
	{
		dirt_symbol[i] = new unsigned char[w];
		dirt_color[i] = new unsigned char[w];
		for(int j=0;j<w;j++)
		{
			int fade = genrand(1,100); // used for fading to stone/darkness
			if(get_border_fade_intensity(j,i) < fade) // dirt
			{
				int sym_choice = genrand(0,1);
				dirt_symbol[i][j] = (sym_choice==1?0xb2:0xb1);
				int color_choice = genrand(0,10);
				dirt_color[i][j] = (color_choice==0?0x70:0x30);
			} else { // stone/darkness on the border
				dirt_symbol[i][j] = 0; 
				dirt_color[i][j] = 0;
			}
		}
	}
	
	for(int i=0;i<GUI_NUM_WORMS;i++)
	{
		GUIWorm _worm;
		_worm.init(_w, _h);
		worm.push_back(_worm);
	}
}

void GUIDirt::update(int _w, int _h)
{
	for(int i=0;i<worm.size();i++)
		worm[i].update(_w, _h);
	// make worm carve out a path, kinda inefficient that it loops through each worm segment each time but it doesnt matter
	for(int j=0;j<worm.size();j++)
		for(int i=0;i<worm[j].segments.size();i++)
		{
			GUIWormSegment seg = worm[j].segments[i];
			if(dirt_symbol[seg.y][seg.x] != 0) dirt_symbol[seg.y][seg.x] = 0xb0; // only if not at the border
		}
	updateParticles(_w,_h);
	updateFlies(_w, _h);
}

void GUIDirt::draw()
{
	// draw the dirt field
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			rend.set(j,i,dirt_symbol[i][j],dirt_color[i][j]);
		}
	}
	// draw the worms
	for(int i=0;i<worm.size();i++)
		worm[i].draw();
	
	// draw the particles
	for(int i=0;i<particles.size();i++)
		rend.set((int)particles[i]->x, (int)particles[i]->y, 0xdb, particles[i]->color); // green blob
	
	// draw the flies
	for(int i=0;i<flies.size();i++)
		flies[i]->draw();
	
	// draw the border
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if(dirt_symbol[i][j]==0) rend.set(j,i,0,0);
		}
	}
}

GUIDirt::~GUIDirt()
{
	// free dirt background memory
	for(int i=0;i<h;i++)
	{	
		if(dirt_symbol[i] != nullptr) delete[] dirt_symbol[i];
		if(dirt_color[i] != nullptr) delete[] dirt_color[i];
	}
	delete[] dirt_symbol;
	delete[] dirt_color;
	
	// free flies if there are any
	for(int i=0;i<flies.size();i++)
		if(flies[i] != nullptr) delete flies[i];
	flies.clear();
	
	// free particles if any
	for(int i=0;i<particles.size();i++)
		if(particles[i] != nullptr) delete particles[i];
	particles.clear();
}

int GUIDirt::get_border_fade_intensity(int x, int y)
{
	const float ellipse_R = ((w/2)*0.6f); // major
	const float ellipse_r = ((h/2)*0.6f); // minor radius
	
	int intensity=0;
	
	float D = (x-w/2)*(x-w/2)/(ellipse_R*ellipse_R)+(y-h/2)*(y-h/2)/(ellipse_r*ellipse_r);
	
	if(D > 1)
	{
		intensity = (D-1)*100; //(int)((1-D)*100);
	}

	return intensity;
}

#define INITIAL_VELOCITY 0.5f
void GUIDirt::makeSplatter(int x, int y, int num)
{
	char __colors[4] = {0x10, 0x20, 0x40, 0x50};
	char _col = __colors[genrand(0,3)];
	for(int i=0;i<num;i++) // make a bunch of particles
	{
		float dir = genrand(0, 2*PI*1000)/1000.0f; // pick random direction
		float _vx = INITIAL_VELOCITY * std::cos(dir);
		float _vy = INITIAL_VELOCITY * std::sin(dir);
		GUIParticle* _part = new GUIParticle; // add particles to vector
		_part->init((float)x, (float)y, _vx, _vy, _col);
		particles.push_back(_part);
	}
}
#undef INITIAL_VELOCITY

void GUIDirt::makeFly(int x, int y)
{
	GUIFly* _fly = new GUIFly;
	_fly->init(x, y);
	flies.push_back(_fly);
}

void GUIDirt::updateParticles(int _w, int _h)
{
	for(int i=0;i<particles.size();i++)
	{
		particles[i]->update(_w, _h); // move particle
		if(particles[i]->canDelete) {delete particles[i]; particles.erase(particles.begin()+i);} // delete out of bounds particles
		else {
			int px = (int)particles[i]->x;
			int py = (int)particles[i]->y;
			if(px < _w && px >= 0 && py < _h && py >= 0)
				dirt_color[(int)particles[i]->y][(int)particles[i]->x] = particles[i]->color; // recolor particle trail
		}
	}
	particles.shrink_to_fit();
}

void GUIDirt::updateFlies(int _w, int _h)
{
	for(int i=0;i<flies.size();i++)
	{
		flies[i]->update(_w, _h);
		
		// fly exploded
		if(flies[i]->canDelete) {
			makeSplatter(flies[i]->x, flies[i]->y, 20);
			delete flies[i]; flies.erase(flies.begin()+i);}
	}
	flies.shrink_to_fit();
}

/*
 GUIFLY
*/

#define GUIFLY_TIMER 500
void GUIFly::init(int _x, int _y)
{
	x = _x;
	y = _y;
	state = false;
	canDelete = false;
	timer = GUIFLY_TIMER;
}
#undef GUIFLY_TIMER

void GUIFly::draw()
{
	rend.set(x,y,0x40, 0x20); // body
	// wings
	if(state) {rend.set(x-1, y,0xd3,0xf0);rend.set(x+1, y,0xbd,0xf0);} // up
	else      {rend.set(x-1, y,0xd6,0xf0);rend.set(x+1, y,0xb7,0xf0);} // down
}

void GUIFly::update(int _w, int _h)
{
	state = !state; // flap wings
	x += 1 - genrand(0,1)*2; // move around
	y += 1 - genrand(0,1)*2;

	timer --;
	if(timer <= 0) canDelete = true;

	// check if out of bounds
	x = wrap_number(x, 0, _w-1);
	y = wrap_number(y, 0, _h-1);
}

/*
 GUIPARTICLE
*/

void GUIParticle::init(float _x, float _y, float _vx, float _vy, char _color)
{
	x = _x; y = _y; vx = _vx; vy = _vy; color = _color;
	canDelete = false;
}

#define G_ACCELERATION 0.01f
void GUIParticle::update(int _w, int _h)
{
	x += vx;
	y += vy;
	vy += G_ACCELERATION;
	if(y>=_h) canDelete = true;
}
#undef G_ACCELERATION

/*
 GUIWORM
*/

// 16
#define GUIWORM_LENGTH 16
#define GUIWORM_MOVEMENT_TIMER 3
#define GUIWORM_TURNSPEED 0.1f
#define GUIWORM_MIN_TURN_TIMER 5
#define GUIWORM_MAX_TURN_TIMER 50
void GUIWorm::init(int _w, int _h)
{
	movement_timer = GUIWORM_MOVEMENT_TIMER;
	turn_timer = 0;
	turn_direction = 0.0f;
	direction = (float)genrand(0,2*3141)/1000.0f;
	// generate a multi-segment worm
	for(int i=0;i<GUIWORM_LENGTH;i++)
	{
		int x, y;
		// make all the segments
		if(i==0) {x = genrand(0,_w-1); y = genrand(0,_h-1);} // first segment
		else
		{ // other segments
			x = segments[i-1].x; y = segments[i-1].y; // get previous segment in order to attach to it
			GUIWormSegment pseudo_valid_directions[4] = {{x-1,y}, {x+1,y}, {x,y-1}, {x,y+1}};
			std::vector<GUIWormSegment> valid_directions;
			for(int k=0;k<4;k++) // loop through all pseudo-valid directions
			{
				GUIWormSegment s = pseudo_valid_directions[k];
				// affirm that they are valid
				if(s.x >= 0 && s.x < _w && s.y >= 0 && s.y < _h)
				{
					bool nonOverlapping = true;
					// check if its not on top of any previous segment
					for(int j=0;j<segments.size();j++)
						if(segments[j].x == s.x && segments[j].y == s.y) nonOverlapping = false;
					if(nonOverlapping) valid_directions.push_back(s);
				}
			}
			if(valid_directions.size() == 0) // if worm has suffocated its tail
			{
				// validate all directions which arent out of bounds
				for(int k=0;k<4;k++)
				{
					GUIWormSegment s = pseudo_valid_directions[k];
					if(s.x >= 0 && s.x < _w && s.y >= 0 && s.y < _h)
						valid_directions.push_back(s);
				}
			}
			
			// pick from one of the valid directions
			int index = genrand(0,valid_directions.size()-1);
			x = valid_directions[index].x; y = valid_directions[index].y;
		}
		GUIWormSegment seg; // initialize one segment
		seg.x = x; seg.y = y;
		segments.push_back(seg); // attach it to the worm
	}
}

void GUIWorm::draw()
{
	for(int i=0;i<segments.size();i++)
	{
		GUIWormSegment seg = segments[i];
		rend.set(seg.x, seg.y, 0x00, (i%2==0?0x09:0x01));
	}
}

void GUIWorm::update(int _w, int _h)
{
	if(movement_timer == 0)
	{
		movement_timer = GUIWORM_MOVEMENT_TIMER;
		GUIWormSegment newseg = segments[segments.size()-1];
		float x = std::cos(direction);
		float y = std::sin(direction);
		
		float prob_x = 100.0f*(std::fabs(x)/(std::fabs(x)+std::fabs(y)));
		int random = genrand(0,100);
		if(random <= prob_x) // go in x dir
		{
			int dx = x / std::fabs(x);
			newseg.x += dx;
		} else { // go in y dir
			int dy = y / std::fabs(y);
			newseg.y += dy;
		}
		
		// loop around if out of bounds
		if(newseg.x < 0) newseg.x = _w-1;
		if(newseg.x >= _w) newseg.x = 0;
		if(newseg.y < 0) newseg.y = _h-1;
		if(newseg.y >= _h) newseg.y = 0;
		
		segments.push_back(newseg);
		segments.erase(segments.begin());
		segments.shrink_to_fit();
		
		// turn the worm
		if(turn_timer == 0)
		{
			turn_timer = genrand(GUIWORM_MIN_TURN_TIMER, GUIWORM_MAX_TURN_TIMER);
			turn_direction = 1.0f - 2.0f*genrand(0,1);
		} else {
			turn_timer --;
			direction += turn_direction*GUIWORM_TURNSPEED;
		}
		
		// keep direction in -2pi 2pi interval(could make it smaller but it doesnt matter)
		if(direction > 2*PI) direction -= 2*PI;
		if(direction < -2*PI) direction += 2*PI;
	} else {
		movement_timer --;
	}
}