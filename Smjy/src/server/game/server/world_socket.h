/*
 * Copyright (C) 2020
 */

#ifndef S_WORLD_TCP_SOCKET_H
#define S_WORLD_TCP_SOCKET_H

#include "tcp_socket.h"
#include "big_number.h"
#include "world_packet_crypt.h"
//#include "database_env_fwd.h"
#include "query_callback_processor.h"
#include "mpsc_queue.h"

namespace world_packets
{
class server_packet;
namespace auth
{
class auth_session;
class auth_continued_session;
class connect_to_failed;
class ping;
}
}

#pragma pack(push, 1)
struct packet_header
{
	uint32_t size;
	uint8_t tag[12];

	bool is_valid_size() const { return size < 0x10000; }
};
#pragma pack(pop)

class world_packet;
class encryptable_packet;
class world_session;
typedef struct z_stream_s z_stream;
enum connection_type : int8_t;
enum opcode_client : uint16_t;

class world_socket : public tcp_socket//<world_socket>
{
	static const std::string server_connection_initialize;
	static const std::string client_connection_initialize;
	static uint32_t min_size_for_compression;

	static uint8_t auth_check_seed[16];
	static uint8_t session_key_seed[16];
	static uint8_t continue_session_seed[16];
	static uint8_t encryption_key_seed[16];

	typedef tcp_socket/*<world_socket>*/ base_socket;
public:
	world_socket(event_loop* loop, tcp::socket&& sock);
	~world_socket();

	world_socket(const world_socket&) = delete;
	world_socket& operator=(const world_socket&) = delete;

	void start() override;
	bool update() override;

	void send_packet(const world_packet& packet);

	connection_type get_connection_type() const { return type_; }

	void send_auth_response_error(uint32_t code);
	void set_world_session(world_session* session);
	void set_send_buffer_size(size_t send_buffer_size) 
	{
		send_buffer_size_ = send_buffer_size; 
	}
protected:
	void on_close() override;
	void read_handler() override;
	bool read_header_handler();

	enum class read_data_handler_result
	{
		ok = 0,
		error = 1,
		waiting_for_query = 2
	};

	read_data_handler_result read_data_handler();
private:
	void set_read_handler_internal_callback()
	{
		set_read_handler_callback(std::bind(&world_socket::read_handler_internal, this, std::placeholders::_1, std::placeholders::_2));
	}

	void set_initialize_handler_callback()
	{
		set_read_handler_callback(std::bind(&world_socket::initialize_handler, this, std::placeholders::_1, std::placeholders::_2));
	}

	void check_ip_callback(prepared_query_result result);
	void initialize_handler(std::error_code& ec, size_t transferred_bytes);

	void log_opcode_text(opcode_client opcode, const std::unique_lock<std::mutex>& guard) const;
	void send_packet_and_log_opcode(const world_packet& packet);
	void write_packet_to_buffer(const encryptable_packet& packet, message_buffer& buffer);
	uint32_t compress_packet(uint8_t* buffer, const world_packet& ppacket);

	void handle_send_auth_session();
	void handle_auth_session(std::shared_ptr<world_packets::auth::auth_session> auth_session);
	void handle_auth_session_callback(std::shared_ptr<world_packets::auth::auth_session> auth_session, prepared_query_result result);
	void handle_auth_continued_session(std::shared_ptr<world_packets::auth::auth_continued_session> auth_session);
	void handle_auth_continued_session_callback(std::shared_ptr<world_packets::auth::auth_continued_session> auth_session, prepared_query_result result);
	void load_session_permissions_callback(prepared_query_result result);
	void handle_connect_to_failed(world_packets::auth::connect_to_failed& connect_to_failed);
	bool handle_ping(world_packets::auth::ping& ping);
	void handle_enable_encryption_ack();

	connection_type type_;
	uint64_t key_;

	big_number server_challenge_;
	world_packet_crypt auth_crypt_;
	big_number session_key_;
	uint8_t encrypt_key_[16];

	std::chrono::steady_clock::time_point last_ping_time_;
	uint32_t over_speed_ping_;

	std::mutex world_session_lock_;
	world_session* world_session_;
	bool authed_;

	message_buffer header_buffer_;
	message_buffer packet_buffer_;
	mpsc_queue<encryptable_packet> buffer_queue_;
	size_t send_buffer_size_;

	z_stream* compression_stream_;

	query_callback_processor query_processor_;
	std::string ip_country_;
};

#endif
