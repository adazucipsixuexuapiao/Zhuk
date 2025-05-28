// file parser, separates the spaces and stuff like that

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <vector>
#include <fstream>

void sep_char(char s, std::string& inp, std::vector<std::string>& out);

// loads and parses game .dat file (mega lol vector vector!!!!!!!!)
void load_parse_datfile(std::string _path, std::vector<std::vector<std::string>>& parsed);

unsigned int hex_conv_digit(char dig);

// convert 2 digit hex string into integer
unsigned int hex_conv(std::string str);

#endif