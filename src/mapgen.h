#pragma once

#include "random.h"
#include "renderer.h"
#include "structure.h"
#include "item.h"
#include "unit.h"

#include <ctime>
#include <map>
#include <cmath>
#include <utility>

// debugging
#include "debug.h"

// filepath
#define TILE_DATA_PATH "data/data/tile.dat"

float lerp(float a, float b, float w); // linear interpolation
float smoothstep(float a, float b, float w); // smoothstep interpolation
float smootherstep(float a, float b, float w);

typedef unsigned int t_tileID; // tile id type

// integer type used in world coordinates
typedef int t_coord;
struct Coord
{
	t_coord x, y;
};

// 2d array
template<typename t_dimarray>
class dimArray
{
public:
	unsigned int w, h;

	t_dimarray** data;
	~dimArray();
	void init(unsigned int _w, unsigned int _h);
	void set(unsigned int _x, unsigned int _y, t_dimarray d);
	t_dimarray get_val(unsigned int _x, unsigned int _y);
	t_dimarray* get_ptr(unsigned int _x, unsigned int _y);
	void clear(t_dimarray d);
};
/* Dim array functions */
template<typename t_dimarray>
void dimArray<t_dimarray>::init(unsigned int _w, unsigned int _h)
{
	// initialize the multi dim array
	w = _w; h = _h;
	data = new t_dimarray*[h];
	for(int i=0;i<h;i++)
		data[i] = new t_dimarray[w];
}

template<typename t_dimarray>
dimArray<t_dimarray>::~dimArray()
{
	for(int i=0;i<h;i++)
		if(data[i] != nullptr) delete[] data[i];
	if(data != nullptr) delete[] data;
}

// change (x,y) data of dim array
template<typename t_dimarray>
void dimArray<t_dimarray>::set(unsigned int _x, unsigned int _y, t_dimarray d)
{
	if(_x < w && _y < h) data[_y][_x] = d;
}

// get (x,y) value from dim array
template<typename t_dimarray>
t_dimarray dimArray<t_dimarray>::get_val(unsigned int _x, unsigned int _y)
{
	if(_x < w && _y < h) return data[_y][_x];
}

// get (x,y) pointer from dim array
template<typename t_dimarray>
t_dimarray* dimArray<t_dimarray>::get_ptr(unsigned int _x, unsigned int _y)
{
	if(_x < w && _y < h) return &(data[_y][_x]);
}

// replace all data in dim array with d
template<typename t_dimarray>
void dimArray<t_dimarray>::clear(t_dimarray d)
{
	for(int i=0;i<h;i++)
		for(int j=0;j<w;j++)
			data[i][j] = d;
}

bool biome_curve(float value, float min, float max); // blending between biomes

class Tile
{
public:
	float height;
	int temperature; // degrees kelvin
	unsigned int resource=100; // how many items can you get from this tile
	t_tileID tile_id;
	std::vector<Item*> items;
	std::vector<Structure*> structures;
};

#define CHUNK_SIZE 32 // width and height of chunks
// world borders
#define MAX_W 50
#define MIN_W -50
#define MAX_H 50
#define MIN_H -50
class Chunk
{
public:
	t_coord x, y; // location of the chunk
	dimArray<Tile> tiles;
	void init(t_coord _x, t_coord _y);
	void gen(unsigned int j, unsigned int i); // generate particular coord of chunk
	~Chunk();
	
	std::vector<Structure*> structures;
	std::vector<Item*> items;
	
	void add_item(int _x, int _y, Item* _item); // adds or removes item
	void rem_item(int _x, int _y, Item* _item);
	
	void add_struct(int _x, int _y, Structure* _struct); // adds or removes structure
	void rem_struct(int _x, int _y, Structure* _struct);
};

class World
{
public:
	void init();

	void gen_chunk(t_coord x, t_coord y); // generates a chunk(with safeguard)
	Chunk* get_chunk_gen(t_coord x, t_coord y); // gets a chunk guaranteed(even if it doesnt exist)
	Chunk* get_chunk(t_coord x, t_coord y); // gets chunk(returns nullptr if it doesnt exist)
	void set_chunk(t_coord x, t_coord y, Chunk* chnk);
	
	Tile* get_tile(t_coord x, t_coord y); // get tile at these coordinates
	
	~World();
	
	std::map<std::pair<t_coord, t_coord>, Chunk*> chunks;
};

void draw_chunk(Chunk* chunk, t_coord x, t_coord y); // draws chunk with top left coordinate
// at (x, y) in screen coordinates
void draw_outside_chunk(t_coord x, t_coord y); // draw out of bounds chunk

struct vec2
{
public: float x; float y;
};

float vec2_dot(vec2 v1, vec2 v2);

class Perlin
{
public:
	float grid_size;
	unsigned int seed;

	void init(unsigned int _seed, float _grid_size);
	
	unsigned int seed_by_location(int x, int y);
	float interpolate(float a, float b, float w);
	
	vec2 rand_grid(int gx, int gy); // grid coord (gx, gy)
	float noise(float x, float y); // range [-1, 1]
	float noise_sh(float x, float y); // range [0, 1]
};

// default tile template
class DefTile
{
public:
	std::string name;
	bool use_default_color = true;
	bool use_default_symbol = true;
	unsigned char sym;
	unsigned char col;
	unsigned char id;
	
	bool can_get_item = false;
	unsigned int item_id; // what item can you get from this tile
	unsigned int mat_id; // what is the material of the collected item
};

class TileManager
{
public:
	void init();
	~TileManager();
	DefTile* get(unsigned int id);
	DefTile* get_n(std::string name);
	
	std::map<unsigned int, DefTile*> obj_tile; // all the default tiles
	std::map<std::string, unsigned int> tile_n_id; // tile name to id
};

extern TileManager tile_man;