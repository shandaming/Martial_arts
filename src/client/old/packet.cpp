/*
 * Copyright (C) 2018
 */

#include "packet.h"

Packet::Packet(uint8_t* data) : packet_data_(data) {}

void Packet::assign(uint8_t* data) { packet_data_ = data; }

void Packet::assign_body(uint16_t packet_type, uint8_t* body, int body_len)
{
	set_protocol_version(PACKET_VERSION);
	set_packet_type(packet_type);
	set_data_len(0);

	if(body_len > 0)
	{
		set_data_len(body_len);
		memcpy(get_data(), body, body_len);
	}
	set_packet_end();
}
