#include "client.h"

Client client;
std::thread client_thread;

void Client::init()
{
	// initialize client data
	name = config.get_str("client_name");
	// initialize winsocket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(config.get_str("host_ip").c_str(), DEFAULT_PORT, &hints, &result);
	connect_socket = socket(result->ai_family, result->ai_socktype,
	result->ai_protocol);
	
	std::cout << "Connecting to server...\n";
	// connect to the socket
	connect(connect_socket, result->ai_addr, (int)result->ai_addrlen);
	send_command(connect_socket, name); // send out client name
	char buff[1];
	recv(connect_socket, buff, 1, 0);
	id = (unsigned int)buff[0]; // retrieve client id
	
	std::cout << "Connected!\n";
	
	freeaddrinfo(result);
	
	can_tick = false;
}

void client_wait_for_tick()
{
	if(gui.current_state == MENU)
	{
		char buff[4];
		std::cout << "Waiting for server...\n";
		recv(client.connect_socket, buff, 4, 0);
		if(std::string(buff).substr(0,3) == "OK!")
		{
			std::cout << "Can play!\n";
			gui.can_play = true;
		}
	}
	
	if(gui.current_state == WAITING)
	{
		std::cout << "Sending confirmation...\n";
		send(client.connect_socket, "OK!\0", 4, 0);
		gui.current_state = GAME;
	}
	
	if(gui.current_state == GAME)
	{
		char buff[4];
		recv(client.connect_socket, buff, 4, 0);
		// client allowed to send data to server
		if(std::string(buff).substr(0,3) == "OK!")
		{
			if(client.client_command.length() == 0)
				client.client_command = "null_command;";
			send_command(client.connect_socket, client.client_command);
			client.client_command = "";
			client.server_command = recieve_command(client.connect_socket);
		}
		client.can_tick = true;
	}
}

void Client::has_ticked()
{
	// again wait for server allowance
	server_command = ""; // for security's sake
	client_command = "";
	can_tick = false;
}

Client::~Client()
{
	closesocket(connect_socket);
	WSACleanup();
}