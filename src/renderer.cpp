#include "renderer.h"

Renderer rend;

Uint32 Renderer::getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
		case 1:
			return *p;
			break;

		case 2:
			return *(Uint16 *)p;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
				break;

		case 4:
			return *(Uint32 *)p;
			break;

		default:
			return 0;       /* shouldn't happen, but avoids warnings */
	}
}

void Renderer::init(SDL_Renderer* _rend, unsigned int _w, unsigned int _h, unsigned int t_w, unsigned int t_h)
{
	rend = _rend;
	tile_w = t_w;
	tile_h = t_h;
	
	sw = _w; sh = _h; size = sw*sh;
	
	w = (int)(_w / t_w);
	h = (int)(_h / t_h);
	
	// make the screen
	screen = SDL_CreateTexture(_rend, SDL_PIXELFORMAT_ARGB8888,
	SDL_TEXTUREACCESS_STREAMING, sw, sh);
	pixels = new uint32_t[sw*sh];
	
	// set it all to 0 by default
	for(int i=0;i<size;i++)
		pixels[i] = 0;
	
	SDL_UpdateTexture(screen, NULL, pixels, w * 4);
	
	// load the tileset image
	ascii_img = IMG_Load(IMAGE_PATH);
	
	// fill up pixel grid for ascii tileset
	ascii_pixels = new char[256*256];
	for(int i=0;i<256;i++)
	{
		for(int j=0;j<256;j++)
		{
			Uint32 pix = getpixel(ascii_img, j, i);
			if(pix == 4294967295) // 16777215
				ascii_pixels[j+i*256] = 1;
			else ascii_pixels[j+i*256] = 0;
		}
	}
	
	SDL_Surface* cur = IMG_Load("data/image/cursor.png");
	cursor = SDL_CreateTextureFromSurface(rend, cur);
	SDL_FreeSurface(cur);
	
	// initialize all the colors
	colors = new SDL_Color[0xf+1];
	std::ifstream color_file(COLOR_PATH);
	
	int color_index = 0;
	while(color_file.good())
	{
		// read the file line by line, seperate the spaces and use the info to fill out the color array
		std::string line;
		std::getline(color_file, line);
		std::vector<std::string> vec;
		sep_char(' ', line, vec);
	
		SDL_Color col;
		col.r = std::atoi(vec[0].c_str());
		col.g = std::atoi(vec[1].c_str());
		col.b = std::atoi(vec[2].c_str());

		if(color_index <= 0xf)
			*(colors+color_index) = col;
		color_index ++;
	}
	
	color_file.close();
	
	// fill out colors_dat[]
	for(int i=0;i<16;i++)
	{
		colors_dat[i] = 0;
		colors_dat[i] |= 0xff000000;
		colors_dat[i] |= colors[i].r << 4*4;
		colors_dat[i] |= colors[i].g << 2*4;
		colors_dat[i] |= colors[i].b;
	}
	
	// create the symbol and color grid and fill it with zeroes
	symbol_grid = new unsigned char*[h];
	color_grid = new unsigned char*[h];
	prev_symbol_grid = new unsigned char*[h];
	prev_color_grid = new unsigned char*[h];
	
	for(int i=0;i<h;i++)
	{
		symbol_grid[i] = new unsigned char[w];
		color_grid[i] = new unsigned char[w];
		prev_symbol_grid[i] = new unsigned char[w];
		prev_color_grid[i] = new unsigned char[w];
		
		// fill with 0
		for(int j=0;j<w;j++)
		{
			symbol_grid[i][j] = 0x00;
			color_grid[i][j] = 0x00;
			prev_symbol_grid[i][j] = 0x00;
			prev_color_grid[i][j] = 0x00;
		}
	}
}

void Renderer::update()
{
	SDL_RenderPresent(rend);
}

// renders screen(and left)
void Renderer::draw_symbols()
{
	SDL_Rect src, dst;
	dst.w = 16; dst.h = 16; src.w = 16; src.h = 16;
	
	// loops through the grid
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{	
			// check if this character has updated
			if(prev_symbol_grid[i][j] != symbol_grid[i][j] || prev_color_grid[i][j] != color_grid[i][j])
			{
				// copy pixels onto pixels* array
				src.x = tile_w * (symbol_grid[i][j]%16);
				src.y = tile_h * (int)(symbol_grid[i][j] / 16);
				
				dst.x = j*tile_w;
				dst.y = i*tile_h;

				unsigned char col = color_grid[i][j];
				unsigned char fg = (col & 0xf0) >> 4;
				unsigned char bg = (col & 0x0f);
				
				for(int y=0;y<16;y++)
				{
					for(int x=0;x<16;x++)
					{
						unsigned int loc = j*tile_w+x+(y+i*tile_h)*sw;
						if(ascii_pixels[(src.x+x)+(src.y+y)*256] == 1)
							pixels[loc] = colors_dat[fg];
						else pixels[loc] = colors_dat[bg];
					}
				}
				
				// update previous character array
				prev_symbol_grid[i][j] = symbol_grid[i][j];
				prev_color_grid[i][j] = color_grid[i][j];
			}
		}
	}
	
	SDL_UpdateTexture(screen, NULL, pixels, sw*4); 
	SDL_RenderCopy(rend, screen, NULL, NULL);
}

