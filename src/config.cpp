#include "config.h"

Configuration config;

void Configuration::init()
{
	error = CONFIG_ERR_OK; // set to ok for now
	std::ifstream file(CONFIG_PATH);
	
	if(!file.is_open()) // failed to open file
	{
		error = CONFIG_ERR_FILE;
		debug_log("Could not load config.txt!");
		return;
	}
	
	while(file.good())
	{
		std::string line;
		std::vector<std::string> vec;
		std::getline(file, line);

		sep_char(' ', line, vec);
		
		// command
		if(vec[0] == ":")
		{
			// in case there are multiple words after the parameter, combine them
			std::string str;
			for(int i=2;i<vec.size()-1;i++)
			{
				str += vec[i];
				str += " ";
			}
			str += vec[vec.size()-1];
			data[vec[1]]=str;
		}
	}
	
	file.close();
}

e_config_error Configuration::get_error() { return error; }

std::string Configuration::get_str(std::string loc)
{
	error = CONFIG_ERR_OK;
	
	std::string out;
	/*
	try{ // check if there is such a key
		out = data.at(loc);
	}catch(const out_of_range &e) { // no such key
		error = CONFIG_ERR_KEY;
		out = "";
	}*/
	out = data.at(loc);
	
	return out;
}

bool Configuration::get_bool(std::string loc)
{
	error = CONFIG_ERR_OK;
	std::string a = get_str(loc);
	if(error != CONFIG_ERR_OK) return false; // could not find key
	if(a != "1" && a != "0") // invalid value
	{
		error = CONFIG_ERR_TYPE_BOOL;
		return false;
	}
	return (a=="1"?true:false); // everything ok
}

int Configuration::get_int(std::string loc)
{
	error = CONFIG_ERR_OK;
	std::string a = get_str(loc);
	if(error != CONFIG_ERR_OK) return false; // could not find key

	// implement error handling for nonnumber entries

	return std::atoi(a.c_str()); // everything ok
}

float Configuration::get_float(std::string loc)
{
	error = CONFIG_ERR_OK;
	std::string a = get_str(loc);
	if(error != CONFIG_ERR_OK) return false; // could not find key
	
	// implement error handling for nonnumber entries
	
	return std::atof(a.c_str());
}