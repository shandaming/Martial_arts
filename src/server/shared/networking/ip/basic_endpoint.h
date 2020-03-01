/*
 * Copyright (C) 2020
 */

#ifndef IP_BASIC_ENDPOINT_H
#define IP_BASIC_ENDPOINT_H

#include "endpoint.h"

template <typename InternetProtocol>
class basic_endpoint
{
	//typedef sockaddr socket_addr_type;
public:
	typedef InternetProtocol protocol_type;
	typedef socket_addr_type data_type;

	basic_endpoint() : impl_() {}

	basic_endpoint(const InternetProtocol& internet_protocol, uint16_t port_num) : 
		impl_(internet_protocol.family(), port_num) {}

	basic_endpoint(const boost::asio::ip::address& addr, uint16_t port_num) :
		impl_(addr, port_num) {}

	basic_endpoint(const basic_endpoint& other) : impl_(other.impl_) {}

	basic_endpoint(basic_endpoint&& other) : impl_(other.impl_) {}

	basic_endpoint& operator=(const basic_endpoint& other) 
	{
		impl_ = other.impl_;
		return *this;
	}

	basic_endpoint& operator=(basic_endpoint&& other) 
	{
		impl_ = other.impl_;
		return *this;
	}

	protocol_type protocol() const 
	{
		if (impl_.is_v4())
			return InternetProtocol::v4();
		return InternetProtocol::v6();
	}

	data_type* data() { return impl_.data(); }

	const data_type* data() const { return impl_.data(); }

	std::size_t size() const { return impl_.size(); }

	void resize(std::size_t new_size) { impl_.resize(new_size); }

	std::size_t capacity() const { return impl_.capacity(); }

	uint16_t port() const { return impl_.port(); }

	void port(uint16_t port_num) { impl_.port(port_num); }

	address address() const { return impl_.address(); }

	void address(const address& addr) { impl_.address(addr); }

	friend bool operator==(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) 
	{
		return e1.impl_ == e2.impl_;
	}

	friend bool operator!=(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) 
	{
		return !(e1 == e2);
	}

	friend bool operator<(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) 
	{
		return e1.impl_ < e2.impl_;
	}

	friend bool operator>(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) 
	{
		return e2.impl_ < e1.impl_;
	}

	friend bool operator<=(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) 
	{
		return !(e2 < e1);
	}

	friend bool operator>=(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) 
	{
		return !(e1 < e2);
	}
private:
	endpoint impl_;
};

#endif
