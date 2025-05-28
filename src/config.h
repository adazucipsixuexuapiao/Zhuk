// reads the config file
#pragma once

#define CONFIG_PATH "config.txt"

#include <fstream>
#include <iostream>
#include <map>

#include "fileparser.h"
#include "debug.h"

// configuration error types
enum e_config_error
{
	CONFIG_ERR_OK, // everything was fine
	// requested invalid type
	CONFIG_ERR_TYPE_BOOL,
	CONFIG_ERR_TYPE_INT,
	CONFIG_ERR_TYPE_FLOAT,
	// no such entry/key in map
	CONFIG_ERR_KEY,
	// could not load configuration file
	CONFIG_ERR_FILE
};

// stores config.txt information
class Configuration
{
public:
	void init(); // reads config.txt data and stores it in the std::map
	
	// gets values from the map
	bool get_bool(std::string loc);
	int get_int(std::string loc);
	std::string get_str(std::string loc);
	float get_float(std::string loc);
	
	std::map<std::string, std::string> data;

	e_config_error error; // stores the result of the last operation
	e_config_error get_error();
};

// global configuration manager
extern Configuration config;