#pragma once

#define UNIT_DATA_PATH "data/data/unit.data"
class Unit
{
public:
	int x, y; // position
	unsigned int unit_id;
	unsigned int client_id; // who it belongs to
};