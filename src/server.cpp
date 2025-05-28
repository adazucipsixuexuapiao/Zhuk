#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <windows.h>

#include <vector>
#include <chrono>
#include <string>
#include <thread>

#include "config.h"
#include "netutil.h"

#define DEFAULT_PORT "27015"

int init(SOCKET* listen_socket, const char* localaddr)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	
	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	// start winsocket
	WSADATA wsa_data;
	int err = WSAStartup(MAKEWORD(2,2), &wsa_data);
	
	// Resolve the local address and port to be used by the server
	getaddrinfo(localaddr, DEFAULT_PORT, &hints, &result);
	
	// create the listening socket
	*listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	
	// bind the TCP listening socket
    err = bind(*listen_socket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	
	// no error
	return 0;
}

void display_def_msg(int conn_size)
{
	// display information message
	std::cout << "Server opened at IP:" << config.get_str("server_ip") << "\n";
	std::cout << "Port:" << DEFAULT_PORT << "\n";
	std::cout << "Connected:" << conn_size << "\n";
}

void sendall_clients(std::vector<SOCKET>& sockets, const char* buff, unsigned int buflen)
{
	for(int i=0;i<sockets.size();i++)
		send(sockets[i], buff, buflen, 0);
}

int main(int argc, char** argv)
{
	config.read_config();
	
	// initialize the listening socket
	SOCKET listen_socket;
	init(&listen_socket, config.get_str("server_ip").c_str());
	
	bool preparing = false;
	bool playing = false;
	
	// all of the connected clients
	std::vector<SOCKET> client_sockets;
	std::map<std::string, unsigned int> client_map_s_int;
	std::map<unsigned int , std::string> client_map_int_s;
	
	// world seed
	unsigned int world_seed = std::time(0);
	
	// options loop
	while(true)
	{
		system("cls");
		
		display_def_msg(client_sockets.size());
		
		std::cout << "1) Wait for a connection\n";
		std::cout << "2) Prepare to play\n";
		std::cout << "3) Exit\n";
		
		std::string inp;
		std::cin >> inp;
		if(inp == "1")
		{
			std::cout << "Waiting for a connection...\n";
			listen(listen_socket, 2);
			SOCKET client = accept(listen_socket, NULL, NULL);
			client_sockets.push_back(client);
			// recieve client name
			std::string name = recieve_command(client);
			client_map_s_int[name] = client_sockets.size() - 1; // assign id to name
			client_map_int_s[client_sockets.size() - 1] = name; // assign name to id
			// send client id back
			char buff[1];
			buff[0] = (char)client_sockets.size()-1; // id packet
			send(client, buff, 1, 0);
		}
		if(inp == "2") {preparing = true; break;}
		if(inp == "3") break;
	}
	
	system("cls");
	display_def_msg(client_sockets.size());
	
	if(preparing)
	{
		// clients get permission to start playing
		sendall_clients(client_sockets, "OK!\0", 4);
		
		for(int i=0;i<client_sockets.size();i++)
		{
			char buff[4];
			std::cout << "Reading confirmation...\n";
			recv(client_sockets[i], buff, 4, 0);
		}
		playing = true;
	}
	
	if(playing)
	{
		bool running = true;
		
		// first tick
		sendall_clients(client_sockets, "OK!\0", 4);
		// get commands from all the clients
		std::string command = "";
		for(int i=0;i<client_sockets.size();i++)
			recieve_command(client_sockets[i]);
		// send first tick command
		command += "seed " + std::to_string(world_seed) + ";";
		command += "init_worldgen;";
		command += "chnk -41 -41;";
		command += "chnk 40 40;";
		command += "build 0 3 -1296 -1296 0;";
		command += "build 1 3 1296 1296 0;";
		command += "camera_center 0 -1296 -1296;";
		command += "camera_center 1 1296 1296;";
		for(int i=0;i<client_sockets.size();i++) // give all clients their names
			command += "client " + std::to_string(i) + " " + client_map_int_s[i] + ";";
		
		for(int i=0;i<client_sockets.size();i++)
			send_command(client_sockets[i], command);
		
		// ticking loop
		while(running)
		{
			// sleep for 30ms
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			
			sendall_clients(client_sockets, "OK!\0", 4);
			
			// get commands from all the clients
			command = "";
			for(int i=0;i<client_sockets.size();i++)
			{
				command += recieve_command(client_sockets[i]);
			}
			
			// echo the combined command back to the clients
			for(int i=0;i<client_sockets.size();i++)
			{
				send_command(client_sockets[i], command);
			}
		}
	}
	
	// cleanup winsocket
	closesocket(listen_socket);
	for(int i=0;i<client_sockets.size();i++)
		closesocket(client_sockets[i]);
	WSACleanup();
	return 0;
}