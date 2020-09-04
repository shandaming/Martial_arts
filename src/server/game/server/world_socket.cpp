/*
 * Copyright (C) 2020
 */

#include <zlib.h>

#include "world_socket.h"
#include "world_packet.h"
#include "world_session.h"
#include "database_env.h"
#include "authentication_packets.h"
#include "world.h"
#include "packet_log.h"
#include "errors.h"
#include "log.h"

#pragma pack(push, 1)

struct compressed_world_packet
{
	uint32_t uncompressed_size;
	uint32_t uncompressed_adler;
	uint32_t compressed_adler;
};

#pragma pack(pop)

class encryptable_packet : public world_packet
{
public:
	encryptable_packet(const world_packet& packet, bool encrypt) : 
		world_packet(packet), encrypt_(encrypt) { }

	bool needs_encryption() const { return encrypt_; }
private:
	bool encrypt_;
};


const std::string world_socket::server_connection_initialize(
		"WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT - V2");
const std::string world_socket::client_connection_initialize(
		"WORLD OF WARCRAFT CONNECTION - CLIENT TO SERVER - V2");
uint32_t world_socket::min_size_for_compression = 0x400;

uint8_t world_socket::auth_check_seed[16] = {
	0xC5, 0xC6, 0x98, 0x95, 0x76, 0x3F, 0x1D, 0xCD, 0xB6, 0xA1, 0x37, 0x28, 0xB3, 0x12, 0xFF, 0x8A };
uint8_t world_socket::session_key_seed[16] = { 
	0x58, 0xCB, 0xCF, 0x40, 0xFE, 0x2E, 0xCE, 0xA6, 0x5A, 0x90, 0xB8, 0x01, 0x68, 0x6C, 0x28, 0x0B };
uint8_t world_socket::continue_session_seed[16] = { 
	0x16, 0xAD, 0x0C, 0xD4, 0x46, 0xF9, 0x4F, 0xB2, 0xEF, 0x7D, 0xEA, 0x2A, 0x17, 0x66, 0x4D, 0x2F };
uint8_t world_socket::encryption_key_seed[16] = { 
	0xE9, 0x75, 0x3C, 0x50, 0x90, 0x93, 0x61, 0xDA, 0x3B, 0x07, 0xEE, 0xFA, 0xFF, 0x9D, 0x41, 0xB8 };

world_socket::world_socket(event_loop* loop, tcp::socket&& sock) : 
	tcp_socket(loop, std::forward<tcp::socket>(sock)), 
	type_(CONNECTION_TYPE_REALM), 
	key_(0), over_speed_ping_(0),
	world_session_(nullptr), 
	authed_(false), send_buffer_size_(4096), 
	compression_stream_(nullptr)
{
	server_challenge_.set_rand(8 * 16);
	memset(encrypt_key_, 0, sizeof(encrypt_key_));
	header_buffer_.resize(sizeof(packet_header));
}

world_socket::~world_socket()
{
	if(compression_stream_)
	{
		deflateEnd(compression_stream_);
		delete compression_stream_;
	}
}

void world_socket::start()
{
	std::string ip_address = get_remote_ip_address().to_string();
	login_database_prepared_statement* stmt = login_database.get_prepared_statement(LOGIN_SEL_IP_INFO);
	stmt->set_string(0, ip_address);
	query_processor_.add_query(login_database.async_query(stmt).with_prepared_callback(
				std::bind(&world_socket::check_ip_callback, this, std::placeholders::_1)));
}

void world_socket::check_ip_callback(prepared_query_result result)
{
	if(result)
	{
		bool banned = false;
		do
		{
			field* fields = result->fetch();
			if (fields[0].get_uint64() != 0)
				banned = true;

		}while (result->next_row());

		if(banned)
		{
			LOG_ERROR("network", "world_socket::check_ip_callback: Sent Auth Response "
					"(IP %s banned).",
					get_remote_ip_address().to_string().c_str());
			delayed_close_socket();
			return;
		}
	}

	packet_buffer_.resize(client_connection_initialize.length() + 1);

	//AsyncReadWithCallback(&world_socket::initialize_handler);
	set_initialize_handler_callback();

	message_buffer initializer;
	initializer.write(server_connection_initialize.c_str(), server_connection_initialize.length());
	initializer.write("\n", 1);

	// - IoContext.run thread, safe.
	queue_packet(std::move(initializer));
}

