#pragma once

#include <map>
#include <string>
#include "item.h"
#include <vector>

#define STRUCTURE_DATA_PATH "data/data/structure.dat"

enum e_structure_state
{
	BUILDING, WORKING, RUBBLE
};

class Structure
{
public:
	unsigned int client_id;
	unsigned int id; // structure id
	int x, y; // position
	int cx, cy; // chunk position
	unsigned int orientation; // 0 - right, 1 - down, 2 - left, 3 - up
	unsigned int timer;
	e_structure_state structure_state;
	int health=100;
	
	void init(unsigned int _client_id, unsigned int _id, int _x, int _y, unsigned int _orient);
	
	void (*update_func)(void*); // update function pointer
	void (*draw_func)(void*); // draw function pointer
	void (*init_func)(void*); // custom initialization
	
	void update();
	void draw(); // draws the structure relative to the camera
	void setf(void (*_update_func)(void*), void (*_draw_func)(void*), void (*_init_func)(void*)); // sets update draw and init functions and initialize
};

// structure "template"
class DefStructure
{
public:
	std::string name; // name of the structure
	unsigned int id; // structure id
	unsigned int size; // structure size(default 1)
	void init_default(); // inits by default settings
};

class StructFurnace
{
public:
	std::vector<Item*> contents;
	std::vector<Item*> fuel;
	
	Structure* parent;
	
	int burn_timer = 0;
	int cool_timer = 0;
	
	int temperature;
	int max_temperature;
	int desired_temperature;
	~StructFurnace();
};

// holds information about structures from structure.dat
// also holds additional information about structures
class StructureManager
{
public:
	void init(); // initializes the structure manager
	~StructureManager();
	
	DefStructure* get(int id);
	std::map<int, DefStructure*> obj; // all structures from file
	
	std::map<Structure*, StructFurnace*> furnaces;
};
extern StructureManager struct_man;

// draws the structure based on the orientation, d_char and d_col
void draw_structure(const unsigned char* d_char, const unsigned char* d_col, int x, int y, unsigned int _orient);

void struct_default_init(void* def_arg);

void update_drill(void* _drill);
void draw_drill(void* _drill);

void update_road(void* _road);
void draw_road(void* _road);

void draw_incubator(void* _incub);
void update_incubator(void* _incub);

void init_furnace(void* _furn);
void draw_furnace(void* _furn);
void update_furnace(void* _furn);