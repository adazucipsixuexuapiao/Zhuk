#include "renderer.h"

void Renderer::init(SDL_Renderer* _rend, unsigned int _w, unsigned int _h, unsigned int t_w, unsigned int t_h)
{
	rend = _rend;
	tile_w = t_w;
	tile_h = t_h;
	
	w = (int)(_w / t_w);
	h = (int)(_h / t_h);
	
	// load the image
	SDL_Surface* _ascii_img = IMG_Load(POS_IMAGE_PATH);
	ascii_img_pos = SDL_CreateTextureFromSurface(rend, _ascii_img);
	SDL_FreeSurface(_ascii_img);
	_ascii_img = IMG_Load(NEG_IMAGE_PATH);
	ascii_img_neg = SDL_CreateTextureFromSurface(rend, _ascii_img);
	SDL_FreeSurface(_ascii_img);
	
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
	
	// create the symbol and color grid and fill it with zeroes
	symbol_grid = new unsigned char*[h];
	color_grid = new unsigned char*[h];
	
	for(int i=0;i<h;i++)
	{
		symbol_grid[i] = new unsigned char[w];
		color_grid[i] = new unsigned char[w];
		
		// fill with 0
		for(int j=0;j<w;j++)
		{
			symbol_grid[i][j] = 0x00;
			color_grid[i][j] = 0x00;
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
			src.x = tile_w * (symbol_grid[i][j]%16);
			src.y = tile_h * (int)(symbol_grid[i][j] / 16);
			
			dst.x = j*tile_w;
			dst.y = i*tile_h;

			unsigned char col = color_grid[i][j];
			unsigned char fg = (col & 0xf0) >> 4;
			unsigned char bg = (col & 0x0f);
			
			
			SDL_SetTextureColorMod(ascii_img_pos,
				colors[fg].r,
				colors[fg].g,
				colors[fg].b
			);
			
			SDL_SetTextureColorMod(ascii_img_neg,
				colors[bg].r,
				colors[bg].g,
				colors[bg].b
			);
			
			SDL_RenderCopy(rend, ascii_img_pos, &src, &dst);
			SDL_RenderCopy(rend, ascii_img_neg, &src, &dst);
		}
	}
}

void Renderer::set(int x, int y, unsigned char sym, unsigned char color)
{
	if(x >= 0 && x < w && y >= 0 && y < h)
	{
		symbol_grid[y][x] = sym;
		color_grid[y][x] = color;
	}
}

Renderer::~Renderer()
{
	// cleanup
	if(colors != nullptr) delete[] colors;
	if(ascii_img_pos != nullptr) SDL_DestroyTexture(ascii_img_pos);
	if(ascii_img_neg != nullptr) SDL_DestroyTexture(ascii_img_neg);
	if(cursor != nullptr) SDL_DestroyTexture(cursor);
	
	for(int i=0;i<h;i++)
	{
		delete[] symbol_grid[i];
		delete[] color_grid[i];
	}
	delete[] symbol_grid;
	delete[] color_grid;
}