void world_socket::initialize_handler(std::error_code& error, size_t transfered_bytes)
{
	if(error)
	{
		close_socket();
		return;
	}

	get_read_buffer().write_completed(transfered_bytes);

	message_buffer& packet = get_read_buffer();
	if (packet.get_active_size() > 0)
	{
		if (packet_buffer_.get_remaining_space() > 0)
		{
			// need to receive the header
			size_t read_header_size = std::min(packet.get_active_size(), 
					packet_buffer_.get_remaining_space());
			packet_buffer_.write(packet.get_read_pointer(), read_header_size);
			packet.read_completed(read_header_size);

			if (packet_buffer_.get_remaining_space() > 0)
			{
				// Couldn't receive the whole header this time.
				ASSERT(packet.get_active_size() == 0);
				//AsyncReadWithCallback(&world_socket::initialize_handler);
				set_initialize_handler_callback();
				return;
			}

			byte_buffer buffer(std::move(packet_buffer_));
			std::string initializer = buffer.read_string(client_connection_initialize.length());
			if (initializer != client_connection_initialize)
			{
				close_socket();
				return;
			}

			uint8_t terminator;
			buffer >> terminator;
			if (terminator != '\n')
			{
				close_socket();
				return;
			}

			compression_stream_ = new z_stream();
			compression_stream_->zalloc = (alloc_func)NULL;
			compression_stream_->zfree = (free_func)NULL;
			compression_stream_->opaque = (voidpf)NULL;
			compression_stream_->avail_in = 0;
			compression_stream_->next_in = NULL;
			int32_t z_res = deflateInit2(compression_stream_, WORLD->get_int_configs(CONFIG_COMPRESSION), 
					Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
			if (z_res != Z_OK)
			{
				close_socket();
				LOG_ERROR("network", "Can't initialize packet compression (zlib: deflateInit) "
						"error code: %i (%s)", z_res, zError(z_res));
				return;
			}

			packet_buffer_.reset();
			handle_send_auth_session();
			//AsyncRead();
			set_read_handler_internal_callback();
			return;
		}
	}

	//AsyncReadWithCallback(&world_socket::initialize_handler);
	set_initialize_handler_callback();
}

bool world_socket::update()
{
	encryptable_packet* queued;
	message_buffer buffer(send_buffer_size_);

	while(buffer_queue_.dequeue(queued))
	{
		uint32_t packet_size = queued->size();
		if(packet_size > min_size_for_compression && queued->needs_encryption())
            		packet_size = compressBound(packet_size) + sizeof(compressed_world_packet);

        	if(buffer.get_remaining_space() < packet_size + sizeof(packet_header))
        	{
            		queue_packet(std::move(buffer));
            		buffer.resize(send_buffer_size_);
        	}

        	if(buffer.get_remaining_space() >= packet_size + sizeof(packet_header))
            		write_packet_to_buffer(*queued, buffer);
        	else    // single packet larger than 4096 bytes
        	{
            		message_buffer packet_buffer(packet_size + sizeof(packet_header));
            		write_packet_to_buffer(*queued, packet_buffer);
            		queue_packet(std::move(packet_buffer));
        	}

        	delete queued;
    	}

    	if(buffer.get_active_size() > 0)
        	queue_packet(std::move(buffer));

    	if(!base_socket::update())
        	return false;

    	query_processor_.process_ready_queries();

    	return true;
}

void world_socket::handle_send_auth_session()
{
	big_number dos_challenge;
	dos_challenge.set_rand(32 * 8);

	world_packets::auth::auth_challenge challenge;
	memcpy(challenge.challenge.data(), server_challenge_.as_byte_array(16).get(), 16);
	memcpy(challenge.dos_challenge.data(), dos_challenge.as_byte_array(32).get(), 32);
	challenge.dos_zero_bits = 1;

	send_packet_and_log_opcode(*challenge.write());
}

void world_socket::on_close()
{
	{
        	std::lock_guard<std::mutex> lock(world_session_lock_);
        	world_session_ = nullptr;
    	}
}

void world_socket::read_handler()
{
	if(!is_open())
        	return;

    	message_buffer& packet = get_read_buffer();
    	while(packet.get_active_size() > 0)
    	{
        	if(header_buffer_.get_remaining_space() > 0)
        	{
            		// need to receive the header
            		size_t read_header_size = std::min(packet.get_active_size(), 
					header_buffer_.get_remaining_space());
            		header_buffer_.write(packet.get_read_pointer(), read_header_size);
            		packet.read_completed(read_header_size);

            		if(header_buffer_.get_remaining_space() > 0)
            		{
                		// Couldn't receive the whole header this time.
                		ASSERT(packet.get_active_size() == 0);
                		break;
            		}

            		// We just received nice new header
            		if(!read_header_handler())
            		{
                		close_socket();
                		return;
            		}
        	}

        	// We have full read header, now check the data payload
        	if(packet_buffer_.get_remaining_space() > 0)
        	{
            		// need more data in the payload
            		size_t read_data_size = std::min(packet.get_active_size(), 
					packet_buffer_.get_remaining_space());
            		packet_buffer_.write(packet.get_read_pointer(), read_data_size);
            		packet.read_completed(read_data_size);

            		if(packet_buffer_.get_remaining_space() > 0)
            		{
                		// Couldn't receive the whole data this time.
                		ASSERT(packet.get_active_size() == 0);
                		break;
            		}
        	}

        	// just received fresh new payload
        	read_data_handler_result result = read_data_handler();
        	header_buffer_.reset();
        	if(result != read_data_handler_result::ok)
        	{
            		if(result != read_data_handler_result::waiting_for_query)
                	close_socket();

            		return;
        	}
    	}

    	//AsyncRead();
	set_read_handler_internal_callback();
}

void world_socket::set_world_session(world_session* session)
{
	std::lock_guard<std::mutex> lock(world_session_lock_);
	world_session_ = session;
	authed_ = true;
}

bool world_socket::read_header_handler()
{
	ASSERT(header_buffer_.get_active_size() == sizeof(packet_header), 
			"Header size " SZFMTD " different than expected " SZFMTD, header_buffer_.get_active_size(), 
			sizeof(packet_header));

    	packet_header* header = reinterpret_cast<packet_header*>(header_buffer_.get_read_pointer());

    	if(!header->is_valid_size())
    	{
		LOG_ERROR("network", "world_socket::read_header_handler(): "
				"client %s sent malformed packet (size: %u)",
            	get_remote_ip_address().to_string().c_str(), header->size);
        	return false;
    	}

    	packet_buffer_.resize(header->size);
    	return true;
}

world_socket::read_data_handler_result world_socket::read_data_handler()
{
	packet_header* header = reinterpret_cast<packet_header*>(header_buffer_.get_read_pointer());

    	if(!auth_crypt_.decrypt_recv(packet_buffer_.get_read_pointer(), header->size, header->tag))
    	{
        	LOG_ERROR("network", "world_socket::read_header_handler(): "
				"client %s failed to decrypt packet (size: %u)",
            	get_remote_ip_address().to_string().c_str(), header->size);
        	return read_data_handler_result::error;
    	}

    	world_packet packet(std::move(packet_buffer_), get_connection_type());
    	opcode_client opcode = packet.read<opcode_client>();
    	if(uint32_t(opcode) >= uint32_t(NUM_OPCODE_HANDLERS))
    	{
        	LOG_ERROR("network", "world_socket::read_header_handler(): "
				"client %s sent wrong opcode (opcode: %u)",
            	get_remote_ip_address().to_string().c_str(), uint32_t(opcode));
        	return read_data_handler_result::error;
    	}

    	packet.set_opcode(opcode);

    	if(PACKET_LOG->can_log_packet())
        	PACKET_LOG->log_packet(packet, CLIENT_TO_SERVER, get_remote_ip_address(), get_remote_port(), 
				get_connection_type());

    	std::unique_lock<std::mutex> lock(world_session_lock_, std::defer_lock);

    	switch(opcode)
	{
		case CMSG_PING:
        	{
            		log_opcode_text(opcode, lock);
            		world_packets::auth::ping ping(std::move(packet));
            		if(!ping.read_no_throw())
            		{
                		LOG_ERROR("network", "world_socket::read_data_handler(): "
						"client %s sent malformed CMSG_PING", get_remote_ip_address().to_string().c_str());
                		return read_data_handler_result::error;
            		}
            		if(!handle_ping(ping))
                		return read_data_handler_result::error;
            		break;
        	}
        	case CMSG_AUTH_SESSION:
        	{
            		log_opcode_text(opcode, lock);
            		if(authed_)
            		{
                		// locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                		if(lock.try_lock())
                    			LOG_ERROR("network", "world_socket::ProcessIncoming: "
							"received duplicate CMSG_AUTH_SESSION from %s", world_session_->get_player_info().c_str());
                		return read_data_handler_result::error;
            		}

            		std::shared_ptr<world_packets::auth::auth_session> auth_session = std::make_shared<world_packets::auth::auth_session>(std::move(packet));
            		if(!auth_session->read_no_throw())
            		{
                		LOG_ERROR("network", "world_socket::read_data_handler(): client %s sent malformed CMSG_AUTH_SESSION", get_remote_ip_address().to_string().c_str());
                		return read_data_handler_result::error;
            		}
            		handle_auth_session(auth_session);
            		return read_data_handler_result::waiting_for_query;
        	}
        	case CMSG_AUTH_CONTINUED_SESSION:
        	{
            		log_opcode_text(opcode, lock);
            		if(authed_)
            		{
                		// locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                		if(lock.try_lock())
                    			LOG_ERROR("network", "world_socket::ProcessIncoming: received duplicate CMSG_AUTH_CONTINUED_SESSION from %s", world_session_->get_player_info().c_str());
                		return read_data_handler_result::error;
            		}

            		std::shared_ptr<world_packets::auth::auth_continued_session> auth_session = std::make_shared<world_packets::auth::auth_continued_session>(std::move(packet));
            		if(!auth_session->read_no_throw())
            		{
                		LOG_ERROR("network", "world_socket::read_data_handler(): client %s sent malformed CMSG_AUTH_CONTINUED_SESSION", get_remote_ip_address().to_string().c_str());
                		return read_data_handler_result::error;
            		}
            		handle_auth_continued_session(auth_session);
            		return read_data_handler_result::waiting_for_query;
        	}
        	case CMSG_KEEP_ALIVE:
            		log_opcode_text(opcode, lock);
            		break;
        	case CMSG_LOG_DISCONNECT:
            		log_opcode_text(opcode, lock);
            		packet.rfinish();   // contains uint32_t disconnectReason;
            		break;
        	case CMSG_ENABLE_NAGLE:
            		log_opcode_text(opcode, lock);
            		set_no_delay(false);
            		break;
        	case CMSG_CONNECT_TO_FAILED:
        	{
            		lock.lock();

            		log_opcode_text(opcode, lock);
            		world_packets::auth::connect_to_failed connect_to_failed(std::move(packet));
            		if(!connect_to_failed.read_no_throw())
            		{
                		LOG_ERROR("network", "world_socket::read_data_handler(): client %s sent malformed CMSG_CONNECT_TO_FAILED", get_remote_ip_address().to_string().c_str());
                		return read_data_handler_result::error;
            		}
            		handle_connect_to_failed(connect_to_failed);
            		break;
        	}
        	case CMSG_ENABLE_ENCRYPTION_ACK:
            		log_opcode_text(opcode, lock);
            		handle_enable_encryption_ack();
            		break;
        	default:
		{
            		lock.lock();

            		log_opcode_text(opcode, lock);

            		if(!world_session_)
            		{
                		LOG_ERROR("network.opcode", "ProcessIncoming: Client not authed opcode = %u", uint32_t(opcode));
                		return read_data_handler_result::error;
            		}

            		opcode_handler const* handler = opcode_table[opcode];
            		if(!handler)
            		{
                		LOG_ERROR("network.opcode", "No defined handler for opcode %s sent by %s", get_opcode_name_for_logging(static_cast<opcode_client>(packet.get_opcode())).c_str(), world_session_->get_player_info().c_str());
                		break;
            		}

            		// Our Idle timer will reset on any non PING opcodes.
            		// Catches people idling on the login screen and any lingering ingame connections.
            		world_session_->reset_timeout_time();

            		// Copy the packet to the heap before enqueuing
            		world_session_->queue_packet(new world_packet(std::move(packet)));
            		break;
        	}
    	}

    	return read_data_handler_result::ok;
}

void world_socket::log_opcode_text(OpcodeClient opcode, std::unique_lock<std::mutex> const& guard) const
{
    if (!guard)
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", get_remote_ip_address().to_string().c_str(), get_opcode_name_for_logging(opcode).c_str());
    }
    else
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", (world_session_ ? world_session_->GetPlayerInfo() : get_remote_ip_address().to_string()).c_str(),
            get_opcode_name_for_logging(opcode).c_str());
    }
}

