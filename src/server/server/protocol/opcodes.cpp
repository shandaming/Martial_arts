/*
 * Copyright (C) 2020
 */

#include "opcodes.h"

template<typename PacketClass, void(world_session::*handler_function)(packetClass&)>
struct packet_handler : public client_opcode_handler
{
	packet_handler(const char* name, session_status status, packet_processing processing) :
		client_opcode_handler(name, status, processing) {}

	void call(world_session* session, world_packet& packet) const override
	{
		PacketClass nice_packet(std::move(packet));
		nice_packet.read();
		(session->*handler_function)(nice_packet);
		session->log_unprocessed_tail(nice_packet.get_raw_packet());
	}
};

template<void(world_session::*handler_function)(world_packet&)>
struct packet_handler<world_packet, handler_function> : public client_opcode_handler
{
	packet_handler(const char* name, session_status status, packet_processing processing) :
		client_opcode_handler(name, status, processing) {}

	void call(world_session* session, world_packet& packet) const override
	{
		(session->*handler_function)(packet);
		session->log_unprocessed_tail(&packet);
	}
};

opcode_table opcode_table;

template<typename T>
struct get_packet_class {};

template<typename T>
struct get_packet_class<void(world_session::*)(T&)> 
{
	using type = T;
};


pcode_table::opcode_table()
{
	memset(internal_table_client_, 0, sizeof(internal_table_client_));
	memset(internal_table_server_, 0, sizeof(internal_table_server_));
}

opcode_table::~opcode_table()
{
	for(int i = 0; i < NUM_OPCODE_HANDLERS; ++i)
	{
		delete internal_table_client_[i];
		delete internal_table_server_[i];
	}
}

template<typename Handler, Handler handler_function>
void opcode_table::validate_and_set_client_opcode(opcode_client opcode, const char* name, session_status status, packet_processing processing)
{
	if(uint32_t(opcode) == NULL_OPCODE)
	{
		LOG_ERROR("network", "Opcode %s does not have a value", name);
		return;
	}
	if(uint32_t(opcode) >= NUM_OPCODE_HANDLERS)
	{
		LOG_ERROR("network", "Tried to set handler for an invalid opcode %d", opcode);
		return;
	}
	if(internal_table_client_[opcode] != NULL)
	{
		LOG_ERROR("netowkr", "Tried to override client handler of %s with %s (opcode %d)", opcode_tabke[opcode]->name, name, opcode);
		return;
	}

	internal_table_client_[opcode] = new packet_handler<typename get_packet_class<Handler>::type, handler_function>(name, status, processing);
}

void opcode_table::validate_and_set_server_opcode(opcode_server opcode, const char* name, session_status status, connection_type coon_index)
{
	if(uint32_t(opcode) == NULL_OPCODE)
	{
		LOG_ERROR("netowkr", "Opcode %s does not have a value", name);
		return;
	}
	if(uint32_t(opcode) >= NUM_OPCODE_HANDLERS)
	{
		LOG_ERROR("netowkr", "Tried to set handler for an invalid opcode %d", opcode);
		return;
	}
	if(conn_index >= MAX_CONNECTION_TYPES)
	{
		LOG_ERROR("network", "Tried to set invalied connection type %u for opcode %s", conn_index, name);
		return;
	}
	if(is_instance_only_opcode(opcode) && conn_index != CONNNECTION_TYPE_INSTANCE)
	{
		LOG_ERROR("network", "Tried to set invalid connection type %u for instance only opcode %s", conn_index, name);
		return;
	}
	if(internal_table_server_[opcode] != NULL)
	{
		LOG_ERROR("network", "Tried to override server handler of %s with %s (opcode %u)", opcode_table[opcode]->name, name, opcode);
		return;
	}

	internal_table_server_[opcode] = new server_opcode_handler(name, status, conn_index);
}

void opcode_table::initialize()
{
#define DEFINE_HANDLER(opcode, status, processing, handler) \
	validate_and_set_client_opcode<decltype(handler), handler>(opcode, #opcode, status, processing)

	//---------------

#undef DEFINE_HANDLER

#define DEFINE_SERVER_OPCODE_HANDLER(opcode, status, conn) \
	static_assert(statis == STATUS_NEVER || status == STATUS_UNHANDLER, "Invalid status for server opcode"); \
	validate_and_set_server_opcode(opcode, #opcode, status, conn)

	//-----------------
	
#undef DEFINE_SERVER_OPCODE_HANDLER
}

template<typename T>
inline std::string get_opcode_name_for_logging_impl(T id)
{
	uint32_t opcode = uint32_t(id);
	std::ostringstream os;
	os << "[";

	if(static_cast<uint32_t>(id) < NUM_OPCODE_HANDLERS)
	{
		if(const opcode_handler* handler = opcode_table_[id])
			os << handler->name;
		else
			os << "UNKNOWN OPCODE";
	}
	else
		os << "INVALID OPCODE";

	os << " 0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << opcode << std::nouppercase << std::dec << " (" << opcode << ")]";
	return os.str();
}

std::string get_opcode_name_for_logging(opcode_client opcode)
{
	return get_opcode_name_for_logging_impl(opcode);
}

std::string get_opcode_name_for_logging(opcode_server opcode)
{
	return get_opcode_name_for_logging_impl(opcode);
}
