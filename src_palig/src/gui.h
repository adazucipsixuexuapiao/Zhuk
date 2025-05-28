#ifndef GUI_H
#define GUI_H

#include "random.h"
#include "renderer.h"
#include <vector>
#include <cmath>



class GuiManager
{
public:
	void init(Renderer* _rend); // gets called at the very start
	
	void draw_cursor();
	SDL_Rect cursor_rect;
	
	void draw_menu_gui(std::vector<SDL_Event>& events); // main menu
	
	void draw_text(int x, int y, std::string text, unsigned char color);
	void draw_box_full(int x1, int y1, int x2, int y2, unsigned char color);
	void draw_box_border(int x1, int y1, int x2, int y2, unsigned char color);
private:
	Renderer* rend;
	
	bool holding_mouse;
	
	bool showing_transparency;
	
	unsigned char selected_fg, selected_bg;
	unsigned char selected_symbol;
	
	unsigned char** canvas_symbol;
	unsigned char** canvas_color;
};

#endif