void world_socket::send_packet_and_log_opcode(world_packet const& packet)
{
    TC_LOG_TRACE("network.opcode", "S->C: %s %s", get_remote_ip_address().to_string().c_str(), get_opcode_name_for_logging(static_cast<opcode_server>(packet.get_opcode())).c_str());
    send_packet(packet);
}

void world_socket::send_packet(world_packet const& packet)
{
    if (!is_open())
        return;

    if (sPacketLog->can_log_packet())
        sPacketLog->log_packet(packet, SERVER_TO_CLIENT, get_remote_ip_address(), get_remote_port(), GetConnectionType());

    buffer_queue_.enqueue(new encryptable_packet(packet, auth_crypt_.IsInitialized()));
}

void world_socket::write_packet_to_buffer(encryptable_packet const& packet, message_buffer& buffer)
{
    uint16_t opcode = packet.get_opcode();
    uint32_t packet_size = packet.size();

    // Reserve space for buffer
    uint8_t* header_pos = buffer.get_write_pointer();
    buffer.write_completed(sizeof(packet_header));
    uint8_t* data_pos = buffer.get_write_pointer();
    buffer.write_completed(sizeof(opcode));

    if (packet_size > min_size_for_compression && packet.needs_encryption())
    {
        compressed_world_packet cmp;
        cmp.UncompressedSize = packet_size + 2;
        cmp.UncompressedAdler = adler32(adler32(0x9827D8F1, (Bytef*)&opcode, 2), packet.contents(), packet_size);

        // Reserve space for compression info - uncompressed size and checksums
        uint8_t* compressionInfo = buffer.get_write_pointer();
        buffer.write_completed(sizeof(compressed_world_packet));

        uint32_t compressedSize = CompressPacket(buffer.get_write_pointer(), packet);

        cmp.CompressedAdler = adler32(0x9827D8F1, buffer.get_write_pointer(), compressedSize);

        memcpy(compressionInfo, &cmp, sizeof(compressed_world_packet));
        buffer.write_completed(compressedSize);
        packet_size = compressedSize + sizeof(compressed_world_packet);

        opcode = SMSG_COMPRESSED_PACKET;
    }
    else if (!packet.empty())
        buffer.write(packet.contents(), packet.size());

    memcpy(data_pos, &opcode, sizeof(opcode));
    packet_size += 2 /*opcode*/;

    packet_header header;
    header.Size = packet_size;
    auth_crypt_.EncryptSend(data_pos, header.Size, header.Tag);

    memcpy(header_pos, &header, sizeof(packet_header));
}

