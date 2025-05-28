#include "gui.h"

#define CANVAS_W 40
#define CANVAS_H 40

void GuiManager::init(Renderer* _rend)
{
	holding_mouse = false;
	showing_transparency = false;
	rend = _rend;
	SDL_ShowCursor(SDL_DISABLE);
	cursor_rect.w = 16; cursor_rect.h = 16;

	canvas_symbol = new unsigned char*[rend->h];
	canvas_color = new unsigned char*[rend->h];
	for(int i=0;i<rend->h;i++)
	{
		canvas_symbol[i] = new unsigned char[rend->w];
		canvas_color[i] = new unsigned char[rend->w];
		for(int j=0;j<rend->w;j++)
		{
			canvas_symbol[i][j] = 0x00;
			canvas_color[i][j] = 0x00;
		}
	}
	
	selected_fg = 0x00;
	selected_bg = 0x00;
	selected_symbol = 0x00;
}

void GuiManager::draw_menu_gui(std::vector<SDL_Event>& events)
{
	// cursor in ascii
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	
	cursor_rect.x = mx; cursor_rect.y = my;
	
	int mouse_x = (int)(mx / rend->tile_w);
	int mouse_y = (int)(my / rend->tile_h);
	
	char hex_char[17] = "0123456789ABCDEF";
	// symbol
	std::string str = "";
	str += hex_char[selected_symbol / 16];
	str += hex_char[selected_symbol % 16];
	draw_text(0,17,"Sym:",0xf0);
	draw_text(4,17, str, 0xf0);
	
	// color
	str = "";
	str += hex_char[selected_fg];
	str += hex_char[selected_bg];
	draw_text(0,18,"Col:",0xf0);
	draw_text(4,18,str, 0xf0);
	
	// hovering
	unsigned char sel_col, sel_sym;
	if(mouse_x >= 17 && mouse_x < 17+CANVAS_W && mouse_y >= 3 && mouse_y < 3+CANVAS_H)
		{
			sel_sym = canvas_symbol[mouse_y-3][mouse_x-17];
			sel_col = canvas_color[mouse_y-3][mouse_x-17];
		}
	
	// symbol
	str = "";
	str += hex_char[sel_sym / 16];
	str += hex_char[sel_sym % 16];
	draw_text(0,19,"Sym:",0xf0);
	draw_text(4,19, str, 0xf0);
	
	// color
	str = "";
	str += hex_char[sel_col / 16];
	str += hex_char[sel_col % 16];
	draw_text(0,20,"Col:",0xf0);
	draw_text(4,20,str, 0xf0);
	
	// color picker
	draw_text(16,0,"Foreground:",0xf0);
	draw_text(16,1,"Background:",0xf0);
	
	for(int i=0;i<0x10;i++)
	{
		if(i == selected_fg) rend->set(16+i+11,0,0xff,((i!=0x0f)?(i | 0xf0):i)); else rend->set(i+11+16,0,0x00,i);
		if(i == selected_bg) rend->set(16+i+11,1,0xff,((i!=0x0f)?(i | 0xf0):i)); else rend->set(i+11+16,1,0x00,i);
	}
	
	// symbol picker
	for(unsigned char i=0;i<16;i++)
	{
		for(unsigned char j=0;j<16;j++)
		{
			unsigned char sym = j | (i << 4);
			rend->set(j,i,sym,(sym == selected_symbol?0xf1:0xf0));
		}
	}
	
	draw_text(0,16,"Transparency:", 0xf0);
	if(showing_transparency)
	{
		draw_text(13,16, "shown", 0xf0);
	} else draw_text(13,16, "not shown", 0xf0);
	
	// canvas
	draw_box_border(16, 2, 16+CANVAS_W+1, 2+CANVAS_H+1, 0xf0);
	for(int i=0;i<CANVAS_H;i++)
	{
		for(int j=0;j<CANVAS_W;j++)
		{
			if(canvas_symbol[i][j] == 0x00 && showing_transparency) // transparent
				rend->set(17+j,3+i, 0x00, 13);
			else rend->set(17+j,3+i,canvas_symbol[i][j], canvas_color[i][j]);
		}
	}
	
	rend->set(mouse_x, mouse_y, 0x18, 0xC0);
	
	// event handling
	for(int i=0;i<events.size();i++)
	{
		SDL_Event evnt = events[i];
		if(evnt.type == SDL_KEYDOWN)
		{
			switch(evnt.key.keysym.sym)
			{
				case SDLK_t:
					showing_transparency = !showing_transparency;
				break;
				
				case SDLK_s: // save
					std::ofstream output;
					
					output.open("image.img", std::ios::binary | std::ios::out);
					
					unsigned char header[] = {CANVAS_W, CANVAS_H};
					output.write((char*)header, 2);
					
					for(int i=0;i<CANVAS_H;i++)
					{
						output.write((char*)canvas_symbol[i], CANVAS_W);
						output.write((char*)canvas_color[i], CANVAS_W);
					}
					
					output.close();
				break;
			}
		}
		if(evnt.type == SDL_MOUSEBUTTONDOWN)
		{
			// check what button has been pressed
			
			// foreground color
			if(mouse_x >= 27 && mouse_x < 27+16 && mouse_y == 0)
				selected_fg = mouse_x - 27;
			
			// background color
			if(mouse_x >= 27 && mouse_x < 27+16 && mouse_y == 1)
				selected_bg = mouse_x - 27;
			
			// symbol picker
			if(mouse_x >= 0 && mouse_x < 16 && mouse_y >= 0 && mouse_x < 16)
				selected_symbol = mouse_x | (mouse_y << 4);
			
			holding_mouse = true;
		}
		if(evnt.type == SDL_MOUSEBUTTONUP)
		{
			holding_mouse = false;
		}
	}
	
	// paint
	if(holding_mouse)
	{
		if(mouse_x >= 17 && mouse_x < 17+CANVAS_W && mouse_y >= 3 && mouse_y < 3+CANVAS_H)
		{
			canvas_symbol[mouse_y-3][mouse_x-17] = selected_symbol;
			canvas_color[mouse_y-3][mouse_x-17] = selected_bg | (selected_fg << 4);
		}
	}
}

