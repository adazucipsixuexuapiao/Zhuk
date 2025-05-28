#include "item.h"
#include "gamelogic.h"

ItemManager item_man;

int Item::set_chunk_position()
{	
	int new_cx = std::floor((float)x / (float)CHUNK_SIZE);
	int new_cy = std::floor((float)y / (float)CHUNK_SIZE);

	Chunk* new_chunk = world.get_chunk(new_cx, new_cy);
	if(new_chunk == nullptr) return -1; // error, no such chunk
	
	// place item in chunk if new cx,cy and remove old instance of item from vec
	if(cx != new_cx || cy != new_cy)
	{
		Chunk* old_chunk = world.get_chunk(cx, cy);
		for(int i=0;i<old_chunk->items.size();i++)
			if(old_chunk->items[i] == this) {old_chunk->items.erase(old_chunk->items.begin()+i); break;}
		Chunk* new_chunk = world.get_chunk(new_cx, new_cy);
		new_chunk->items.push_back(this);
	}
	
	cx = new_cx; cy = new_cy; // update chunk coordinates
	
	Tile* curr_tile = new_chunk->tiles.get_ptr(x - cx * CHUNK_SIZE, y - cy * CHUNK_SIZE);
	curr_tile->items.push_back(this);
	
	return 1;
}

void Item::del_chunk_position()
{
	Chunk* this_chunk = world.get_chunk(cx, cy);
	Tile* this_tile = this_chunk->tiles.get_ptr(x - cx * CHUNK_SIZE, y - cy * CHUNK_SIZE);
	for(int i=0;i<this_tile->items.size();i++) // find the current item in the chunk and delete it
		if(this_tile->items[i] == this)
		{
			this_tile->items.erase(this_tile->items.begin()+i);
			break;
		}
}

void Item::update()
{
	if(hauled) return;
	mov_pos(); // change position based on dx, dy
}

void Item::draw()
{
	if(hauled) return;
	int _x = x - camera.x;
	int _y = y - camera.y;

	unsigned char symbol = item_man.obj_item[item_id]->sym;
	unsigned char color = item_man.obj_mat[mat_id]->col;
	
	rend.set(_x, _y, symbol, color);
}

void Item::mov_pos()
{
	int val = 0;
	if(dx != 0 || dy != 0)
	{
		del_chunk_position();
		x += dx; y += dy;
		val = set_chunk_position();
		
		// check if any items on this tile
		if(val != -1)
		{
			Chunk* this_chunk = world.get_chunk(cx, cy);
			// found another item on tile, reverse item
			if(this_chunk->tiles.get_ptr(x-cx*CHUNK_SIZE, y-cy*CHUNK_SIZE)->items.size() >= 2)
			{
				del_chunk_position();
				x -= dx; y -= dy;
				val = set_chunk_position();
			}
		}
	}
	
	// on error
	if(val == -1)
	{
		// no need to del_chunk_position() again, because it is already deleted at this point
		// reverse item
		x -= dx; y -= dy;
		set_chunk_position();
	}
	
	// stop moving
	dx = 0;
	dy = 0;
}

void Item::set_d(int _dx, int _dy)
{
	dx = _dx; dy = _dy;
}

void Item::init(unsigned int _item_id, unsigned int _mat_id, int _x, int _y)
{
	hauled = false;
	x = _x;
	y = _y;
	cx = std::floor((float)x / (float)CHUNK_SIZE);
	cy = std::floor((float)y / (float)CHUNK_SIZE);
	item_id = _item_id;
	mat_id = _mat_id;
	weight = 1;
	dx = 0; dy = 0;
}

void DefItem::init_default() {}
void DefMat::init_default() {desc_name = ""; col = 0xF0;
flammable = false;}
void ItemManager::init()
{
	std::vector<std::vector<std::string>> dat_parse_item;
	std::vector<std::vector<std::string>> dat_parse_mat;
	load_parse_datfile(ITEM_DATA_PATH, dat_parse_item);
	load_parse_datfile(MATERIAL_DATA_PATH, dat_parse_mat);
	
	// loop through the items
	for(int i=0;i<dat_parse_item.size();i++)
	{
		DefItem* def_item = new DefItem;
		def_item->init_default();
		// loop through lines
		for(int j=0;j<dat_parse_item[i].size();j++)
		{
			std::vector<std::string> sepline;
			sep_char(' ', dat_parse_item[i][j], sepline);
			std::string comm = sepline[0];
			if(comm == "n")
			{
				def_item->name = sepline[1];
			}
			if(comm == "id")
			{
				def_item->id = std::stoi(sepline[1]);
			}
			if(comm == "wt")
			{
				def_item->weight = std::stoi(sepline[1]);
			}
			if(comm == "sym")
			{
				// convert symbol from hex to int
				def_item->sym = hex_conv(sepline[1]);
			}
			if(comm == "vol")
			{
				def_item->vol = std::stoi(sepline[1]);
			}
		}
		obj_item[def_item->id] = def_item; // assign to maps
		item_n_id[def_item->name] = def_item->id;
	}
	
	// loop through the materials
	for(int i=0;i<dat_parse_mat.size();i++)
	{
		DefMat* def_mat = new DefMat;
		def_mat->init_default();
		// loop through lines
		for(int j=0;j<dat_parse_mat[i].size();j++)
		{
			std::vector<std::string> sepline;
			sep_char(' ', dat_parse_mat[i][j], sepline);
			std::string comm = sepline[0];
			if(comm == "n")
			{
				def_mat->name = sepline[1];
			}
			if(comm == "desc")
			{
				def_mat->desc_name = sepline[1];
			}
			if(comm == "id")
			{
				def_mat->id = std::stoi(sepline[1]);
			}
			if(comm == "wm")
			{
				def_mat->weight_mult = std::stof(sepline[1]);
			}
			if(comm == "col")
			{
				def_mat->col = hex_conv(sepline[1]);
			}
			if(comm == "flam")
			{
				def_mat->flammable = (bool)std::stoi(sepline[1]);
			}
			if(comm == "cel")
			{
				def_mat->cel = std::stoi(sepline[1]);
			}
			if(comm == "melt")
			{
				def_mat->melting_point = std::stoi(sepline[1]);
			}
			if(comm == "degkg")
			{
				def_mat->degkg = std::stoi(sepline[1]);
			}
		}
		
		if(def_mat->desc_name == "") def_mat->desc_name = def_mat->name; // use material name if no descriptor
		
		obj_mat[def_mat->id] = def_mat; // assign to maps
		mat_n_id[def_mat->name] = def_mat->id;
	}
}

DefItem* ItemManager::get_item(unsigned int id) {return obj_item.at(id);}
DefMat* ItemManager::get_mat(unsigned int id) {return obj_mat.at(id);}

ItemManager::~ItemManager()
{
	// free memory from item templates
	for(auto & x : obj_item)
		if(x.second != nullptr) delete x.second;
	// free memory from material templates
	for(auto & x : obj_mat)
		if(x.second != nullptr) delete x.second;
}