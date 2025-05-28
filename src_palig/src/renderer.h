// handles ascii rendering

#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>

#include <iostream>
#include <string>

#include "fileparser.h"

#define POS_IMAGE_PATH "data/image/image_pos.png"
#define NEG_IMAGE_PATH "data/image/image_neg.png"
#define COLOR_PATH "data/colors.dat"

class Renderer
{
public:
	unsigned int w, h; // render space in ascii characters
	unsigned char** symbol_grid; // ascii code
	unsigned char** color_grid; // data takes form 0xFB (F - foreground, B - background)
	
	SDL_Renderer* rend; // same as in Window
	
	SDL_Texture* ascii_img_pos;
	SDL_Texture* ascii_img_neg; // loaded ascii images
	SDL_Texture* cursor;
	
	unsigned int tile_w, tile_h; // tile width, height in pixels
	
	// _w, _h are window width and height.
	void init(SDL_Renderer* _rend, unsigned int _w, unsigned int _h, unsigned int t_w, unsigned int t_h);
	void update(); // displays what is in target
	
	void set(int x, int y, unsigned char sym, unsigned char color); // set one of the symbols to something
	
	void draw_symbols(); // puts character grid onto target
	void draw_char(int x, int y, unsigned char sym, unsigned char color, void* pixels, int pitch); // draws a single character at ascii coordinates x,y
	void pp(int x, int y, Uint8 r, Uint8 g, Uint8 b, void* pixels, int pitch); // puts pixel in target
	
	SDL_Color rp(int x, int y, SDL_Surface* surface); // reads pixel from surface(MUST BE LOCKED)
	
	~Renderer();
	
	SDL_Color* colors; // indexes 0 - f
};

#endif