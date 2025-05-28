#include "debug.h"

bool debug_flipper = false;
bool debug_use_input = false;

void debug_input(SDL_Event e)
{
	if(debug_use_input == false) return;
	if(e.type == SDL_KEYDOWN)
	{
		switch(e.key.keysym.sym)
		{
			case SDLK_F1:
				debug_flipper = !debug_flipper; // swap the debug flipper
				std::cout << "Debug flipper state:" << (debug_flipper?"true":"false") << std::endl;
			break;
		}
	}
}

void debug_log(std::string _msg)
{
	std::ofstream file(DEBUG_LOG_PATH, std::ios_base::app);
	std::string msg = _msg + "\n";
	file << msg;
	file.close();
}