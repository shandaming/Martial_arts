/*
 * Copyright (C) 2018
 */

#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <netinet/in.h>

#include <cstdint>
#include <string>

namespace net
{
	class Inet_address
	{
			friend bool operator==(const Inet_address& l, const Inet_address& r);
			friend bool operator!=(const Inet_address& l, const Inet_address& r);
		public:
			Inet_address();
			Inet_address(const sockaddr_in& addr);
			explicit Inet_address(const std::string& str, uint16_t port);

			Inet_address(const Inet_address& other);
			Inet_address& operator=(const Inet_address& other);

			Inet_address(Inet_address&& other);
			Inet_address& operator=(Inet_address&& other);

			uint16_t to_port() const;

	sa_family_t family() const { return addr_.sin_family; }

			std::string to_string() const;

			//sockaddr get_sockaddr();
			sockaddr_in get_addr() const { return addr_; }

			bool is_loopback() const;

			operator sockaddr_in&() { return addr_; }
		private:
			sockaddr_in addr_;
	};

	bool operator==(const Inet_address& l, const Inet_address& r);
	bool operator!=(const Inet_address& l, const Inet_address& r);

	Inet_address make_address(const std::string& str);
}

#endif
