#include "localizer.h"

Localizer localizer;

void Localizer::init()
{
	language = config.get_str("language");

	// read the latvian dictionary
	if(language == "latvian")
	{
		std::ifstream file("data/localization/latvian.txt");
		
		while(file.good())
		{
			std::string line;
			std::vector<std::string> vec;
			std::getline(file, line);

			sep_char('|', line, vec); // separate by |
			
			dictionary[vec[0]] = vec[1]; // assign to dictionary
		}
		
		file.close();
	}
}

std::string loc(std::string str)
{
	std::string _out = str;
	if(localizer.language != "english")
	{
		// find the suitable element in the dictionary
		if(localizer.dictionary.count(str) > 0)
			_out = localizer.dictionary.at(str);
	}
	return _out;
}