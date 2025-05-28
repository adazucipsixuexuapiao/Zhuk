// handles ascii rendering

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>

#include <iostream>
#include <string>

#include "fileparser.h"

#define IMAGE_PATH "data/image/image_pos.png"
#define POS_IMAGE_PATH "data/image/image_pos.png"
#define NEG_IMAGE_PATH "data/image/image_neg.png"
#define COLOR_PATH "data/colors.dat"

class Renderer
{
public:
	/* Render space(ASCII grid) */
	unsigned int w, h; // render space in ascii characters
	unsigned char** symbol_grid; // grid of ascii glyphs
	unsigned char** color_grid; // data takes form 0xFB (F - foreground, B - background)
	unsigned char** prev_symbol_grid; // For slower machines, stores previous frame to not draw again
	unsigned char** prev_color_grid;
	
	/* Render space(pixel grid) */
	unsigned int sw, sh, size; // render space in pixels
	SDL_Texture* screen;
	uint32_t* pixels;
	unsigned int tile_w, tile_h; // tile width, height in pixels
	
	SDL_Renderer* rend; // same as in Window
	
	/* Loaded image data */
	char* ascii_pixels;
	SDL_Surface* ascii_img; // loaded ascii images
	SDL_Texture* cursor;
	
	SDL_Color* colors; // indexes 0 - f
	uint32_t colors_dat[16]; // in raw data format
	
	// _w, _h are window width and height.
	void init(SDL_Renderer* _rend, unsigned int _w, unsigned int _h, unsigned int t_w, unsigned int t_h);
	void update(); // displays what is in target
	
	void set(int x, int y, unsigned char sym, unsigned char color); // set one of the symbols to something
	void clear(unsigned char color); // clears symbol_grid and color_grid
	void draw_symbols(); // puts character grid onto target
	void draw_char(int x, int y, unsigned char sym, unsigned char color); // draws a single character at ascii coordinates x,y

	Uint32 getpixel(SDL_Surface *surface, int x, int y);

	/* Cool drawing functions */
	void draw_text(int x, int y, std::string text, unsigned char color);
	void draw_text_w(int x, int y, int w, std::string text, unsigned char color);
	void draw_box_full(int x1, int y1, int x2, int y2, unsigned char color);
	void draw_box_full_cus(int x1, int y1, int x2, int y2, unsigned char border_color, unsigned char color,
		unsigned char fill, unsigned char hor, unsigned char ver, unsigned char tl, unsigned char tr, unsigned char br,
		unsigned char bl);
	void draw_rect(int x1, int y1, int x2, int y2, unsigned char color, unsigned char sym);
	void draw_box_border(int x1, int y1, int x2, int y2, unsigned char color);
	void draw_box_border_cus(int x1, int y1, int x2, int y2, unsigned char color,
		unsigned char hor, unsigned char ver, unsigned char tl, unsigned char tr, unsigned char br, unsigned char bl);

	~Renderer();
};

extern Renderer rend; // global renderer