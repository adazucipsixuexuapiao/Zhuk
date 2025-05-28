#ifndef ASCII_LOADER_H
#define ASCII_LOADER_H

#include <fstream>
#include <map>
#include <string>
#include <iostream>

#include "renderer.h"

#define ASCII_PATH "data/img/"

// 0x00 symbol means transparent
class AsciiImage
{
public:
	unsigned char** symbol; // symbol and color grid
	unsigned char** color;
	unsigned int w, h;
	
	void init(unsigned int _w, unsigned int _h);
	~AsciiImage();
	void draw(int x, int y);
};

class AsciiManager
{
public:
	void loadall(); // load all in directory
	void load(std::string filename, std::string mapname);
	
	void draw(std::string name, int x, int y);
	
	AsciiImage* get(std::string name);
	~AsciiManager();
	
	std::map<std::string, AsciiImage*> img;
};

#endif