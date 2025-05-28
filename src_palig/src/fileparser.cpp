#include "fileparser.h"

void sep_char(char s, std::string inp, std::vector<std::string>& out)
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