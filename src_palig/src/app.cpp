#include "app.h"

Application::Application()
{	
	SDL_Init(SDL_INIT_EVERYTHING);
	isRunning = true;
	
	config.read_config();
	win.init(config);
	rend.init(win.rend, win.W, win.H, 16, 16);
	gui.init(&rend);
	
	SDL_ShowCursor(SDL_DISABLE);
}

Application::~Application()
{
	SDL_Quit();
}

void Application::update()
{
	int mouse_x, mouse_y; // (in symbol coordinates)
	
	while(isRunning)
	{
		std::vector<SDL_Event> events;
		// input
		SDL_Event evnt;
		while(SDL_PollEvent(&evnt))
		{
			events.push_back(evnt);
			if(evnt.type == SDL_QUIT) isRunning = false;
		}
		
		// stuff below GUI here:
		for(int i=0;i<rend.h;i++)
			for(int j=0;j<rend.w;j++)
			{
				rend.set(j,i,0x00,0x00);
			}
		
		
		gui.draw_menu_gui(events);
		
		rend.draw_symbols();
		
		gui.draw_cursor();
		
		rend.update();
		
	}
}