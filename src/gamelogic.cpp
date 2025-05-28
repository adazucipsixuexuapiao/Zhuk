#include "gamelogic.h"

GameLogic gamelogic;
World world;
Camera camera;

/* CAMERA class */

void Camera::pos_move(t_coord dx, t_coord dy)
{
	x += dx;
	y += dy;
	// do bounds checks later
}

void Camera::spd_move(t_coord dx, t_coord dy)
{
	x += speed*dx;
	y += speed*dy;
	// do bounds checks later
}

bool Camera::in_view(int _x, int _y)
{
	if(_x >= x && _x < x+rend.w && _y >= y && _y < y+rend.h)
		return true;
	else return false;
}

void Camera::set_pos(t_coord _x, t_coord _y)
{x = _x; y = _y;}

Camera::Camera()
{
	dragging = false; border_move = false;
	x = 0; y = 0;
	cursor_x = 0; cursor_y = 0; cursor_wx = 0; cursor_wy = 0; cursor_px = 0; cursor_py = 0;
}

bool in_box(int x, int y, int x1, int y1, int x2, int y2)
{
	if(x >= x1 && x <= x2 && y >= y1 && y <= y2)
		return true;
	return false;
}

void Camera::update()
{
	// mouse position
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	cursor_px = cursor_x;
	cursor_py = cursor_y;
	cursor_x = (int)(mx / rend.tile_w);
	cursor_y = (int)(my / rend.tile_h);
	cursor_wx = x + cursor_x;
	cursor_wy = y + cursor_y;
	cursor_cx = std::floor((float)cursor_wx / (float)CHUNK_SIZE);
	cursor_cy = std::floor((float)cursor_wy / (float)CHUNK_SIZE);
	
	if(dragging) // drag camera with right mouse button
	{
		pos_move(cursor_px - cursor_x, cursor_py - cursor_y);
	} else { // if cursor is on edge and not dragging, then move camera
		if(border_move == false) goto border_skip;
		int c_dx = 0; int c_dy = 0;
		if(cursor_x <= 3) c_dx = -1;
		if(cursor_x >= rend.w - 4) c_dx = 1;
		if(cursor_y <= 3) c_dy = -1;
		if(cursor_y >= rend.h - 4) c_dy = 1;
		camera.pos_move(c_dx, c_dy);
	}
border_skip:;
	
	// clear vectors
	hover_items.clear();
	hover_structs.clear();
	hover_units.clear();
	
	Chunk* chnk = world.get_chunk(cursor_cx, cursor_cy);
	if(chnk == nullptr) return;
	// assign hovering item vector
	hover_tile = world.get_tile(cursor_wx, cursor_wy);
	hover_items = hover_tile->items;
	
	// hover structures(check surroundings)
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<2;j++)
		{
			Tile* hover_tile_2 = world.get_tile(cursor_wx - j, cursor_wy - i);
			if(hover_tile_2 == nullptr) continue;
			for(int k=0;k<hover_tile_2->structures.size();k++)
			{
				bool can_insert = true;
				if((i != 0 || j != 0) && struct_man.get(hover_tile_2->structures[k]->id)->size == 1) can_insert = false;
				if(can_insert) hover_structs.push_back(hover_tile_2->structures[k]);
			}
		}
	}
	
	// loop through units
	// (DO DIS)
	
	//
	if(alt_health_view)
	{
		gui.drawing_alt_text = true;
		gui.current_alt_text = "";
		if(hover_structs.size() > 0)
		gui.current_alt_text = std::to_string(hover_structs[0]->health);
		
	} else {
		gui.drawing_alt_text = false;
	}
}

Camera::~Camera(){}


/* GAMELOGIC class */

void GameLogic::init()
{
	tickcount = 0; road_timer = 0;
	
	// input state initialization
	shift_down = false; ctrl_down = false;
	alt_down = false;
	
	dragging_camera = false;
	
	// initialize structure, item and material templates
	struct_man.init();
	item_man.init();
	tile_man.init();
}

