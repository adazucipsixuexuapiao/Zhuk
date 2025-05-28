#include "config.h"

void Configuration::read_config()
{
	std::ifstream file(CONFIG_PATH);
	
	while(file.good())
	{
		std::string line;
		std::vector<std::string> vec;
		std::getline(file, line);

		sep_char(' ', line, vec);
		
		// command
		if(vec[0] == ":")
			data[vec[1]]=vec[2];
	}
	
	file.close();
}

bool Configuration::get_bool(std::string loc){return (data.at(loc)=="1"?true:false);}
int Configuration::get_int(std::string loc){return std::atoi(data.at(loc).c_str());} // dangerous shit
std::string Configuration::get_str(std::string loc){return data.at(loc);}