uint32_t world_socket::CompressPacket(uint8_t* buffer, world_packet const& packet)
{
    uint32_t opcode = packet.get_opcode();
    uint32_t bufferSize = deflateBound(compression_stream_, packet.size() + sizeof(uint16_t));

    compression_stream_->next_out = buffer;
    compression_stream_->avail_out = bufferSize;
    compression_stream_->next_in = (Bytef*)&opcode;
    compression_stream_->avail_in = sizeof(uint16_t);

    int32 z_res = deflate(compression_stream_, Z_NO_FLUSH);
    if (z_res != Z_OK)
    {
        LOG_ERROR("network", "Can't compress packet opcode (zlib: deflate) error code: %i (%s, msg: %s)", z_res, zError(z_res), compression_stream_->msg);
        return 0;
    }

    compression_stream_->next_in = (Bytef*)packet.contents();
    compression_stream_->avail_in = packet.size();

    z_res = deflate(compression_stream_, Z_SYNC_FLUSH);
    if (z_res != Z_OK)
    {
        LOG_ERROR("network", "Can't compress packet data (zlib: deflate) error code: %i (%s, msg: %s)", z_res, zError(z_res), compression_stream_->msg);
        return 0;
    }

    return bufferSize - compression_stream_->avail_out;
}

struct AccountInfo
{
    struct
    {
        uint32_t Id;
        bool IsLockedToIP;
        std::string LastIP;
        std::string LockCountry;
        LocaleConstant Locale;
        bool IsBanned;

    } BattleNet;

    struct
    {
        uint32_t Id;
        std::array<uint8_t, 64> KeyData;
        uint8_t Expansion;
        int64 MuteTime;
        uint32_t Recruiter;
        std::string OS;
        bool IsRectuiter;
        AccountTypes Security;
        bool IsBanned;
    } Game;

    bool IsBanned() const { return BattleNet.IsBanned || Game.IsBanned; }

    explicit AccountInfo(field* fields)
    {
        //           0             1           2          3                4            5           6          7            8     9     10          11
        // SELECT a.id, a.sessionkey, ba.last_ip, ba.locked, ba.lock_country, a.expansion, a.mutetime, ba.locale, a.recruiter, a.os, ba.id, aa.gmLevel,
        //                                                              12                                                            13    14
        // bab.unbandate > UNIX_TIMESTAMP() OR bab.unbandate = bab.bandate, ab.unbandate > UNIX_TIMESTAMP() OR ab.unbandate = ab.bandate, r.id
        // FROM account a LEFT JOIN battlenet_accounts ba ON a.battlenet_account = ba.id LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID IN (-1, ?)
        // LEFT JOIN battlenet_account_bans bab ON ba.id = bab.id LEFT JOIN account_banned ab ON a.id = ab.id LEFT JOIN account r ON a.id = r.recruiter
        // WHERE a.username = ? ORDER BY aa.RealmID DESC LIMIT 1
        Game.Id = fields[0].GetUInt32();
        HexStrToByteArray(fields[1].GetString(), Game.KeyData.data());
        BattleNet.LastIP = fields[2].GetString();
        BattleNet.IsLockedToIP = fields[3].GetBool();
        BattleNet.LockCountry = fields[4].GetString();
        Game.Expansion = fields[5].GetUInt8();
        Game.MuteTime = fields[6].GetInt64();
        BattleNet.Locale = LocaleConstant(fields[7].GetUInt8());
        Game.Recruiter = fields[8].GetUInt32();
        Game.OS = fields[9].GetString();
        BattleNet.Id = fields[10].GetUInt32();
        Game.Security = AccountTypes(fields[11].GetUInt8());
        BattleNet.IsBanned = fields[12].GetUInt32() != 0;
        Game.IsBanned = fields[13].GetUInt32() != 0;
        Game.IsRectuiter = fields[14].GetUInt32() != 0;

        if (BattleNet.Locale >= TOTAL_LOCALES)
            BattleNet.Locale = LOCALE_enUS;
    }
};

void world_socket::HandleAuthSession(std::shared_ptr<world_packets::Auth::AuthSession> authSession)
{
    // Get the account information from the auth database
    LoginDatabasePreparedStatement* stmt = login_database.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME);
    stmt->setInt32(0, int32(realm.Id.Realm));
    stmt->setString(1, authSession->RealmJoinTicket);

    query_processor_.AddQuery(login_database.async_query(stmt).with_prepared_callback(std::bind(&world_socket::HandleAuthSessionCallback, this, authSession, std::placeholders::_1)));
}

