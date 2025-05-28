#include "fileparser.h"

void sep_char(char s, std::string& inp, std::vector<std::string>& out)
{
	int begin_index = 0;
	if(inp.length() == 0)
	{
		out.push_back(" ");
		return;
	}
	for(int i=0;i<inp.length();i++)
	{
		// if reached seperating char or end of string, put stuff into vector
		if(inp[i] == s)
		{
			out.push_back(inp.substr(begin_index, i-begin_index));
			begin_index = i+1;
			continue;
		}
		if(i == inp.length()-1)
		{
			out.push_back(inp.substr(begin_index, i-begin_index+1));
			break;
		}
	}
}

void load_parse_datfile(std::string _path, std::vector<std::vector<std::string>>& parsed)
{
	// load structure.dat information
	std::ifstream file(_path);
	
	std::string data_string = "";
	while(file.good())
	{
		std::string line;
		std::getline(file, line);
		data_string += line;
		data_string += "\n";
	}
	file.close();

	// parse the read data
	std::vector<std::string> s_vec; // one structure per string
	sep_char(';', data_string, s_vec);
	
	s_vec.pop_back(); // remove element after last semicolon
	
	// parse the lines
	for(int i=0;i<s_vec.size();i++)
	{		
		std::vector<std::string> lines;
		sep_char('\n', s_vec[i], lines);
		parsed.push_back(lines);
	}
}

unsigned int hex_conv_dig(char dig)
{
	if(dig >= '0' && dig <= '9')
		return (unsigned int)(dig - '0');
	if(dig >= 'A' && dig <= 'F')
		return (unsigned int)(dig - 'A' + 10);
	return 0;
}

unsigned int hex_conv(std::string str)
{
	unsigned int ret = 0;
	ret += hex_conv_dig(str[1]);
	ret += 16*hex_conv_dig(str[0]);
	return ret;
}