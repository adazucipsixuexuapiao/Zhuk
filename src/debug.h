// debugger and error handler

#pragma once

#include <SDL2/SDL.h>

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>

#define TIME_START() const auto debug_timer_start{std::chrono::steady_clock::now()};
#define TIME_STOP(msg) {const auto debug_timer_end{std::chrono::steady_clock::now()}; \
                       const std::chrono::duration<double> debug_elapsed_seconds{debug_timer_end - debug_timer_start}; \
					   std::cout << msg << " elapsed seconds:" << debug_elapsed_seconds.count() << std::endl;}

#define DEBUG(msg) {std::cout << msg << std::endl;}

extern bool debug_flipper; // flips with inputs(starts at false)
extern bool debug_use_input; // use input for debugger
void debug_input(SDL_Event e);

#define DEBUG_LOG_PATH "errlog.txt"
void debug_log(std::string _msg); // log error to errlog.txt