void world_socket::HandleAuthSessionCallback(std::shared_ptr<world_packets::Auth::AuthSession> authSession, prepared_query_result result)
{
    // Stop if the account is not found
    if (!result)
    {
        // We can not log here, as we do not know the account. Thus, no accountId.
        LOG_ERROR("network", "world_socket::HandleAuthSession: Sent Auth Response (unknown account).");
        delayed_close_socket();
        return;
    }

    RealmBuildInfo const* buildInfo = sRealmList->GetBuildInfo(realm.Build);
    if (!buildInfo)
    {
        SendAuthResponseError(ERROR_BAD_VERSION);
        LOG_ERROR("network", "world_socket::HandleAuthSession: Missing auth seed for realm build %u (%s).", realm.Build, get_remote_ip_address().to_string().c_str());
        delayed_close_socket();
        return;
    }

    AccountInfo account(result->Fetch());

    // For hook purposes, we get Remoteaddress at this point.
    std::string address = get_remote_ip_address().to_string();

    SHA256Hash digestKeyHash;
    digestKeyHash.UpdateData(account.Game.KeyData.data(), account.Game.KeyData.size());
    if (account.Game.OS == "Wn64")
        digestKeyHash.UpdateData(buildInfo->Win64AuthSeed.data(), buildInfo->Win64AuthSeed.size());
    else if (account.Game.OS == "Mc64")
        digestKeyHash.UpdateData(buildInfo->Mac64AuthSeed.data(), buildInfo->Mac64AuthSeed.size());

    digestKeyHash.Finalize();

    HmacSha256 hmac(digestKeyHash.GetLength(), digestKeyHash.GetDigest());
    hmac.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    hmac.UpdateData(server_challenge_.as_byte_array(16).get(), 16);
    hmac.UpdateData(auth_check_seed, 16);
    hmac.Finalize();

    // Check that Key and account name are the same on client and server
    if (memcmp(hmac.GetDigest(), authSession->Digest.data(), authSession->Digest.size()) != 0)
    {
        LOG_ERROR("network", "world_socket::HandleAuthSession: Authentication failed for account: %u ('%s') address: %s", account.Game.Id, authSession->RealmJoinTicket.c_str(), address.c_str());
        delayed_close_socket();
        return;
    }

    SHA256Hash keyData;
    keyData.UpdateData(account.Game.KeyData.data(), account.Game.KeyData.size());
    keyData.Finalize();

    HmacSha256 sessionKeyHmac(keyData.GetLength(), keyData.GetDigest());
    sessionKeyHmac.UpdateData(server_challenge_.as_byte_array(16).get(), 16);
    sessionKeyHmac.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    sessionKeyHmac.UpdateData(session_key_seed, 16);
    sessionKeyHmac.Finalize();

    uint8_t sessionKey[40];
    SessionKeyGenerator<SHA256Hash> sessionKeyGenerator(sessionKeyHmac.GetDigest(), sessionKeyHmac.GetLength());
    sessionKeyGenerator.Generate(sessionKey, 40);

    _sessionKey.SetBinary(sessionKey, 40);

    HmacSha256 encryptKeyGen(40, sessionKey);
    encryptKeyGen.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    encryptKeyGen.UpdateData(server_challenge_.as_byte_array(16).get(), 16);
    encryptKeyGen.UpdateData(encryption_key_seed, 16);
    encryptKeyGen.Finalize();

    // only first 16 bytes of the hmac are used
    memcpy(encrypt_key_, encryptKeyGen.GetDigest(), 16);

    // As we don't know if attempted login process by ip works, we update last_attempt_ip right away
    LoginDatabasePreparedStatement* stmt = login_database.GetPreparedStatement(LOGIN_UPD_LAST_ATTEMPT_IP);
    stmt->setString(0, address);
    stmt->setString(1, authSession->RealmJoinTicket);
    login_database.Execute(stmt);
    // This also allows to check for possible "hack" attempts on account

    stmt = login_database.GetPreparedStatement(LOGIN_UPD_ACCOUNT_INFO_CONTINUED_SESSION);
    stmt->setString(0, _sessionKey.AsHexStr());
    stmt->setUInt32(1, account.Game.Id);
    login_database.Execute(stmt);

    // First reject the connection if packet contains invalid data or realm state doesn't allow logging in
    if (sWorld->IsClosed())
    {
        SendAuthResponseError(ERROR_DENIED);
        LOG_ERROR("network", "world_socket::HandleAuthSession: World closed, denying client (%s).", get_remote_ip_address().to_string().c_str());
        delayed_close_socket();
        return;
    }

    if (authSession->RealmID != realm.Id.Realm)
    {
        SendAuthResponseError(ERROR_DENIED);
        LOG_ERROR("network", "world_socket::HandleAuthSession: Client %s requested connecting with realm id %u but this realm has id %u set in config.",
            get_remote_ip_address().to_string().c_str(), authSession->RealmID, realm.Id.Realm);
        delayed_close_socket();
        return;
    }

    // Must be done before world_session is created
    bool wardenActive = sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED);
    if (wardenActive && account.Game.OS != "Win" && account.Game.OS != "Wn64" && account.Game.OS != "Mc64")
    {
        SendAuthResponseError(ERROR_DENIED);
        LOG_ERROR("network", "world_socket::HandleAuthSession: Client %s attempted to log in using invalid client OS (%s).", address.c_str(), account.Game.OS.c_str());
        delayed_close_socket();
        return;
    }

    if (IpLocationRecord const* location = sIPLocation->GetLocationRecord(address))
        _ipCountry = location->CountryCode;

    ///- Re-check ip locking (same check as in auth).
    if (account.BattleNet.IsLockedToIP)
    {
        if (account.BattleNet.LastIP != address)
        {
            SendAuthResponseError(ERROR_RISK_ACCOUNT_LOCKED);
            TC_LOG_DEBUG("network", "world_socket::HandleAuthSession: Sent Auth Response (Account IP differs. Original IP: %s, new IP: %s).", account.BattleNet.LastIP.c_str(), address.c_str());
            // We could log on hook only instead of an additional db log, however action logger is config based. Better keep DB logging as well
            sScriptMgr->OnFailedAccountLogin(account.Game.Id);
            delayed_close_socket();
            return;
        }
    }
    else if (!account.BattleNet.LockCountry.empty() && account.BattleNet.LockCountry != "00" && !_ipCountry.empty())
    {
        if (account.BattleNet.LockCountry != _ipCountry)
        {
            SendAuthResponseError(ERROR_RISK_ACCOUNT_LOCKED);
            TC_LOG_DEBUG("network", "world_socket::HandleAuthSession: Sent Auth Response (Account country differs. Original country: %s, new country: %s).", account.BattleNet.LockCountry.c_str(), _ipCountry.c_str());
            // We could log on hook only instead of an additional db log, however action logger is config based. Better keep DB logging as well
            sScriptMgr->OnFailedAccountLogin(account.Game.Id);
            delayed_close_socket();
            return;
        }
    }

    int64 mutetime = account.Game.MuteTime;
    //! Negative mutetime indicates amount of seconds to be muted effective on next login - which is now.
    if (mutetime < 0)
    {
        mutetime = time(NULL) + llabs(mutetime);

        stmt = login_database.GetPreparedStatement(LOGIN_UPD_MUTE_TIME_LOGIN);
        stmt->setInt64(0, mutetime);
        stmt->setUInt32(1, account.Game.Id);
        login_database.Execute(stmt);
    }

    if (account.IsBanned())
    {
        SendAuthResponseError(ERROR_GAME_ACCOUNT_BANNED);
        LOG_ERROR("network", "world_socket::HandleAuthSession: Sent Auth Response (Account banned).");
        sScriptMgr->OnFailedAccountLogin(account.Game.Id);
        delayed_close_socket();
        return;
    }

    // Check locked state for server
    AccountTypes allowedAccountType = sWorld->GetPlayerSecurityLimit();
    TC_LOG_DEBUG("network", "Allowed Level: %u Player Level %u", allowedAccountType, account.Game.Security);
    if (allowedAccountType > SEC_PLAYER && account.Game.Security < allowedAccountType)
    {
        SendAuthResponseError(ERROR_SERVER_IS_PRIVATE);
        TC_LOG_DEBUG("network", "world_socket::HandleAuthSession: User tries to login but his security level is not enough");
        sScriptMgr->OnFailedAccountLogin(account.Game.Id);
        delayed_close_socket();
        return;
    }

    TC_LOG_DEBUG("network", "world_socket::HandleAuthSession: Client '%s' authenticated successfully from %s.", authSession->RealmJoinTicket.c_str(), address.c_str());

    // update the last_ip in the database as it was successful for login
    stmt = login_database.GetPreparedStatement(LOGIN_UPD_LAST_IP);

    stmt->setString(0, address);
    stmt->setString(1, authSession->RealmJoinTicket);

    login_database.Execute(stmt);

    // At this point, we can safely hook a successful login
    sScriptMgr->OnAccountLogin(account.Game.Id);

    authed_ = true;
    world_session_ = new world_session(account.Game.Id, std::move(authSession->RealmJoinTicket), account.BattleNet.Id, shared_from_this(), account.Game.Security,
        account.Game.Expansion, mutetime, account.Game.OS, account.BattleNet.Locale, account.Game.Recruiter, account.Game.IsRectuiter);

    // Initialize Warden system only if it is enabled by config
    if (wardenActive)
        world_session_->InitWarden(&_sessionKey);

    query_processor_.AddQuery(world_session_->LoadPermissionsAsync().with_prepared_callback(std::bind(&world_socket::LoadSessionPermissionsCallback, this, std::placeholders::_1)));
    AsyncRead();
}

