#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#include "config.h"
#include "gui.h"
#include "netutil.h"

#define DEFAULT_PORT "27015"
class Client
{
public:
	void init();
	~Client();
	
	void has_ticked(); // called after a successful gametick
	
	SOCKET connect_socket;
	bool can_tick; // ticking was allowed by the server

	unsigned int id; // client id
	std::string name; // client name

	std::string client_command;
	std::string server_command;
	
	std::map<std::string, unsigned int> client_map_str_int; // client names
	std::map<unsigned int, std::string> client_map_int_str;
	
	std::vector<std::pair<int, int>> pending_chunks_gen; // pending chunks to generate
};

extern Client client;
extern std::thread client_thread;

void client_wait_for_tick();