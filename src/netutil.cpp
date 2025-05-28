#include "netutil.h"

std::string ret_null_term(char* buff, int header)
{
	std::string out = "";
	
	unsigned int start = 0;
	if(header == 1) start = 2;
	
	for(int i=start;true;i++)
	{
		if(buff[i] != '\0')
			out += buff[i];
		else
			break;
	}
	return out;
}

// recieve a command from a client socket
std::string recieve_command(SOCKET sock)
{
	std::string out = "";
	
	char buff[DEFAULT_PACKET_SIZE];
	recv(sock, buff, DEFAULT_PACKET_SIZE, 0);
	
	// (MSB first)
	unsigned int num_packets = (unsigned int)buff[0]*256 + (unsigned int)buff[1];
	
	// add everything except the header
	out += ret_null_term(buff, 1);
	
	// read subsequent packets
	for(int i=0;i<num_packets-1;i++)
	{
		recv(sock, buff, DEFAULT_PACKET_SIZE, 0);
		out += ret_null_term(buff, 0);
	}
	return out;
}

// formats the command into default sized packets
std::vector<char*> format_command(std::string command)
{
	std::vector<char*> form;
	
	unsigned int len = command.length() + 1 + COMMAND_BYTES; // including null termination and header
	unsigned int packet_num = len / DEFAULT_PACKET_SIZE + (len % DEFAULT_PACKET_SIZE > 0 ? 1 : 0);
	
	// allocate buffers
	for(int i=0;i<packet_num;i++)
	{
		char* buff = new char[DEFAULT_PACKET_SIZE];
		form.push_back(buff);
	}
	
	// add header to first packet(MSB goes first)
	form[0][0] = packet_num / 256;
	form[0][1] = packet_num % 256;
	
	// start adding the commands to the buffers
	int buff_index = 2;
	int form_index = 0;
	for(int i=0;i<command.length();i++)
	{
		form[form_index][buff_index] = command[i];
		buff_index ++;
		// reached end of the buffer, go to the next one
		if(buff_index >= DEFAULT_PACKET_SIZE)
		{
			buff_index = 0;
			form_index ++;
		}
	}
	// add null terminator
	form[form_index][buff_index] = '\0';
	
	return form;
}

void send_command(SOCKET sock, std::string command)
{
	std::vector<char*> comm = format_command(command);
	for(int i=0;i<comm.size();i++)
		send(sock, comm[i], DEFAULT_PACKET_SIZE, 0);
	
	// deallocate formatted command from memory
	for(int i=0;i<comm.size();i++)
		delete[] comm[i];
}