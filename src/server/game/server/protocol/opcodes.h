/*
 * Copyright (C) 2020
 */

#ifndef PR_OP_CODES_H
#define PR_OP_CODES_H

enum connection_type : int8_t
{
	CONNECTION_TYPE_REALM		= 0,
	CONNECTION_TYPE_INSTANCE	= 1,
	MAX_CONNECTION_TYPES,

	CONNECTION_TYPE_DEFAULT		= -1
};

enum opcode_misc : uint16_t
{
	MAX_OPCODE					= 0x3fff,
	NUM_OPCODE_HANDLERS			= (MAX_OPCODE + 1),
	UNKNOWN_OPCODE				= 0xffff,
	NULL_OPCODE					= 0xba00
};

enum opcode_client : uint16_t
{
	CMSG_AUTH_CONTINUED_SESSION		= 0x3766,
	CMSG_AUTH_SESSION				= 0x3765,
	CMSG_CONNECT_TO_FAILED			= 0x35D4,
	CMSG_ENABLE_ENCRYPTION_ACK		= 0x3767,
	CMSG_ENABLE_NAGLE				= 0x376B,
	CMSG_KEEP_ALIVE					= 0x3680,
	CMSG_LOG_DISCONNECT				= 0x3769,

	CMSG_PING						= 0x3768
};

enum opcode_server : uint16_t
{
	SMSG_AUTH_CHALLENGE				= 0x3048,
	SMSG_AUTH_RESPONSE				= 0x256D,
	SMSG_ENABLE_ENCRYPTION			= 0x3049,
	SMSG_QUEST_GIVER_STATUS			= 0x2A9A,
	SMSG_RESUME_COMMS				= 0x304B,
	SMSG_WAIT_QUEUE_FINISH			= 0x256F,
	SMSG_WAIT_QUEUE_UPDATE			= 0x256E,
	SMSG_PONG						= 0x304E,

	SMSG_ACCOUNT_HEIRLOOM_UPDATE	= 0xBADD,
	SMSG_COMPRESSED_PACKET			= 0x3052,
	SMSG_MULTIPLE_PACKETS			= 0x3051,
};

inline bool is_instance_only_opcode(uint32_t opcode)
{
	switch(opcode)
	{
		case SMSG_QUEST_GIVER_STATUS:
			return true;
		default:
			return false;
	}
}

enum session_status
{
	STATUS_AUTHED = 0,
	STATUS_LOGGEDIN,
	STATUS_TRANSFER,
	STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT,
	STATUS_NEVER,
	STATUS_UNHANDLED
};

enum packet_processing
{
	PROCESS_INPLACE = 0,
	PROCESS_THREADUNSAFE,
	PROCESS_THREADSAFE
};

struct opcode_handler
{
	opcode_handler(const char* name_, session_status status_) : name(name_), status(status_) {}
	virtual ~opcode_handler() {}

	const char* name;
	session_status status;
};

struct client_opcode_handler : public opcode_handler
{
	client_opcode_handler(const char* name, session_status status, packet_processing processing) :
		opcode_handler(name, status), 
		processing_place(processing) {}

	virtual void call(world_session* session, world_packet& packet) const = 0;

	packet_processing processing_place;
};

struct server_opcode_handler : public opcode_handler
{
	server_opcode_handler(const char* name, session_status status, connection_type conn_index) :
		opcode_handler(name, status), 
		connection_index(conn_index) {}

	connection_type connection_index;
};

class opcode_table
{
public:
	opcode_table();
	~opcode_table();

	opcode_table(const opcode_table&) = delete;
	opcode_table& operator=(const opcode_table&) = delete;

	void initialize();

	const client_opcode_handler* operator[](opcode_client index) const
	{
		return internal_table_client_[index];
	}

	const server_opcode_handler* operator[](opcode_server index) const
	{
		return internal_table_server_[index];
	}
private:
	template<typename Handler, Handler handler_function>
	void validate_and_set_client_opcode(opcode_client opcode, const char* name, session_status status, packet_processing processing);

	void validate_and_set_server_opcode(opcode_server opcode, const char* name, session_status status, connection_type conn_index);

	client_opcode_handler* internal_table_client_[NUM_OPCODE_HANDLERS];
	server_opcode_handler* internal_table_server_[NUM_OPCODE_HANDLERS];
};

extern opcode_table opcode_table;

std::string get_opcode_name_for_logging(opcode_client opcode);
std::string get_opcode_name_for_logging(opcode_server opcode);

#endif