void world_socket::handle_auth_session(std::shared_ptr<world_packets::auth::auth_session> auth_session)
{
	login_database_prepared_statement* stmt = login_database.get_prepared_statement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME);
	stmt->set_int32(0, int32_t(realm.id.realm));
	stmt->set_string(1, auth_session->realm_join_ticket);

	query_processor_.add_query(login_database.async_query(stmt).with_prepared_callback(std::bind(&world_socket::handle_auth_session_callback, this, auth_session, std::placeholders::_1)));
}

void world_socket::handle_auth_session_callback(std::shared_ptr<world_packets::auth::auth_session> session, prepared_query_result result)
{
	if(!result)
	{
		LOG_ERROR("network", "World_socket::handle_auth_session: Sent Auth Response (unknown account).");
		delayed_close_socket();
		return;
	}

	const realm_build_info* build_info = REALM_LIST->get_build_info(realm.build);
	if(!build_info)
	{
		send_auth_response_error(ERROR_BAD_VERSION);
		LOG_ERROR("network", "world_socket::handle_auth_session: Missing auth seed for realm build %u (%s).", realm.build, get_remote_ip_address().to_string().c_str());
		return;
	}

	account_info account(result->fetch());

	std::string address = get_remote_ip_address().to_string();

	sha256_hash digest_key_hash;
	digest_key_hash.update_data(account.game.key_data.data(), account.game.key_data.size());
	if(account.game.os == "Win64")
		digest_key_hash.update_data(build_info->win64_auth_seed.data(), build_info->win64_auth_seed.size());
	else if(account.game.os == "Mc64")
		digest_key_hash.update_data(build_info->mac64_auth_seed.data(), build_info->mac64_auth_seed.size());

	digest_key_hash.finalize();

	hmac_sha256 hmac(digest_key_hash.get_length(), digest_key_hash.get_digest());
	hmac.update_data(auth_session->local_challenge.data(), auth_session->local_challenge.size());
	hmac.update_data(server_challenge_.as_byte_array(16).get(), 16);
	hmac.update_data(auth_check_seed, 16);
	hmac.finalize();

	if(memcmp(hmac.get_digest(), auth_session->digest.data(), auth_session->digest.size()) != 0)
	{
		LOG_ERROR("network", "world_socket::handle_auth_session: Authentication failed for account: %u ('%s') address: %s", account.game.id, auth_session->realm_join_ticket.c_str(), address.c_str());
		delayed_close_socket();
		return;
	}

	sha256_hash key_data;
	key_data.update_data(account.game.key_data.data(), account.game.key_data.size());
	key_data.finalize();

	hmac_sha256 session_key_hmac(key_data.get_length(), key_data.get_digest());
	session_key_hmac.update_data(server_challenge_.as_byte_array(16).get(), 16);
	session_key_hmac.update_data(auth_session->local_challenge.data(), auth_session->local_challenge.size());
	session_key_hmac.update_data(session_key_seed, 16);
	session_key_hmac.finalize();

	uint8_t session_key[40];
	session_key_generator<sha256_hash> session_key_generator(session_key_hmac.get_digest(), session_key_hmac.get_length());
	session_key_generator.generate(session_key, 40);

	session_key_.set_binary(session_key, 40);

	hmac_sha256 encrypt_key_gen(40, session_key);
	encrypt_key_gen.update_data(auth_session->local_challenge.data(), auth_session->local_challenge.size());
	encrypt_key_gen.update_data(server_challenge.as_byte_array(16).get(), 16);
	encrypt_key_gen.update_data(encryption_key_seed, 16);
	encrypt_key_gen.finalize();

	memcpy(encrypt_key_, encrypt_key_gen.get_digest(), 16);

	login_database_prepared_statement* stmt = login_database.get_prepared_statement(LOGIN_UPD_LAST_ATTEMPT_IP);
	stmt->set_string(0, address);
	stmt->set_string(1, auth_session->realm_join_ticket);
	login_database.execute(stmt);

	stmt = login_database.get_prepared_statement(LOGIN_UPD_ACCOUNT_INFO_CONTINUED_SESSION);
	stmt->set_string(0, session_key_.as_hex_str());
	stmt->set_uint32(1, account.game.id);
	login_database.execute(stmt);

	if(WORLD->is_closed())
	{
		send_auth_response_error(ERROR_DENIED);
		LOG_ERROR("network", "world_socket::handle_auth_session: World closed, denying client (%s).", get_remote_ip_address().to_string().c_str());
		delayed_close_socket();
		return;
	}

	if(auth_session->realm_id != realm.id.realm)
	{
		send_auth_response_error(ERROR_DENIED);
		LOG_ERROR("network", "world_socket::handle_auth_session: Client %s requested connection with realm id %u but this realm has id %u set in config.", get_remote_ip_address().to_string().c_str(), auth_session->realm_id, realm.id.realm);
		delayed_close_socket();
		return;
	}

	bool warden_active = WORLD->get_bool_config(CONFIG_WAROEN_ENABLED);
	if(warden_active && account.game.os != "Win" && account.game.os != "Win64" && account.game.os != "Mc64")
	{
		send_auth_response_error(ERROR_DENIED);
		LOG_ERROR("network", "world_socket::handle_auth_session: Client %s attempted to log in using invalid client OS (%s)", address.c_str(), account.game.os.c_str());
		delayed_close_socket();
		return;
	}

	if(const ip_location_record* location = IP_LOCATION->get_location_record(address))
		ip_contry_ = location->country_code;

	if(account.battle_net.is_locked_to_ip)
	{
		if(account.battle_net.last_ip != address)
		{
			send_auth_response_error(ERROR_RISK_ACCOUNT_LOCKED);
			LOG_DEBUG("network", "worlk_socket::handle_auth_session: Sent Auth Response (Account IP differs. Original IP: %s, new IP: %s).", account.battle_net.last_ip.c_str(), address.c_str());
			SCRIPT_MGR->on_failed_account_login(account.game.id);
			delayed_close_socket();
			return;
		}
	}
	else if(!account.battle_net.lock_country.empty() &&
			account.battle_net.lock_country != "00" &&
			!ip_country_.empty())
	{
		if(account.battle_net.lock_country != ip_country_)
		{
			send_auth_response_error(ERROR_RISK_ACCOUNT_LOCKED);
			LOG_DEBUG("network", "world_socket::handle_auth_session: Sent Auth Response (Account country differs. Original country: %s new country: %s).", account.battle_net.lock_country.c_str(), ip_country_.c_str());
			SCRIPT_MGR->on_failed_account_login(account.game.id);
			delayed_close_socket();
			return;
		}
	}

	int64_t mutetime = account.game.mute_time;

	if(mutetime < 0)
	{
		mutetime = time(NULL) + llabs(mutetime);

		stmt = login_database.get_prepared_statement(LOGIN_UPD_MUTE_TIME_LOGIN);
		stmt->set_int64(0, mutetime);
		stmt->set_uint32(1, account.game.id);
		login_database.execute(stmt);
	}

	if(account.is_banned())
	{
		send_auth_response_error(ERROR_GAME_ACCOUNT_BANNED);
		LOG_ERROR("network", "world_socket::handle_auth_session: Sent Auth Response (Account banned).");
		SCRIPT_MGR->on_failed_account_login(account.game.id);
		delayed_close_socket();
		return;
	}

	account_types allowed_account_type = WORLD->get_player_security_limit();
	LOG_DEBUG("network", "Allowed Level: %u Player Level %u", allowed_account_type, account.game.security);
	if(allowed_account_type > SEC_PLAYER && account.game.security < allowed_account_type)
	{
		send_auth_response_error(ERROR_SERVER_IS_PRIVATE);
		LOG_DEBUG("network", "world_socket::handle_auth_session: User tries to login but his security level is not enough");
		SCRIPT_MGR->on_failed_account_login(account.game.id);
		delayed_close_socket();
		return;
	}

	LOG_DEBUG("network", "world_socket::handle_auth_session: Client '%s' authenticated successfully from %s.", auth_session->realm_join_ticket.c_str(), address.c_str());

	stmt = login_database.get_prepared_statement(LOGIN_UPD_LAST_IP);
	stmt->set_string(0, address);
	stmt->set_string(1, auth_session->realm_join_ticket);

	login_database.execute(stmt);

	SCRIPT_MGR->on_account_login(account.game.id);

	authed_ = true;
	world_session_ = new world_session(account.game.id, std::move(auth_session->realm_join_ticket), account.battle_net.id, shared_from_this(), account.game.security, account.game.expansion, mutetime, account.game.os, account.battle_net.locale, account.game.recruiter, account.game.is_rectuiter);

	if(warden_active)
		world_session_->init_warden(&session_key_);

	query_processor.add_query(world_session->load_permissions_async().with_prepared_callback(std::bind(&world_socket::load_session_permissions_callback, this, std::placeholders::_1)));
	async_read();
}

