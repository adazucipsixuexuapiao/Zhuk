#pragma once

#include <map>
#include <string>
#include <vector>

#define ITEM_DATA_PATH "data/data/item.dat"
#define MATERIAL_DATA_PATH "data/data/mat.dat"
class Item
{
public:
	int x, y; // position
	int cx, cy; // chunk position
	int dx, dy; // moving direction
	unsigned int item_id; // item id
	unsigned int mat_id; // material id
	unsigned int weight; // in grams
	int set_chunk_position(); // set item's belonging to chunk(-1 on fail, 0 success)
	void del_chunk_position(); // delete old chunk position
	void update();
	void draw();
	void mov_pos(); // move item by set amount determined by dx, dy
	void set_d(int _dx, int _dy); // change dx, dy
	void init(unsigned int _item_id, unsigned int _mat_id, int _x, int _y);
	bool hauled; // is it not on the ground
};

// default item template from item.dat
class DefItem
{
public:
	void init_default();
	
	std::string name;
	unsigned int id;
	unsigned int weight;
	unsigned char col; // if color is 0x00, then use material color
	unsigned char sym;
	unsigned int vol; // volume in cubic centimeters
};

// default material template from mat.dat
class DefMat
{
public:
	void init_default();
	
	std::string name;
	std::string desc_name; // descriptor name(e.g. "golden")
	unsigned int id;
	float weight_mult; // weight multiplier
	unsigned char col; // color(default color is 0xF0)
	bool flammable = false; // default false
	unsigned int melting_point=99999;
	unsigned int degkg = 0; // degrees per kg(when burning)
	unsigned int cel = 0; // temperature increase
};

class ItemManager
{
public:
	void init();
	
	std::map<unsigned int, DefItem*> obj_item;
	std::map<unsigned int, DefMat*> obj_mat;
	
	DefItem* get_item(unsigned int id);
	DefMat* get_mat(unsigned int id);
	
	// name to id maps
	std::map<std::string, unsigned int> item_n_id;
	std::map<std::string, unsigned int> mat_n_id;
	
	~ItemManager();
};

extern ItemManager item_man;