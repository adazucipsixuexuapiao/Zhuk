#include "mapgen.h"

TileManager tile_man;

float lerp(float a, float b, float w){return (a*(1-w) + b*w);}

float vec2_dot(vec2 v1, vec2 v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

float smoothstep(float a, float b, float w)
{
	return (b - a) * (3.0 - w * 2.0) * w * w + a;
}

float smootherstep(float a, float b, float w)
{
	return (b - a) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a;
}
	
/* Perlin noise implementation */
void Perlin::init(unsigned int _seed, float _grid_size)
{
	grid_size = _grid_size; seed = _seed;
}

//4-8 . 7 e-005
unsigned int Perlin::seed_by_location(int x, int y)
{	
	// make sure nothing is 0
	if(x >= 0) x++;
	else x --;
	
	if(y >= 0) y++;
	else y --;
	
	std::srand((unsigned int)x * seed);
	unsigned int x2 = std::rand();
	std::srand((unsigned int)y * seed);
	unsigned int y2 = std::rand();
	
	std::srand(x2*y2);
	return std::rand();
	
}

vec2 Perlin::rand_grid(int gx, int gy)
{	
	// seed by location
	unsigned int _gridseed = seed_by_location(gx, gy);
	std::srand(_gridseed);
	// calculate the randomly facing unit vector
	vec2 out;
	//float angle = (float)eng_grand(0,360, grid_eng) / 360.0f * 2.0f * (3.1415926f);
	float angle = (float)(std::rand() % 360) / 360.0f * 2.0f * 3.1415926f;
	out.x = std::cos(angle);
	out.y = std::sin(angle);
	
	return out;
}

// 0.000(2-3)
float Perlin::noise(float x, float y)
{
	// calculate top left grid coordinate
	int gx = std::floor(x / grid_size);
	int gy = std::floor(y / grid_size);
	
	// grid coordinates to world coordinates
	float w_gx = gx * grid_size;
	float w_gy = gy * grid_size;
	
	vec2 g00, g10, g01, g11; // surrounding grid vectors
	g00 = rand_grid(gx, gy);
	g10 = rand_grid(gx+1, gy);
	g01 = rand_grid(gx, gy+1);
	g11 = rand_grid(gx+1, gy+1);
	
	// x and y interpolation weights
	float ix = (float)(x-w_gx) / grid_size;
	float iy = (float)(y-w_gy) / grid_size;
	
	// calculate dot products with offset vectors and interpolate them

	float dp1 = interpolate(
				vec2_dot(g00, (vec2){(x-w_gx)/grid_size,(y-w_gy)/grid_size}),
				vec2_dot(g10, (vec2){(x-w_gx-grid_size)/grid_size,(y-w_gy)/grid_size}),
				ix
				);
	float dp2 = interpolate(
				vec2_dot(g01, (vec2){(x-w_gx)/grid_size,(y-w_gy-grid_size)/grid_size}),
				vec2_dot(g11, (vec2){(x-w_gx-grid_size)/grid_size,(y-w_gy-grid_size)/grid_size}),
				ix
				);
	float retval = interpolate(dp1, dp2, iy);
	return retval;
}

float Perlin::noise_sh(float x, float y) { return 0.5f * (noise(x,y)+1.0f); }

// 8.8 e-007
float Perlin::interpolate(float a, float b, float w)
{
	return smootherstep(a,b,w);
}

/* auxilliary functions */

void draw_chunk(Chunk* chunk, t_coord x, t_coord y)
{
	if(chunk == nullptr) return;
	for(int i=0;i<CHUNK_SIZE;i++)
	{
		for(int j=0;j<CHUNK_SIZE;j++)
		{
			unsigned char col=0x00, sym=0x00;
			float hmap = chunk->tiles.get_ptr(j,i)->height;
			
			if(hmap >= 0.0f) // above sea level
			{
				if(hmap > 0.20f) sym = 0xb2;
				else if(hmap > 0.10f) sym = 0xb1;
				else sym = 0xb0;
			} else { // below sea level
				if(-hmap > 0.20f) sym = 0xb0;
				else if(-hmap > 0.10f) sym = 0xb1;
				else sym = 0xb2;
			}
			
			t_tileID tile = chunk->tiles.get_ptr(j,i)->tile_id;
			
			DefTile* def_tile = tile_man.get(tile);
			if(def_tile->use_default_color == false) col = def_tile->col;
			if(def_tile->use_default_symbol == false) sym = def_tile->sym;
			
			rend.set(x+j, y+i, sym, col);
		}
	}
}

void draw_outside_chunk(t_coord x, t_coord y)
{	
	for(int i=0;i<CHUNK_SIZE;i++)
		for(int j=0;j<CHUNK_SIZE;j++)
			rend.set(x+j, y+i, 0xb0, 0x10);
}

// perlin generator (octave noise)
Perlin per_1;
Perlin per_2;

Perlin per_temperature;
Perlin per_humidity;

// resource noises
Perlin per_res_oil;
Perlin per_res_iron;
Perlin per_res_copper;

bool biome_curve(float value, float min, float max)
{
	if(value < min) return false;
	if(value > max) return true;
	
	// value is between min and max
	float k = (value - min) / (max - min); // what percentage
	float prob = smootherstep(0.0f, 1.0f, k);
	if(genrand(0, 100) < prob*100.0f)
		return true;
	return false;
}

bool get_resource(float value)
{
	if(value >= 0.4f) return true;
	else return false;
}

void set_resource(Tile* tile, unsigned int id, float value)
{
	tile->tile_id = id;
	tile->resource = (unsigned int)std::fabs(-10000.0f*(value - 0.4f)*(value - 0.6f));
}

void Chunk::gen(unsigned int j, unsigned int i)
{
	// convert to world coordinates
	float w_x = (float)((int)j + x*CHUNK_SIZE);
	float w_y = (float)((int)i + y*CHUNK_SIZE);
	
	// get noise values
	float val_terr_1 = per_1.noise(w_x, w_y);
	float val_terr_2 = per_2.noise(w_x, w_y);
	float terrain = val_terr_1 * 0.7 + val_terr_2 * 0.3;
	
	float value_temp = per_temperature.noise(w_x, w_y);
	value_temp -= 0.5f*(terrain+1.0f)*0.2f;
	
	Tile* tile = tiles.get_ptr(j,i);
	tile->temperature = value_temp*100+273;
	
	if(terrain >= 0.0f) // land
	{
		tile->tile_id = 1; // mud
		if(value_temp <= -0.2f &&
		biome_curve(-value_temp, 0.2f, 0.21f)) // colder
			tile->tile_id = 3; // ice
		else if(value_temp >= 0.2f &&
		biome_curve(value_temp, 0.2f, 0.21f)) // hotter
			tile->tile_id = 2; // desert
	}
	else tile->tile_id = 0;
	tile->height = terrain;
	
	if(terrain >= 0.0f)
	{
		// generate resources
		float val_iron = per_res_iron.noise(w_x, w_y);
		float val_copper = per_res_copper.noise(w_x, w_y);
		
		if(get_resource(val_iron)) set_resource(tile, 4, val_iron);
		if(get_resource(val_copper)) set_resource(tile, 5, val_copper);
	}
}

/* Chunk functions */
void Chunk::init(t_coord _x, t_coord _y)
{
	tiles.init(CHUNK_SIZE, CHUNK_SIZE);
	x = _x; y = _y;
	
	// temporary random generation
	for(int i=0;i<CHUNK_SIZE;i++)
	{
		for(int j=0;j<CHUNK_SIZE;j++)
		{
			gen(j, i);
		}
	}
}

void Chunk::add_item(int _x, int _y, Item* _item)
{
	items.push_back(_item);
	Tile* tile = tiles.get_ptr(_x, _y);
	tile->items.push_back(_item);
}
void Chunk::rem_item(int _x, int _y, Item* _item)
{
	
}

void Chunk::add_struct(int _x, int _y, Structure* _struct)
{
	structures.push_back(_struct);
	Tile* tile = tiles.get_ptr(_x, _y);
	tile->structures.push_back(_struct);
}

void Chunk::rem_struct(int _x, int _y, Structure* _struct)
{
	
}

Chunk::~Chunk()
{
	// cleanup
}

/* World functions */

void World::init()
{
	const float scale = 0.7f;
	
	// perlin generator (octave noise)
	per_1.init(genrand(1,INT_MAX-1), 50.0f * scale);
	per_2.init(genrand(1,INT_MAX-1), 10.0f * scale);

	per_temperature.init(genrand(1,INT_MAX-1), 200.0f * scale);
	per_humidity.init(genrand(1,INT_MAX-1), 500.0f * scale);
	
	per_res_oil.init(genrand(1,INT_MAX-1), 20.0f * scale);
	per_res_iron.init(genrand(1,INT_MAX-1), 20.0f * scale);
	per_res_copper.init(genrand(1,INT_MAX-1), 20.0f * scale);
}

Chunk* World::get_chunk_gen(t_coord x, t_coord y)
{
	Chunk* chnk = get_chunk(x, y);
	if(chnk == nullptr)
	{
		chnk = new Chunk;
		chnk->init(x, y);
		set_chunk(x,y,chnk);
	}
	return chnk;
}

void World::gen_chunk(t_coord x, t_coord y)
{
	// outside of world boundaries
	if(x > MAX_W || x < MIN_W || y > MAX_H || y < MIN_H) return;
	// safeguard
	if(chunks.count(std::pair<int,int>(x,y)) > 0)
		return;
	
	// generate the chunk
	Chunk* chnk = new Chunk;
	chnk->init(x,y);
	set_chunk(x,y,chnk);
}

Chunk* World::get_chunk(t_coord x, t_coord y)
{
	Chunk* ret = nullptr;
	try
	{
		ret = chunks.at(std::pair<int,int>(x,y));
	} catch (const std::out_of_range &e) {
		ret = nullptr;
	}
	return ret;
}
void World::set_chunk(t_coord x, t_coord y, Chunk* chnk)
{
	chunks[std::pair<int,int>(x,y)] = chnk;
}

World::~World()
{
	
}

Tile* World::get_tile(t_coord x, t_coord y)
{
	int cx = std::floor((float)x / (float)CHUNK_SIZE);
	int cy = std::floor((float)y / (float)CHUNK_SIZE);
	
	Chunk* chnk = get_chunk(cx, cy);
	if(chnk == nullptr) return nullptr;
	
	return chnk->tiles.get_ptr(x - cx*CHUNK_SIZE, y - cy*CHUNK_SIZE);
}

void TileManager::init()
{
	// load tile templates
	std::vector<std::vector<std::string>> dat_parse_tile;
	load_parse_datfile(TILE_DATA_PATH, dat_parse_tile);
	
	for(int i=0;i<dat_parse_tile.size();i++)
	{
		DefTile* def_tile = new DefTile;
		
		for(int j=0;j<dat_parse_tile[i].size();j++)
		{
			std::vector<std::string> sepline;
			sep_char(' ', dat_parse_tile[i][j], sepline);
			std::string comm = sepline[0];
			if(comm == "n") def_tile->name = sepline[1];
			if(comm == "id") def_tile->id = std::stoi(sepline[1]);
			if(comm == "sym")
			{
				def_tile->use_default_symbol = false;
				def_tile->sym = hex_conv(sepline[1]);
			}
			if(comm == "col")
			{
				def_tile->use_default_color = false;
				def_tile->col = hex_conv(sepline[1]);
			}
			if(comm == "it")
			{
				def_tile->can_get_item = true;
				def_tile->item_id = item_man.item_n_id.at(sepline[1]);
			}
			if(comm == "mat")
			{
				def_tile->mat_id = item_man.mat_n_id.at(sepline[1]);
			}
		}
		
		obj_tile[def_tile->id] = def_tile;
		tile_n_id[def_tile->name] = def_tile->id;
	}
}

TileManager::~TileManager()
{
	// free memory from tile templates
	for(auto & x : obj_tile)
		if(x.second != nullptr) delete x.second;
}

DefTile* TileManager::get(unsigned int id) { return obj_tile.at(id); }
DefTile* TileManager::get_n(std::string name)
{
	return obj_tile.at(tile_n_id.at(name));
}