#include "structure.h"
#include "gamelogic.h"
#include "miscdraw.h"

#include "fileparser.h"

StructureManager struct_man;

void Structure::update()
{
	timer ++;
	update_func((void*)this);
}

void Structure::draw() {draw_func(this);}

void draw_structure(const unsigned char* d_char, const unsigned char* d_col, int x, int y, unsigned int _orient)
{
	for(int i=0;i<4;i++)
	{
		int dx = x + i%2;
		int dy = y + i/2;
		char* orient = draw_seq_by_orient(_orient);
		rend.set(dx, dy, d_char[orient[i]], d_col[orient[i]]);
	}
}

void Structure::init(unsigned int _client_id, unsigned int _id, int _x, int _y, unsigned int _orient)
{
	timer = 0;
	client_id = _client_id;
	id = _id;
	x = _x;
	y = _y;
	orientation = _orient;
	
	// convert to chunk cordinates
	cx = std::floor((float)x / (float)CHUNK_SIZE);
	cy = std::floor((float)y / (float)CHUNK_SIZE);
}

void Structure::setf(void (*_update_func)(void*), void (*_draw_func)(void*), void (*_init_func)(void*))
{update_func = _update_func; draw_func = _draw_func;
init_func = _init_func; if(init_func == nullptr) init_func = struct_default_init; init_func((void*)this);}

void DefStructure::init_default()
{
	size = 1;
}

void StructureManager::init()
{
	std::vector<std::vector<std::string>> dat_parse;
	load_parse_datfile(STRUCTURE_DATA_PATH, dat_parse);
	
	// loop through the structures
	for(int i=0;i<dat_parse.size();i++)
	{
		DefStructure* def_struct = new DefStructure;
		def_struct->init_default();
		// loop through lines
		for(int j=0;j<dat_parse[i].size();j++)
		{
			std::vector<std::string> sepline;
			sep_char(' ', dat_parse[i][j], sepline);
			std::string comm = sepline[0];
			if(comm == "n") def_struct->name = sepline[1];
			if(comm == "id") def_struct->id = std::stoi(sepline[1]);
			if(comm == "size") def_struct->size = std::stoi(sepline[1]);
		}
		obj[def_struct->id] = def_struct; // assign to map
	}
}

DefStructure* StructureManager::get(int id) {return obj[id]; }

StructureManager::~StructureManager()
{
	// free memory from structure templates
	for(auto & x : obj)
		if(x.second != nullptr) delete x.second;
	
	// free memory from furnaces
	for(auto & x : furnaces)
		if(x.second != nullptr) delete x.second;
}

void struct_default_init(void* def_arg) {}

#define DRILL_TIMER 100
// drill functions
void update_drill(void* _drill)
{
	Structure* drill = (Structure*)_drill;
	// spawn item
	if(drill->timer >= DRILL_TIMER)
	{
		int ix, iy; // item x, y(relative)
		unsigned int orient = drill->orientation;
		if(orient == 0) {ix = 2; iy = 1;} // set item position based on orientation
		if(orient == 1) {ix = 0; iy = 2;}
		if(orient == 2) {ix = -1; iy = 0;}
		if(orient == 3) {ix = 1; iy = -1;}
		drill->timer = 0;
		
		// check squares under drill
		for(int i=0;i<2;i++)
			for(int j=0;j<2;j++)
			{
				Tile* tile_under = world.get_tile(drill->x+j, drill->y+i);
				DefTile* tile_under_def = tile_man.get(tile_under->tile_id);
				if(tile_under->resource > 0) // check if there are any resources there
				{
					if(tile_under_def->can_get_item) // check if actually can mine
					{
						// spawn item and subtract resources
						spawn_item(tile_under_def->item_id,
						tile_under_def->mat_id, drill->x+ix, drill->y+iy);
						tile_under->resource --;
						goto end_drill;
					}
				} else { // turn the tile into mud if nothing
					tile_under->tile_id = tile_man.tile_n_id["mud"];
				}
			}
		end_drill:;
	}
}

void draw_drill(void* _drill)
{
	Structure* drill = (Structure*)_drill;
	unsigned int x, y; // coordinates of top left
	x = drill->x - camera.x;
	y = drill->y - camera.y;
	
	// default orientation
	unsigned char drill_char[4] = {
		0x2a, 0xb8,
		0xd3, 0x1a
	};
	unsigned char drill_color[4] = {
		0x73, 0x73,
		0x73, 0xb3
	};
	
	draw_structure(drill_char, drill_color, x, y, drill->orientation);
	
	// change arrow by orientation
	char arrow_sym = arrow_by_orient(drill->orientation);
	rend.set(x + p_by_orient((drill->orientation + 2) % 4) % 2, y + p_by_orient((drill->orientation+2)%4) / 2, arrow_sym, drill_color[3]);
}

