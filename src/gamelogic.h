#pragma once

#include <vector>
#include <string>

#include "renderer.h"
#include "mapgen.h"
#include "client.h"
#include "fileparser.h"
#include "audio.h"
#include "effects.h"

// debugging
#include "debug.h"

extern World world;

class Camera
{
public:
	t_coord x, y; // coordinate of the top left corner
	t_coord speed;
	int w, h;
	bool border_move; // allow camera to be moved by moving mouse to edge
	
	int cursor_wx, cursor_wy; // in world coordinates
	int cursor_cx, cursor_cy; // in chunk coordinates
	int cursor_x, cursor_y; // in screen coordinates
	int cursor_px, cursor_py; // previous (x,y) of cursor
	
	Camera();
	~Camera();
	void pos_move(t_coord dx, t_coord dy); // move the camera by a set amount
	void spd_move(t_coord dx, t_coord dy); // move the camera by a set amount * speed
	void set_pos(t_coord _x, t_coord _y);
	void update();
	
	bool in_view(t_coord _x, t_coord _y); // is the point in view of the camera?
	
	void check_hover();
	
	// what is mouse hovering over
	Tile* hover_tile;
	std::vector<Item*> hover_items;
	std::vector<Structure*> hover_structs;
	std::vector<Unit*> hover_units;
	
	bool dragging;
	bool alt_health_view=false; // alt + h
};

extern Camera camera;

struct func_pos
{
public:
	int x, y; bool active = false;
};

#define ROAD_TICKCOUNT 10
class GameLogic
{
public:
	void init();
	void draw();
	void update(std::vector<SDL_Event>& events);
	void update_input(SDL_Event evnt);
	~GameLogic();
	
	void tick(std::string server_command);
	
	// function key functionality
	void function_key(unsigned int _f);
	func_pos func_positions[12];
	func_pos func_last_position; // last camera position before function key
	
	unsigned int tickcount;
	unsigned int road_timer; // to ensure roads move evenly

	// input states
	bool shift_down, ctrl_down, alt_down;
	bool dragging_camera;
	
	// belonging to the player(for faster access)
	// should not be used for gamelogic updates to ensure synchronization
	std::vector<Structure*> p_structures;
	std::vector<Unit*> p_units;
	
	// belonging to anyone
	std::vector<Unit*> units;
	std::vector<Structure*> structures;
	std::vector<Item*> items;
};

extern GameLogic gamelogic;

void build_structure(unsigned int client_id, unsigned int entity_id, int x, int y, unsigned int orientation);
void spawn_item(unsigned int item_id, unsigned int mat_id, int x, int y);