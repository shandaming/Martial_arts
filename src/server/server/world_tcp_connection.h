/*
 * Copyright (C) 2020
 */

#ifndef WORLD_TCP_CONNECTION_H
#define WORLD_TCP_CONNECTION_H

#include <networking/tcp_connection.h>

#pragma pack(push, 1)
struct packet_header
{
	uint32_t size;
	uint8_t* tag[12];

	bool is_valid_size() const { return size < 0x10000; }
};
#pragma pack(pop)

class encryptable_packet;
typedef struct z_stream_s z_stream;

class world_tcp_connection : public tcp_connection<world_tcp_connection>
{
	static const std::string server_connection_initialize;
	static const std::string client_connection_initialize;
	static constexpr uint32_t min_size_for_compression;

	static constexpr uint8_t auth_check_seed[16];
	static constexpr uint8_t session_key_seed[16];
	static constexpr uint8_t continue_session_seed[16];
	static constexpr uint8_t encryption_key_seed[16];

	typedef tcp_connection<world_tcp_connection> base_tcp_connection;
public:
	world_tcp_connection(event_loop* loop, socket&& socket);
	~world_tcp_connection();

	world_tcp_connection(const world_tcp_connection&) = delete;
	world_tcp_connection& operator=(const world_tcp_connection&) = delete;

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
	bool read_handler_handler();

	enum class read_data_handler_result
	{
		ok = 0;
		error = 1;
		waiting_for_query = 2
	};

	read_data_handler_result read_data_handler();
private:
	void set_read_handler_internal_callback()
	{
		set_read_handler_callback(std::bind(&WorldSocket::read_handler_internal, _1, _2, this);
	}

	void set_initializeHandler_callback()
	{
		set_read_handler_callback(std::bind(&WorldSocket::InitializeHandler, _1, _2, this);
	}

	void check_ip_callback(prepare_query_result result);
	void initialize_handler(std::error_code& ec, size_t transferred_bytes);

	void log_opcode_text(opcode_client opcode, const std::unique_lock<std::mutex>& guard) const;
	void send_packet_and_log_opcode(const world_packet& packet);
	void write_packet_to_buffer(const encryptable_packet& packet, message_buffer& buffer);
	uint32_t compress_packet(uint8_t* buffer, const world_packet& ppacket);

	void handle_send_auth_session();
	void handle_auth_session(std::shared_ptr<world_packet>)

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
