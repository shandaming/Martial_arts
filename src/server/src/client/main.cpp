/*
 * Copyright (C) 2018
 */

#include <iostream>
#include <string>

#include "../inet_address.h"
#include "../socket.h"

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		std::cout << "Must 3 argc.";
		return 1;
	}

	net::Inet_address address(argv[0], atoi(argv[2]));
	Socket socket(create_tcp_socket(address.family()));
	socket.bind(address);
	connect_server(address);

	std::string msg;
	while(std::cin >> msg)
	{
		ssize_t size = write(socket.get_fd(), msg.c_str(), msg.size());
	}
}