void world_socket::load_session_permissions_callback(prepared_query_result result)
{
    // RBAC must be loaded before adding session to check for skip queue permission
    world_session_->get_rbac_data()->load_from_db_callback(result);

    send_packet_and_log_opcode(*world_packets::auth::enable_encryption(encrypt_key_, true).write());
}

void world_socket::handle_auth_continued_session(std::shared_ptr<world_packets::auth::auth_continued_session> auth_session)
{
    world_session::connect_to_key key;
    key.raw = auth_session->key;

    type_ = connection_type(key.fields.connection_type);
    if (type_ != CONNECTION_TYPE_INSTANCE)
    {
        send_auth_response_error(ERROR_DENIED);
        delayed_close_socket();
        return;
    }

    uint32_t account_id = uint32_t(key.fields.account_id);
    login_database_prepared_statement* stmt = login_database.get_prepared_statement(LOGIN_SEL_ACCOUNT_INFO_CONTINUED_SESSION);
    stmt->set_uint32(0, account_id);

    query_processor_.add_query(login_database.async_query(stmt).with_prepared_callback(std::bind(&world_socket::handle_auth_continued_session_callback, this, auth_session, std::placeholders::_1)));
}

void world_socket::handle_auth_continued_session_callback(std::shared_ptr<world_packets::auth::auth_continued_session> auth_session, prepared_query_result result)
{
    if (!result)
    {
        send_auth_response_error(ERROR_DENIED);
        delayed_close_socket();
        return;
    }

    world_session::connect_to_key key;
    key_ = key.raw = auth_session->key;

    uint32_t account_id = uint32_t(key.fields.account_id);
    field* fields = result->fetch();
    std::string login = fields[0].get_string();
    session_key_.set_hex_str(fields[1].get_cstring());

    hmac_sha256 hmac(40, session_key_.as_byte_array(40).get());
    hmac.update_data(reinterpret_cast<uint8_t const*>(&auth_session->Key), sizeof(auth_session->Key));
    hmac.update_data(auth_session->local_challenge.data(), auth_session->local_challenge.size());
    hmac.update_data(server_challenge_.as_byte_array(16).get(), 16);
    hmac.update_data(continue_session_seed, 16);
    hmac.finalize();

    if (memcmp(hmac.get_digest(), auth_session->digest.data(), auth_session->digest.size()))
    {
        LOG_ERROR("network", "world_socket::handle_auth_continued_session: Authentication failed for account: %u ('%s') address: %s", account_id, login.c_str(), get_remote_ip_address().to_string().c_str());
        delayed_close_socket();
        return;
    }

    hmac_sha256 encrypt_key_gen(40, session_key_.as_byte_array(40).get());
    encrypt_key_gen.update_data(auth_session->local_challenge.data(), auth_session->local_challenge.size());
    encrypt_key_gen._update_data(server_challenge_.as_byte_array(16).get(), 16);
    encrypt_key_gen.update_data(encryption_key_seed, 16);
    encrypt_key_gen.finalize();

    // only first 16 bytes of the hmac are used
    memcpy(encrypt_key_, encrypt_key_gen.get_digest(), 16);

    send_packet_and_log_opcode(*world_packets::auth::enable_encryption(encrypt_key_, true).write());
    async_read();
}

