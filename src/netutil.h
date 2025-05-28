#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <iostream>

#define DEFAULT_PACKET_SIZE 512
// how many bytes to allocate for number of packets to send
#define COMMAND_BYTES 2

// return substring until null termination
std::string ret_null_term(std::string str);

// recieve a command from a client socket
std::string recieve_command(SOCKET sock);

// formats the command into default sized packets
std::vector<char*> format_command(std::string command);

// sends formatted command to socket
void send_command(SOCKET sock, std::string command);