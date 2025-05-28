#include "app.h"

Application::Application()
{	
	// initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	isRunning = true;
	
	config.init();
	localizer.init();
	client.init();
	
	win.init();
	rend.init(win.rend, win.W, win.H, 16, 16);
	
	gui.init();
	gamelogic.init();	
	audio.init();
	
	SDL_ShowCursor(SDL_DISABLE);
}

Application::~Application()
{
	SDL_Quit();
}

void Application::update()
{
	fps_timer.init(60);
	
	gui.init_menu_gui(); // initialize the menu(buttons)

	client_thread = std::thread(client_wait_for_tick);

	//audio.playm("menu.wav"); // play music
	while(isRunning)
	{
		fps_timer.start();

		// input
		SDL_Event evnt;
		std::vector<SDL_Event> events;
		while(SDL_PollEvent(&evnt))
		{
			events.push_back(evnt);
			if(evnt.type == SDL_QUIT) isRunning = false;
		}
		
		// stuff below GUI here:
		rend.clear(0x00);
		
		if(gui.current_state == GAME)
		{
			gui.update(events); // update gui before game
			if(client.can_tick)
			{
				gamelogic.tick(client.server_command);
				client.has_ticked();
				client_thread.join(); // restart the client thread
				client_thread = std::thread(client_wait_for_tick); 
			}
			gamelogic.update(events);
			gamelogic.draw();
		}
		
		// GUI
		gui.draw_gui();
		if(gui.current_state != GAME)
			gui.update(events);
		
		if(gui.current_state == QUIT) isRunning = false;
		
		// everything else
		rend.draw_symbols();
		gui.draw_cursor();
		
		rend.update();
		
		fps_timer.stop();
		
		gui.clicked_panel = false;
	}
}