// road functions
void update_road(void* _road)
{
	if(gamelogic.road_timer != 0) return; // dont do anything if its not the road's turn yet
	Structure* road = (Structure*)_road;
	Chunk* chnk = world.get_chunk(road->cx, road->cy); // chunk the road is in
	// get road tile
	Tile* curr_tile = chnk->tiles.get_ptr(road->x - road->cx * CHUNK_SIZE, road->y - road->cy * CHUNK_SIZE);
	for(int i=0;i<curr_tile->items.size();i++)
	{
		Item* item = curr_tile->items[i];
		int dx, dy;
		// move item accordingly
		if(road->orientation == 0) {dx = 1; dy = 0;}
		if(road->orientation == 1) {dx = 0; dy = 1;}
		if(road->orientation == 2) {dx = -1; dy = 0;}
		if(road->orientation == 3) {dx = 0; dy = -1;}
		item->set_d(dx, dy);
	}
}

void draw_road(void* _road)
{
	Structure* road = (Structure*)_road;
	unsigned int x, y;
	x = road->x - camera.x;
	y = road->y - camera.y;
	
	unsigned char col = 0x18;
	unsigned char sym;
	unsigned int orient = road->orientation;
	if(orient == 0) sym = 0x1a;
	if(orient == 1) sym = 0x19;
	if(orient == 2) sym = 0x1b;
	if(orient == 3) sym = 0x18;
	
	rend.set(x,y,sym,col);
}

void update_incubator(void* _incub)
{
	
}

void draw_incubator(void* _incub)
{
	Structure* incub = (Structure*)_incub;
	unsigned int x, y; // coordinates of top left
	x = incub->x - camera.x;
	y = incub->y - camera.y;
	// default orientation
	unsigned char drill_char[4] = {
		0x0a, 0x0a,
		0x0a, 0x0a
	};
	unsigned char drill_color[4] = {
		0x5d, 0x5d,
		0x5d, 0x5d
	};
	draw_structure(drill_char, drill_color, x, y, incub->orientation);
}

void draw_furnace(void* _furn)
{
	Structure* furn = (Structure*)_furn;
	unsigned int x, y; // coordinates of top left
	x = furn->x - camera.x;
	y = furn->y - camera.y;
	
	unsigned char furn_char[4] = {
		0xd7, 0xa9,
		0xd7, 0x1e
	};
	unsigned char furn_col[4] = {
		0x87, 0x87,
		0x87, 0xb1
	};
	draw_structure(furn_char, furn_col, x, y, 0);
}

StructFurnace::~StructFurnace()
{
	struct_man.furnaces.erase(parent);
}

void init_furnace(void* _furn)
{
	Structure* furn = (Structure*)_furn;
	struct_man.furnaces[furn] = new StructFurnace;
	StructFurnace* str_furn = struct_man.furnaces[furn];
	str_furn->parent = furn;
	
	str_furn->temperature = world.get_tile(furn->x, furn->y)->temperature;
	str_furn->desired_temperature = str_furn->temperature;
	str_furn->max_temperature = 1000;
}

#define FURNACE_TIMER 100
#define FURNACE_COOLING_TIMER 30
void update_furnace(void* _furn)
{
	Structure* furn = (Structure*)_furn;
	StructFurnace* furn_str = struct_man.furnaces.at(furn);

	furn_str->burn_timer ++;
	furn_str->cool_timer ++;
	
	// temperature too high = explosion :)
	if(furn_str->temperature > furn_str->max_temperature)
		effect_man.make_explosion(furn->x, furn->y, 20, 5, 20, 4);

	// burning requirements met
	if(furn_str->burn_timer > FURNACE_TIMER && furn_str->temperature < furn_str->desired_temperature)
	{
		// can burn something
		if(furn_str->fuel.size() > 0)
		{
			Item* burnable = furn_str->fuel[0];
			DefMat* burnable_mat = item_man.get_mat(burnable->mat_id);
			DefItem* burnable_it = item_man.get_item(burnable->item_id);
			// make heat appropriately
			unsigned int temp_increase = 0;
			
			unsigned int wt = (unsigned int)((float)burnable_it->weight * burnable_mat->weight_mult);
			temp_increase = (unsigned int)((float)wt / 1000.0f * burnable_mat->degkg);
			
			delete furn_str->fuel[0]; // destroy the item
			furn_str->fuel.erase(furn_str->fuel.begin());
			furn_str->burn_timer = 0;
			furn_str->temperature += temp_increase;
			
			effect_man.make_smoke_puff(furn->x, furn->y, 5);
		}
	}
	
	// cooling
	if(furn_str->cool_timer > FURNACE_COOLING_TIMER)
	{
		furn_str->cool_timer = 0;
		// cool the furnace if requirements met
		if(furn_str->temperature > world.get_tile(furn->x, furn->y)->temperature)
			furn_str->temperature --;
	}
}