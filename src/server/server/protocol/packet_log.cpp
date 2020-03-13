/*
 * Copyright (C) 2020
 */

#include "packet_log.h"

#pragma pack(push, 1)

struct log_header
{
	char signature[3];
	uint16_t format_version;
	uint8_t sniffer_id;
	uint32_t build;
	char locale[4];
	uint8_t session_key[40];
	uint32_t sniff_start_unixtime;
	uint32_t sniff_start_ticks;
	uint32_t optional_data_size;
};

struct packet_header
{
	struct optional_data
	{
		uint8_t socket_ip_bytes[16];
		uint32_t socket_port;
	};

	uint32_t direction;
	uint32_t connection_id;
	uint32_t arrival_ticks;
	uint32_t optional_data_size;
	uint32_t length;
	optional_data optional_data;
	uint32_t opcode;
};

#pragma pack(pop)

packet_log::packet_log() : file_(NULL)
{
	std::call_once(initialize_flag_, &packet_log::initialize, this);
}

packet_log::~packet_log()
{
	if(file_)
		fclose(file_);
	file_ = NULL;
}

packet_log* packet_log::instance()
{
	static packet_log instance;
	return &instance;
}

void packet_log::initialize()
{
	std::string log_dir = CONFIG_MGR->get_string_default("log_dir", "");
	if(!log_dir.empty())
	{
		if((!log_dir.at(log_dir.length() - 1) != '/') && (log_dir.at(log_dir.length() - 1) != '\\'))
			log_dir.push_back('/');
	}

	std::string logname = CONFIG_MGR->get_string_default("packet_log_file", "");
	if(!logname.empty())
	{
		file_ = fopen((log_dir + logname).c_str(), "wb");
		if(can_log_packet())
		{
			log_header header;
			header.signature[0] = 'P'; 
			header.signature[1] = 'K'; 
			header.signature[2] = 'T';
			header.format_version = 0x0301;
			header.sniffer_id = 'T';
			header.build = realm.build;
			header.locale[0] = 'e'; 
			header.locale[1] = 'n'; 
			header.locale[2] = 'U'; 
			header.locale[3] = 'S';
			std::memset(header.session_key, 0, sizeof(header.session_key));
			header.sniff_start_unixtime = time(NULL);
			header.sniff_start_ticks = get_ms_time();
			header.optional_data_size = 0;

			fwrite(&header, sizeof(header), 1, file_);
		}
	}
}

void packet_log::log_packet(const world_packet& packet, direction direction, const address addr, uint16_t port, connection_type connection_type)
{
	std::lock_guard<std::mutex> lock(log_packet_lock_);

	packet_header header;
	header.direction = direction == CLIENT_TO_SERVER ? 0x47534d43 : 0x47534d53;
	header.connection_id = connection_type;
	header.arrival_ticks = get_ms_time();

	header.optional_data_size = sizeof(header.optional_data);
	memset(header.optional_data.socket_ip_bytes, 0, sizeof(header.optional_data.socket_ip_bytes));
	if (addr.is_v4())
	{
		auto bytes = addr.to_v4().to_bytes();
		memcpy(header.optional_data.socket_ip_bytes, bytes.data(), bytes.size());
	}
	else if(addr.is_v6())
	{
		auto bytes = addr.to_v6().to_bytes();
		memcpy(header.optional_data.socket_ip_bytes, bytes.data(), bytes.size());
	}

	header.optional_data.socket_port = port;
	std::size_t size = packet.size();
	if (direction == CLIENT_TO_SERVER)
		size -= 2;

	header.Length = size + sizeof(header.opcode);
	header.opcode = packet.get_opcode();

	fwrite(&header, sizeof(header), 1, file_);
	if (size)
	{
		const uint8_t* data = packet.contents();
		if (direction == CLIENT_TO_SERVER)
			data += 2;
		fwrite(data, 1, size, file_);
	}

	fflush(file_);
}
