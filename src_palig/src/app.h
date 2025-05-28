#ifndef APP_H
#define APP_H

#include "window.h"
#include "renderer.h"
#include "config.h"
#include "gui.h"

#include "random.h"

#include <iostream>

class Application
{
public:
	Application();
	~Application();
	
	void update(); // main loop
private:
	Window win;
	Renderer rend;
	GuiManager gui;
	//AudioManager aud;
	Configuration config;
	bool isRunning;
};

#endif