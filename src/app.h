#pragma once

#include "window.h"
#include "renderer.h"
#include "config.h"
#include "gui.h"
#include "timer.h"
#include "random.h"
#include "gamelogic.h"
#include "localizer.h"
#include "audio.h"
#include "client.h"
#include "debug.h"

#include <iostream>

class Application
{
public:
	Application();
	~Application();
	
	void update(); // main loop
private:
	Window win;
	
	//AudioManager aud;
	bool isRunning;
};