void world_socket::handle_connect_to_failed(world_packets::auth::connect_to_failed& connect_to_failed)
{
    if (world_session_)
    {
        if (world_session_->player_loading())
        {
            switch (connect_to_failed.serial)
            {
                case world_packets::auth::connect_to_serial::world_attempt1:
                    world_session_->send_connect_to_instance(world_packets::auth::connect_to_serial::world_attempt2);
                    break;
                case world_packets::auth::connect_to_serial::world_attempt2:
                    world_session_->send_connect_to_instance(world_packets::auth::connect_to_serial::world_attempt3);
                    break;
                case world_packets::auth::connect_to_serial::world_attempt3:
                    world_session_->send_connect_to_instance(world_packets::auth::_connect_to_serial::world_attempt4);
                    break;
                case world_packets::auth::connect_to_serial::world_attempt4:
                    world_session_->send_connect_to_instance(world_packets::auth::connect_to_serial::world_attempt5);
                    break;
                case world_packets::auth::connect_to_serial::world_attempt5:
                {
                    LOG_ERROR("network", "%s failed to connect 5 times to world socket, aborting login", world_session_->get_player_info().c_str());
                    world_session_->abort_login(world_packets::character::login_failure_reason::no_world);
                    break;
                }
                default:
                    return;
            }
        }
        //else
        //{
        //    transfer_aborted when/if we get map node redirection
        //    send_packet_and_log_opcode(*world_packets::Auth::ResumeComms().write());
        //}
    }
}

void world_socket::handle_enable_encryption_ack()
{
    auth_crypt_.init(encrypt_key_);
    if (type_ == CONNECTION_TYPE_REALM)
        WORLD->add_session(world_session_);
    else
        WORLD->add_instance_socket(shared_from_this(), key_);
}

void world_socket::send_auth_response_error(uint32_t code)
{
    world_packets::auth::auth_response response;
    response.result = code;
    send_packet_and_log_opcode(*response.write());
}

bool world_socket::handle_ping(world_packets::auth::ping& ping)
{
    using namespace std::chrono;

    if (last_ping_time_ == steady_clock::time_point())
        last_ping_time_ = steady_clock::now();
    else
    {
        steady_clock::time_point now = steady_clock::now();

        steady_clock::duration diff = now - last_ping_time_;

        last_ping_time_ = now;

        if (diff < seconds(27))
        {
            ++over_speed_ping_;

            uint32_t maxAllowed = sWorld->getIntConfig(CONFIG_MAX_OVERSPEED_PINGS);

            if (maxAllowed && over_speed_ping_ > maxAllowed)
            {
                std::unique_lock<std::mutex> lock(world_session_lock_);

                if (world_session_ && !world_session_->HasPermission(rbac::RBAC_PERM_SKIP_CHECK_OVERSPEED_PING))
                {
                    LOG_ERROR("network", "world_socket::handle_ping: %s kicked for over-speed pings (address: %s)",
                        world_session_->GetPlayerInfo().c_str(), get_remote_ip_address().to_string().c_str());

                    return false;
                }
            }
        }
        else
            over_speed_ping_ = 0;
    }

    {
        std::lock_guard<std::mutex> lock(world_session_lock_);

        if (world_session_)
        {
            world_session_->SetLatency(ping.Latency);
            world_session_->ResetClientTimeDelay();
        }
        else
        {
            LOG_ERROR("network", "world_socket::handle_ping: peer sent CMSG_PING, but is not authenticated or got recently kicked, address = %s", get_remote_ip_address().to_string().c_str());
            return false;
        }
    }

    send_packet_and_log_opcode(*world_packets::Auth::Pong(ping.Serial).write());
    return true;
}