void GameLogic::draw()
{
	// determine what chunks are in view
	int cx = std::floor((float)camera.x / (float)CHUNK_SIZE); // top left chunk (x,y)
	int cy = std::floor((float)camera.y / (float)CHUNK_SIZE);
	
	int num_hor = std::ceil((float)rend.w / (float)CHUNK_SIZE)+1;
	int num_ver = std::ceil((float)rend.h / (float)CHUNK_SIZE)+1;
	
	// get chunks seperately to prevent structures from clipping on chunk boundaries
	std::vector<Chunk*> chunks_to_draw;
	// loop through the chunks that are in view
	for(int i=0;i<num_ver;i++)
	{
		for(int j=0;j<num_hor;j++)
		{
			// draw out of bounds chunks
			if(cx+j < MIN_W || cx+j > MAX_W || cy+i < MIN_H || cy + i > MAX_H)
			{
				draw_outside_chunk((cx+j)*CHUNK_SIZE-camera.x,
					(cy+i)*CHUNK_SIZE-camera.y);
				continue;
			}
			
			Chunk* chnk = world.get_chunk(cx+j, cy+i);
			if(chnk == nullptr) // generate if there is no chunk
			{
				bool is_pending = false; // was the chunk already pended to generate
				for(int i=0;i<client.pending_chunks_gen.size();i++)
				{
					std::pair<int,int> coordinates = client.pending_chunks_gen[i];
					if(coordinates.first == cx+j && coordinates.second == cy+i)
					{is_pending = true; break;}
				}
				
				if(!is_pending)
				{
					client.client_command += "chnk " + std::to_string(cx+j) + " " + std::to_string(cy+i) + ";";
					std::pair<int,int> coordinates(cx+j,cy+i); // make it pending for generation
					client.pending_chunks_gen.push_back(coordinates);
				}
				continue;
			}
			// draw the chunk
			draw_chunk(chnk, (cx+j)*CHUNK_SIZE-camera.x,
				(cy+i)*CHUNK_SIZE-camera.y);
			
			chunks_to_draw.push_back(chnk);
		}
	}
	// draw things on top of the chunks
	for(int i=0;i<chunks_to_draw.size();i++)
	{
		// draw structures
		Chunk* chnk = chunks_to_draw[i];
		for(int j=0;j<chnk->structures.size();j++)
		{
			if(chnk->structures[j]->structure_state == WORKING)
				chnk->structures[j]->draw();
			else {
				// draw gear symbol if constructing or rubble
				unsigned int size = struct_man.get(chnk->structures[j]->id)->size;
				for(int k1=0;k1<size;k1++)
					for(int k2=0;k2<size;k2++)
						rend.set(chnk->structures[j]->x - camera.x+k2,chnk->structures[j]->y-camera.y+k1,0x0F,0x03);
			}
		}
		
		// draw items
		for(int i=0;i<chnk->items.size();i++)
		{
			Item* item = chnk->items[i];
			chnk->items[i]->draw();
		}
	}
	// draw effects
	effect_man.draw();
}

void GameLogic::update(std::vector<SDL_Event>& events)
{
	camera.update();
	// input handling
	for(SDL_Event evnt : events)
		update_input(evnt);
}

