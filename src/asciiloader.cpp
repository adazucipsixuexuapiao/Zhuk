#include "asciiloader.h"

AsciiImage::~AsciiImage()
{
	for(int i=0;i<h;i++)
	{
		if(symbol[i] != nullptr) delete[] symbol[i];
		if(color[i] != nullptr) delete[] color[i];
	}
	if(symbol != nullptr) delete[] symbol;
	if(color != nullptr) delete[] color;
}

void AsciiImage::init(unsigned int _w, unsigned int _h)
{
	w = _w;
	h = _h;
	symbol = new unsigned char*[h];
	color = new unsigned char*[h];
	for(int i=0;i<h;i++)
	{
		symbol[i] = new unsigned char[w];
		color[i] = new unsigned char[h];
	}
}

void AsciiImage::draw( int x, int y)
{
	for(int i=0;i<h;i++) for(int j=0;j<w;j++)
		if(symbol[i][j] != 0x00) rend.set(j+x, i+y, symbol[i][j], color[i][j]);
}

void AsciiManager::load(std::string filename, std::string mapname)
{
	// open the .img
	std::ifstream file(filename);
	AsciiImage* newimg = new AsciiImage;
	
	// read the width and height from the first two bytes
	unsigned char* header = new unsigned char[2];
	file.read((char*)header, 2);
	
	newimg->init(header[0], header[1]); // initialize the place where the image will be stored
	if(header != nullptr) delete[] header;
	
	// begin reading the image data
	unsigned int buffsize = (newimg->w * newimg->h)*2;
	
	unsigned char* buffer = new unsigned char[buffsize];
	
	file.read((char*)buffer,buffsize);
	file.close();
	
	// first comes a row of symbol information and then a row of color information
	for(int i=0;i<buffsize;i++)
	{
		unsigned int xpos = i % newimg->w;
		unsigned int ypos = (i / newimg->w) / 2;
		if((i / newimg->w) % 2 == 0) newimg->symbol[ypos][xpos] = buffer[i]; else newimg->color[ypos][xpos] = buffer[i];
	}
	if(buffer != nullptr) delete[] buffer; // deallocate memory
	
	img[mapname] = newimg;
}

void AsciiManager::loadall()
{
	// get all filenames in ASCII_PATH directory
}

void AsciiManager::draw(std::string name, int x, int y)
{
	img.at(name)->draw(x,y);
}

AsciiManager::~AsciiManager()
{
	for(auto const& e: img)
		if(e.second != nullptr) delete e.second;
}