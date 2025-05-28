#pragma once

#include <string>
#include <map>
#include <fstream>

#include "config.h"

class Localizer
{
public:
	void init();
	std::string language;
	std::map<std::string, std::string> dictionary;
};
extern Localizer localizer;

// returns localized string
std::string loc(std::string str);