void GameLogic::update_input(SDL_Event evnt)
{
	// debugger interface
	debug_input(evnt);
	
	// mouse inputs
	if(evnt.type == SDL_MOUSEBUTTONDOWN)
	{
		// start dragging the screen
		if(evnt.button.button == SDL_BUTTON_RIGHT)
		{
			camera.dragging = true;
		}
	}
	if(evnt.type == SDL_MOUSEBUTTONUP)
	{
		// stop dragging the screen
		if(evnt.button.button == SDL_BUTTON_RIGHT)
		{
			camera.dragging = false;
		}
	}
	
	if(shift_down) camera.speed = 10;
	else camera.speed = 1;
	
	std::string constr_id = " 0 ";
	switch(gui.current_construction)
	{
		case FURNACE: constr_id = " 5 "; break;
		case DRILL: constr_id = " 2 "; break;
		case FACTORY: constr_id = " 1 "; break;
		case ARM: constr_id = " 4 "; break;
		case ROAD: constr_id = " 0 "; break;
	}
	
	// keyboard inputs
	if(evnt.type == SDL_KEYDOWN)
	{
		switch(evnt.key.keysym.sym)
		{
			case SDLK_LSHIFT:
				shift_down = true;
			break;
			case SDLK_LALT:
				alt_down = true;
			break;
			case SDLK_LCTRL:
				ctrl_down = true;
			break;
			case SDLK_UP:
				camera.spd_move(0,-1);
			break;
			case SDLK_DOWN:
				camera.spd_move(0,1);
			break;
			case SDLK_LEFT:
				camera.spd_move(-1,0);
			break;
			case SDLK_RIGHT:
				camera.spd_move(1,0);
			break;
			case SDLK_h:
				if(shift_down)
					make_hover_panel();
				if(alt_down)
					camera.alt_health_view = true;
			break;
			
			case SDLK_F1: function_key(1); break;
			case SDLK_F2: function_key(2); break;
			case SDLK_F3: function_key(3); break;
			case SDLK_F4: function_key(4); break;
			case SDLK_F5: function_key(5); break;
			case SDLK_F6: function_key(6); break;
			case SDLK_F7: function_key(7); break;
			case SDLK_F8: function_key(8); break;
			case SDLK_F9: function_key(9); break;
			case SDLK_F10: function_key(10); break;
			case SDLK_F11: function_key(11); break;
			case SDLK_F12: function_key(12); break;
			
			case SDLK_q:
				client.client_command += "build 0 2 " + std::to_string(camera.cursor_x + camera.x) + " "+ std::to_string(camera.cursor_y + camera.y) + " 0;";
			break;
			
			case SDLK_w:
				client.client_command += "build 0" + constr_id + std::to_string(camera.cursor_x + camera.x) + " "+ std::to_string(camera.cursor_y + camera.y) + " 3;";
			break;
			case SDLK_a:
				client.client_command += "build 0" + constr_id + std::to_string(camera.cursor_x + camera.x) + " "+ std::to_string(camera.cursor_y + camera.y) + " 2;";
			break;
			case SDLK_s:
				client.client_command += "build 0" + constr_id + std::to_string(camera.cursor_x + camera.x) + " "+ std::to_string(camera.cursor_y + camera.y) + " 1;";
			break;
			case SDLK_d:
				client.client_command += "build 0" + constr_id + std::to_string(camera.cursor_x + camera.x) + " "+ std::to_string(camera.cursor_y + camera.y) + " 0;";
			break;
			case SDLK_e:
				client.client_command += "spawnitem 0 1 " + std::to_string(camera.cursor_x + camera.x) + " " + std::to_string(camera.cursor_y + camera.y) + ";";
			break;
			case SDLK_z:
				client.client_command += "explode " + std::to_string(camera.cursor_wx) + " " + std::to_string(camera.cursor_wy) + ";";
			break;
		}
	}
	if(evnt.type == SDL_KEYUP)
	{
		switch(evnt.key.keysym.sym)
		{
			case SDLK_LSHIFT:
				shift_down = false;
			break;
			case SDLK_LALT:
				alt_down = false;
			break;
			case SDLK_LCTRL:
				ctrl_down = false;
			break;
			case SDLK_h:
				if(alt_down)
					camera.alt_health_view = false;
			break;
		}
	}
}

void GameLogic::tick(std::string server_command)
{
	tickcount ++;
	// execute server commands
	std::vector<std::string> comm_lines;
	sep_char(';', server_command, comm_lines);
	
	client.pending_chunks_gen.clear(); // clear the chunk generation queue thing
	
	for(int i=0;i<comm_lines.size();i++) // loop through the lines in the commands
	{
		std::vector<std::string> sepline; // separated line
		sep_char(' ', comm_lines[i], sepline);
		std::string comm = sepline[0]; // leading command
		
		if(comm == "seed") // seed rng
			seed_engine((unsigned int)std::stoi(sepline[1]));
		if(comm == "chnk") // generate a chunk
			world.gen_chunk(std::stoi(sepline[1]), std::stoi(sepline[2]));
		if(comm == "init_worldgen") // initialize world generation
			world.init();
		if(comm == "client") // assign client names
		{
			unsigned int cl_id = std::stoi(sepline[1]);
			std::string cl_name = sepline[2];
			client.client_map_str_int[cl_name] = cl_id;
			client.client_map_int_str[cl_id] = cl_name;
		}
		if(comm == "build") // build a structure
		{
			unsigned int client_id = std::stoi(sepline[1]);
			unsigned int struct_id = std::stoi(sepline[2]);
			int x = std::stoi(sepline[3]);
			int y = std::stoi(sepline[4]);
			int orient = std::stoi(sepline[5]);
			
			build_structure(client_id, struct_id, x, y, orient);
		}
		if(comm == "camera_center")
		{
			if(std::stoi(sepline[1]) == client.id)
			{
				int c_x = std::stoi(sepline[2]);
				int c_y = std::stoi(sepline[3]);
				camera.x = c_x - rend.w / 2;
				camera.y = c_y - rend.h / 2;
			}
		}
		if(comm == "spawnunit") // spawn a unit
		{
			
		}
		if(comm == "spawnitem") // spawn an item
		{
			unsigned int item_id = std::stoi(sepline[1]);
			unsigned int mat_id = std::stoi(sepline[2]);
			int x = std::stoi(sepline[3]);
			int y = std::stoi(sepline[4]);
			spawn_item(item_id, mat_id, x, y);
		}
		if(comm == "explode")
		{
			int x = std::stoi(sepline[1]);
			int y = std::stoi(sepline[2]);
			effect_man.make_explosion(x, y, 20, 5, 60, 4);
		}
	}
	
	// execute gamelogic
	// update structures
	for(int i=0;i<structures.size();i++)
	{
		if(structures[i]->structure_state == WORKING)
		{
			structures[i]->update();
			if(structures[i]->health <= 0) // check health
			{
				structures[i]->structure_state = RUBBLE;
				structures[i]->health = 0;
			}
		}
	}
	
	// update road timer
	road_timer ++;
	if(road_timer >= ROAD_TICKCOUNT)
		road_timer = 0;
	
	// move items
	for(int i=0;i<items.size();i++)
		items[i]->update();
	
	// update units
	
	// update effects
	effect_man.update();
}