void GuiManager::draw_text(int x, int y, std::string text, unsigned char color)
{
	int _y = y;
	int _x = x;
	for(int i=0;i<text.length();i++)
	{
		unsigned char letter = text[i];
		if(letter == '\n') {_x = x; _y++; continue;}
		rend->set(_x, _y, letter, color);
		_x++;
	}
}

void GuiManager::draw_cursor()
{	
	// cursor icon
	SDL_RenderCopy(rend->rend, rend->cursor, NULL, &cursor_rect);
}

void GuiManager::draw_box_full(int x1, int y1, int x2, int y2, unsigned char color)
{
	for(int i=0;i<=y2-y1;i++)
	{
		for(int j=0;j<=x2-x1;j++)
		{
			// top and bottom
			if(i == 0 || i==y2-y1)
			{
				if(j == 0 || j == x2-x1) rend->set(x1+j,y1+i,0xff,color); // bulbs
				else rend->set(x1+j,y1+i,0xcd,color); // rows
			}
			else {
				if(j == 0 || j == x2-x1) {rend->set(x1+j,y1+i,0xba,color);} // columns
				else {rend->set(x1+j,y1+i,0x00,color);} // inside
			}
		}
	}
}

void GuiManager::draw_box_border(int x1, int y1, int x2, int y2, unsigned char color)
{
	rend->set(x1,y1,0xff,color);
	rend->set(x2,y1,0xff,color);
	rend->set(x1,y2,0xff,color);
	rend->set(x2,y2,0xff,color);
	
	for(int i=1;i<x2-x1;i++)
	{
		rend->set(x1+i,y1,0xcd,color);
		rend->set(x1+i,y2,0xcd,color);
	}
	for(int i=1;i<y2-y1;i++)
	{
		rend->set(x1,y1+i,0xba,color);
		rend->set(x2,y1+i,0xba,color);
	}
}