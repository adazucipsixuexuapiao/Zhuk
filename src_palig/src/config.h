// reads the config file

#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_PATH "config.txt"

#include <fstream>
#include <iostream>
#include <map>
#include "fileparser.h"

// stores config.txt information
class Configuration
{
public:
	void read_config(); // reads config.txt data and stores it in the std::map
	
	// gets values from the map
	bool get_bool(std::string loc);
	int get_int(std::string loc);
	std::string get_str(std::string loc);
	
	std::map<std::string, std::string> data;
};

#endif