void GameLogic::function_key(unsigned int _f)
{
	unsigned int f = _f-1;
	// set camera
	if(shift_down)
	{
		func_positions[f].active = true;
		func_positions[f].x = camera.x;
		func_positions[f].y = camera.y;
		return;
	}
	if(ctrl_down)
		if(f == 0)
			if(func_last_position.active)
				camera.set_pos(func_last_position.x, func_last_position.y);
	
	// move camera
	if(func_positions[f].active == false) return;
	func_last_position.x = camera.x;
	func_last_position.y = camera.y;
	func_last_position.active = true;
	camera.set_pos(func_positions[f].x, func_positions[f].y);
}

GameLogic::~GameLogic()
{
	// deallocate memory
	for(int i=0;i<items.size();i++)
		if(items[i] != nullptr) delete items[i];
	for(int i=0;i<units.size();i++)
		if(units[i] != nullptr) delete units[i];
	for(int i=0;i<structures.size();i++)
		if(structures[i] != nullptr) delete structures[i];
}

void build_structure(unsigned int client_id, unsigned int entity_id, int x, int y, unsigned int orientation)
{
	Structure* _build = new Structure;
	_build->init(client_id, entity_id, x, y, orientation);
	
	_build->structure_state = WORKING;
	
	Chunk* curr_chunk = world.get_chunk(_build->cx, _build->cy);
	if(curr_chunk == nullptr) return;
	
	// append to structure vectors in both the chunk and gamelogic
	gamelogic.structures.push_back(_build);
	curr_chunk->add_struct(_build->x - _build->cx * CHUNK_SIZE, _build->y - _build->cy * CHUNK_SIZE, _build);
	// built by current client
	if(client_id == client.id)
	{
		gamelogic.p_structures.push_back(_build);
	}
	
	// change update/draw functions based on id
	unsigned int sid = _build->id;
	if(sid == 0)
		_build->setf(update_road, draw_road, nullptr);
	if(sid == 2)
		_build->setf(update_drill, draw_drill, nullptr);
	if(sid == 3)
		_build->setf(update_incubator, draw_incubator, nullptr);
	if(sid == 4)
		_build->setf(update_furnace, draw_furnace,
	init_furnace);
}

void spawn_item(unsigned int item_id, unsigned int mat_id, int x, int y)
{
	// check if chunk where item is exists(stop if not)
	int cx = std::floor((float)x / (float)CHUNK_SIZE);
	int cy = std::floor((float)y / (float)CHUNK_SIZE);
	if(world.get_chunk(cx, cy) == nullptr) return;
	
	Item* _item = new Item;
	_item->init(item_id, mat_id, x, y);
	gamelogic.items.push_back(_item);
	// update chunk in which the item is spawned
	Chunk* curr_chunk = world.get_chunk(_item->cx, _item->cy);
	if(curr_chunk == nullptr) return;
	curr_chunk->add_item(_item->x - _item->cx * CHUNK_SIZE, _item->y - _item->cy * CHUNK_SIZE,_item);
}