// sort of like set but it simply draws it instead of changing the grid
void Renderer::draw_char(int x, int y,
						unsigned char sym, unsigned char color)
{

}

void Renderer::clear(unsigned char color)
{
	for(int i=0;i<h;i++) // set the whole canvas to color and clear
		for(int j=0;j<w;j++)
		{
			symbol_grid[i][j] = 0x00;
			color_grid[i][j] = color;
		}
}

void Renderer::set(int x, int y, unsigned char sym, unsigned char color)
{
	if(x >= 0 && x < w && y >= 0 && y < h) // check if in bounds
	{
		symbol_grid[y][x] = sym;
		color_grid[y][x] = color;
	}
}

Renderer::~Renderer()
{
	#define DARR(a) if(a != nullptr) delete[] a
	// cleanup
	DARR(pixels);
	DARR(colors);
	DARR(ascii_pixels);
	if(screen != nullptr) SDL_DestroyTexture(screen);
	if(ascii_img != nullptr) SDL_FreeSurface(ascii_img);
	if(cursor != nullptr) SDL_DestroyTexture(cursor);
	
	for(int i=0;i<h;i++)
	{
		DARR(symbol_grid[i]);
		DARR(color_grid[i]);
		DARR(prev_symbol_grid[i]);
		DARR(prev_color_grid[i]);
	}
	DARR(symbol_grid);
	DARR(color_grid);
	DARR(prev_symbol_grid);
	DARR(prev_color_grid);
	#undef DARR
}

/* cool rendering functions */
void Renderer::draw_text(int x, int y, std::string text, unsigned char color)
{
	int _y = y;
	int _x = x;
	for(int i=0;i<text.length();i++)
	{
		unsigned char letter = text[i];
		if(letter == '\n') {_x = x; _y++; continue;}
		set(_x, _y, letter, color);
		_x++;
	}
}

void Renderer::draw_text_w(int x, int y, int w, std::string text, unsigned char color)
{
	int _y = y;
	int _x = x;
	int len = (w<text.length()?w:text.length());
	if(w == -1) len = text.length();
	for(int i=0;i<len;i++)
	{
		unsigned char letter = text[i];
		if(letter == '\n') {_x = x; _y++; continue;}
		set(_x, _y, letter, color);
		_x++;
	}
}

void Renderer::draw_box_full(int x1, int y1, int x2, int y2, unsigned char color)
{
	draw_rect(x1,y1,x2,y2,color, 0x00);
	draw_box_border(x1,y1,x2,y2,color);
}

void Renderer::draw_rect(int x1, int y1, int x2, int y2, unsigned char color, unsigned char sym)
{
	for(int i=0;i<=y2-y1;i++)
		for(int j=0;j<=x2-x1;j++)
			set(x1+j,y1+i,sym,color);
}

void Renderer::draw_box_border(int x1, int y1, int x2, int y2, unsigned char color)
{	
	draw_box_border_cus(x1, y1, x2, y2, color, 0xc4, 0xb3, 0xda, 0xbf, 0xd9, 0xc0);
}

void Renderer::draw_box_border_cus(int x1, int y1, int x2, int y2, unsigned char color,
		unsigned char hor, unsigned char ver, unsigned char tl, unsigned char tr, unsigned char br, unsigned char bl)
{
	// corners
	set(x1,y1,tl,color);
	set(x2,y1,tr,color);
	set(x1,y2,bl,color);
	set(x2,y2,br,color);
	
	// lines
	for(int i=1;i<x2-x1;i++)
	{
		set(x1+i,y1,hor,color);
		set(x1+i,y2,hor,color);
	}
	for(int i=1;i<y2-y1;i++)
	{
		set(x1,y1+i,ver,color);
		set(x2,y1+i,ver,color);
	}
}

void Renderer::draw_box_full_cus(int x1, int y1, int x2, int y2, unsigned char border_color,
	unsigned char color, unsigned char fill, unsigned char hor, unsigned char ver, unsigned char tl,
	unsigned char tr, unsigned char br, unsigned char bl)
{
	draw_rect(x1, y1, x2, y2, color, fill);
	draw_box_border_cus(x1, y1, x2, y2, border_color, hor, ver, tl, tr, br, bl);
}