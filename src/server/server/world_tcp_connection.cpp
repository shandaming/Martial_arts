/*
 * Copyright (C) 2020
 */

#include <zlib.h>

#include "world_tcp_connection.h"

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
	encryptable_packet(const world_packet& packet, bool encrypt) : world_packet(packet), encrypt_(encrypt) { }

	bool needs_encryption() const { return encrypt_; }
private:
	bool encrypt_;
};


const std::string world_tcp_connection::server_connection_initialize("WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT - V2");
const std::string world_tcp_connection::client_connection_initialize("WORLD OF WARCRAFT CONNECTION - CLIENT TO SERVER - V2");
constexpr uint32_t world_tcp_connection::min_size_for_compression = 0x400;

constexpr uint8_t world_tcp_connection::auth_check_seed[16] = { 0xC5, 0xC6, 0x98, 0x95, 0x76, 0x3F, 0x1D, 0xCD, 0xB6, 0xA1, 0x37, 0x28, 0xB3, 0x12, 0xFF, 0x8A };
constexpr uint8_t world_tcp_connection::session_key_seed[16] = { 0x58, 0xCB, 0xCF, 0x40, 0xFE, 0x2E, 0xCE, 0xA6, 0x5A, 0x90, 0xB8, 0x01, 0x68, 0x6C, 0x28, 0x0B };
constexpr uint8_t world_tcp_connection::continue_session_seed[16] = { 0x16, 0xAD, 0x0C, 0xD4, 0x46, 0xF9, 0x4F, 0xB2, 0xEF, 0x7D, 0xEA, 0x2A, 0x17, 0x66, 0x4D, 0x2F };
constexpr uint8_t world_tcp_connection::encryption_key_seed[16] = { 0xE9, 0x75, 0x3C, 0x50, 0x90, 0x93, 0x61, 0xDA, 0x3B, 0x07, 0xEE, 0xFA, 0xFF, 0x9D, 0x41, 0xB8 };

world_tcp_connection::world_tcp_connection(event_loop* loop, socket&& socket) : 
	tcp_connection(loop, socket) 
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

world_tcp_connection::~world_tcp_connection()
{
	if(cmpression_stream_)
	{
		deftate_end(compression_stream_);
		delete compression_stream_;
	}
}

void world_tcp_connection::start()
{
	std::string ip_address = get_remote_ip_address().to_string();
	login_database_prepared_statement* stmt = login_database.get_prepared_statement(LOGIN_SEL_IP_INFO);
	stmt->set_string(0, ip_address);
 query_processor_.add_query(login_database.async_query(stmt).with_prepared_callback(std::bind(&world_tcp_connection::check_ip_callback, this, std::placeholders::_1)));
}

void world_tcp_connection::check_ip_callback(prepared_query_result result)
{
	if(result)
	{
		bool banned = false;
		do
		{
			field* fields = result->Fetch();
			if (fields[0].GetUInt64() != 0)
				banned = true;

		}while (result->next_row());

		if(banned)
		{
			LOG_ERROR("network", "world_tcp_connection::check_ip_callback: Sent Auth Response (IP %s banned).", get_remote_ip_address().to_string().c_str());
			delayed_close_socket();
			return;
		}
	}

	packet_buffer_.resize(client_connection_initialize.length() + 1);

	//AsyncReadWithCallback(&world_tcp_connection::initialize_handler);
	set_initialize_handler_callback();

	message_buffer initializer;
	initializer.write(server_connection_initialize.c_str(), server_connection_initialize.length());
	initializer.write("\n", 1);

	// - IoContext.run thread, safe.
	queue_packet(std::move(initializer));
}

void world_tcp_connection::initialize_handler(std::error_code error, std::size_t transfered_bytes)
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
			size_t read_header_size = std::min(packet.get_active_size(), packet_buffer_.get_remaining_space());
			packet_buffer_.write(packet.get_read_pointer(), read_header_size);
			packet.read_completed(read_header_size);

			if (packet_buffer_.get_remaining_space() > 0)
			{
				// Couldn't receive the whole header this time.
				ASSERT(packet.get_active_size() == 0);
				//AsyncReadWithCallback(&world_tcp_connection::initialize_handler);
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
			int32_t z_res = deflateInit2(compression_stream_, sWorld->getIntConfig(CONFIG_COMPRESSION), Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
			if (z_res != Z_OK)
			{
				close_socket();
				TC_LOG_ERROR("network", "Can't initialize packet compression (zlib: deflateInit) error code: %i (%s)", z_res, zError(z_res));
				return;
			}

			packet_buffer_.Reset();
			handle_send_auth_session();
			//AsyncRead();
			set_read_handler_internal_callback();
			return;
		}
	}

	//AsyncReadWithCallback(&world_tcp_connection::initialize_handler);
	set_initialize_handler_callback();
}

bool world_tcp_connection::update()
{
    encryptable_packet* queued;
    message_buffer buffer(send_buffer_size_);
    while (buffer_queue_.dequeue(queued))
    {
        uint32_t packet_size = queued->size();
        if (packet_size > min_size_for_compression && queued->needs_encryption())
            packet_size = compress_bound(packet_size) + sizeof(compressed_world_packet);

        if (buffer.get_remaining_space() < packet_size + sizeof(packet_header))
        {
            queue_packet(std::move(buffer));
            buffer.resize(send_buffer_size_);
        }

        if (buffer.get_remaining_space() >= packet_size + sizeof(packet_header))
            write_packet_to_buffer(*queued, buffer);
        else    // single packet larger than 4096 bytes
        {
            message_buffer packet_buffer(packet_size + sizeof(packet_header));
            write_packet_to_buffer(*queued, packet_buffer);
            queue_packet(std::move(packet_buffer));
        }

        delete queued;
    }

    if (buffer.get_active_size() > 0)
        queue_packet(std::move(buffer));

    if (!BaseSocket::update())
        return false;

    query_processor_.process_ready_queries();

    return true;
}

void world_tcp_connection::handle_send_auth_session()
{
	big_number dos_challenge;
	dos_challenge.set_rand(32 * 8);

	world_packets::Auth::AuthChallenge challenge;
	memcpy(challenge.Challenge.data(), server_challenge_.AsByteArray(16).get(), 16);
	memcpy(challenge.DosChallenge.data(), dos_challenge.AsByteArray(32).get(), 32);
	challenge.DosZeroBits = 1;

	send_packet_and_log_opcode(*challenge.write());
}

void world_tcp_connection::on_close()
{
    {
        std::lock_guard<std::mutex> lock(world_session_lock_);
        world_session_ = nullptr;
    }
}

void world_tcp_connection::read_handler()
{
    if (!is_open())
        return;

    message_buffer& packet = get_read_buffer();
    while (packet.get_active_size() > 0)
    {
        if (header_buffer_.get_remaining_space() > 0)
        {
            // need to receive the header
            std::size_t read_header_size = std::min(packet.get_active_size(), header_buffer_.get_remaining_space());
            header_buffer_.write(packet.get_read_pointer(), read_header_size);
            packet.read_completed(read_header_size);

            if (header_buffer_.get_remaining_space() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.get_active_size() == 0);
                break;
            }

            // We just received nice new header
            if (!read_header_handler())
            {
                close_socket();
                return;
            }
        }

        // We have full read header, now check the data payload
        if (packet_buffer_.get_remaining_space() > 0)
        {
            // need more data in the payload
            std::size_t read_data_size = std::min(packet.get_active_size(), packet_buffer_.get_remaining_space());
            packet_buffer_.write(packet.get_read_pointer(), read_data_size);
            packet.read_completed(read_data_size);

            if (packet_buffer_.get_remaining_space() > 0)
            {
                // Couldn't receive the whole data this time.
                ASSERT(packet.get_active_size() == 0);
                break;
            }
        }

        // just received fresh new payload
        read_data_handler_result result = read_data_handler();
        header_buffer_.Reset();
        if (result != read_data_handler_result::ok)
        {
            if (result != read_data_handler_result::waiting_for_query)
                close_socket();

            return;
        }
    }

    //AsyncRead();
	set_read_handler_internal_callback();
}

void world_tcp_connection::set_world_session(world_session* session)
{
    std::lock_guard<std::mutex> lock(world_session_lock_);
    world_session_ = session;
    authed_ = true;
}

bool world_tcp_connection::read_header_handler()
{
    ASSERT(header_buffer_.get_active_size() == sizeof(packet_header), "Header size " SZFMTD " different than expected " SZFMTD, header_buffer_.get_active_size(), sizeof(packet_header));

    packet_header* header = reinterpret_cast<packet_header*>(header_buffer_.get_read_pointer());

    if (!header->is_valid_size())
    {
        TC_LOG_ERROR("network", "world_tcp_connection::read_header_handler(): client %s sent malformed packet (size: %u)",
            get_remote_ip_address().to_string().c_str(), header->Size);
        return false;
    }

    packet_buffer_.resize(header->Size);
    return true;
}

world_tcp_connection::read_data_handler_result world_tcp_connection::read_data_handler()
{
    packet_header* header = reinterpret_cast<packet_header*>(header_buffer_.get_read_pointer());

    if (!auth_crypt_.DecryptRecv(packet_buffer_.get_read_pointer(), header->Size, header->Tag))
    {
        TC_LOG_ERROR("network", "world_tcp_connection::read_header_handler(): client %s failed to decrypt packet (size: %u)",
            get_remote_ip_address().to_string().c_str(), header->Size);
        return read_data_handler_result::error;
    }

    world_packet packet(std::move(packet_buffer_), GetConnectionType());
    OpcodeClient opcode = packet.read<OpcodeClient>();
    if (uint32_t(opcode) >= uint32_t(NUM_OPCODE_HANDLERS))
    {
        TC_LOG_ERROR("network", "world_tcp_connection::read_header_handler(): client %s sent wrong opcode (opcode: %u)",
            get_remote_ip_address().to_string().c_str(), uint32_t(opcode));
        return read_data_handler_result::error;
    }

    packet.SetOpcode(opcode);

    if (sPacketLog->can_log_packet())
        sPacketLog->log_packet(packet, CLIENT_TO_SERVER, get_remote_ip_address(), get_remote_port(), GetConnectionType());

    std::unique_lock<std::mutex> lock(world_session_lock_, std::defer_lock);

    switch (opcode)
    {
        case CMSG_PING:
        {
            log_opcode_text(opcode, lock);
            world_packets::Auth::Ping ping(std::move(packet));
            if (!ping.ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::read_data_handler(): client %s sent malformed CMSG_PING", get_remote_ip_address().to_string().c_str());
                return read_data_handler_result::error;
            }
            if (!handle_ping(ping))
                return read_data_handler_result::error;
            break;
        }
        case CMSG_AUTH_SESSION:
        {
            log_opcode_text(opcode, lock);
            if (authed_)
            {
                // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                if (lock.try_lock())
                    TC_LOG_ERROR("network", "world_tcp_connection::ProcessIncoming: received duplicate CMSG_AUTH_SESSION from %s", world_session_->GetPlayerInfo().c_str());
                return read_data_handler_result::error;
            }

            std::shared_ptr<world_packets::Auth::AuthSession> authSession = std::make_shared<world_packets::Auth::AuthSession>(std::move(packet));
            if (!authSession->ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::read_data_handler(): client %s sent malformed CMSG_AUTH_SESSION", get_remote_ip_address().to_string().c_str());
                return read_data_handler_result::error;
            }
            HandleAuthSession(authSession);
            return read_data_handler_result::waiting_for_query;
        }
        case CMSG_AUTH_CONTINUED_SESSION:
        {
            log_opcode_text(opcode, lock);
            if (authed_)
            {
                // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                if (lock.try_lock())
                    TC_LOG_ERROR("network", "world_tcp_connection::ProcessIncoming: received duplicate CMSG_AUTH_CONTINUED_SESSION from %s", world_session_->GetPlayerInfo().c_str());
                return read_data_handler_result::error;
            }

            std::shared_ptr<world_packets::Auth::AuthContinuedSession> authSession = std::make_shared<world_packets::Auth::AuthContinuedSession>(std::move(packet));
            if (!authSession->ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::read_data_handler(): client %s sent malformed CMSG_AUTH_CONTINUED_SESSION", get_remote_ip_address().to_string().c_str());
                return read_data_handler_result::error;
            }
            HandleAuthContinuedSession(authSession);
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
            SetNoDelay(false);
            break;
        case CMSG_CONNECT_TO_FAILED:
        {
            lock.lock();

            log_opcode_text(opcode, lock);
            world_packets::Auth::ConnectToFailed connectToFailed(std::move(packet));
            if (!connectToFailed.ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::read_data_handler(): client %s sent malformed CMSG_CONNECT_TO_FAILED", get_remote_ip_address().to_string().c_str());
                return read_data_handler_result::error;
            }
            HandleConnectToFailed(connectToFailed);
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

            if (!world_session_)
            {
                TC_LOG_ERROR("network.opcode", "ProcessIncoming: Client not authed opcode = %u", uint32_t(opcode));
                return read_data_handler_result::error;
            }

            OpcodeHandler const* handler = opcodeTable[opcode];
            if (!handler)
            {
                TC_LOG_ERROR("network.opcode", "No defined handler for opcode %s sent by %s", get_opcode_name_for_logging(static_cast<OpcodeClient>(packet.get_opcode())).c_str(), world_session_->GetPlayerInfo().c_str());
                break;
            }

            // Our Idle timer will reset on any non PING opcodes.
            // Catches people idling on the login screen and any lingering ingame connections.
            world_session_->ResetTimeOutTime();

            // Copy the packet to the heap before enqueuing
            world_session_->queue_packet(new world_packet(std::move(packet)));
            break;
        }
    }

    return read_data_handler_result::ok;
}

void world_tcp_connection::log_opcode_text(OpcodeClient opcode, std::unique_lock<std::mutex> const& guard) const
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

void world_tcp_connection::send_packet_and_log_opcode(world_packet const& packet)
{
    TC_LOG_TRACE("network.opcode", "S->C: %s %s", get_remote_ip_address().to_string().c_str(), get_opcode_name_for_logging(static_cast<opcode_server>(packet.get_opcode())).c_str());
    send_packet(packet);
}

void world_tcp_connection::send_packet(world_packet const& packet)
{
    if (!is_open())
        return;

    if (sPacketLog->can_log_packet())
        sPacketLog->log_packet(packet, SERVER_TO_CLIENT, get_remote_ip_address(), get_remote_port(), GetConnectionType());

    buffer_queue_.enqueue(new encryptable_packet(packet, auth_crypt_.IsInitialized()));
}

void world_tcp_connection::write_packet_to_buffer(encryptable_packet const& packet, message_buffer& buffer)
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

uint32_t world_tcp_connection::CompressPacket(uint8_t* buffer, world_packet const& packet)
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
        TC_LOG_ERROR("network", "Can't compress packet opcode (zlib: deflate) error code: %i (%s, msg: %s)", z_res, zError(z_res), compression_stream_->msg);
        return 0;
    }

    compression_stream_->next_in = (Bytef*)packet.contents();
    compression_stream_->avail_in = packet.size();

    z_res = deflate(compression_stream_, Z_SYNC_FLUSH);
    if (z_res != Z_OK)
    {
        TC_LOG_ERROR("network", "Can't compress packet data (zlib: deflate) error code: %i (%s, msg: %s)", z_res, zError(z_res), compression_stream_->msg);
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

void world_tcp_connection::HandleAuthSession(std::shared_ptr<world_packets::Auth::AuthSession> authSession)
{
    // Get the account information from the auth database
    LoginDatabasePreparedStatement* stmt = login_database.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME);
    stmt->setInt32(0, int32(realm.Id.Realm));
    stmt->setString(1, authSession->RealmJoinTicket);

    query_processor_.AddQuery(login_database.async_query(stmt).with_prepared_callback(std::bind(&world_tcp_connection::HandleAuthSessionCallback, this, authSession, std::placeholders::_1)));
}

void world_tcp_connection::HandleAuthSessionCallback(std::shared_ptr<world_packets::Auth::AuthSession> authSession, prepared_query_result result)
{
    // Stop if the account is not found
    if (!result)
    {
        // We can not log here, as we do not know the account. Thus, no accountId.
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Sent Auth Response (unknown account).");
        delayed_close_socket();
        return;
    }

    RealmBuildInfo const* buildInfo = sRealmList->GetBuildInfo(realm.Build);
    if (!buildInfo)
    {
        SendAuthResponseError(ERROR_BAD_VERSION);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Missing auth seed for realm build %u (%s).", realm.Build, get_remote_ip_address().to_string().c_str());
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
    hmac.UpdateData(server_challenge_.AsByteArray(16).get(), 16);
    hmac.UpdateData(auth_check_seed, 16);
    hmac.Finalize();

    // Check that Key and account name are the same on client and server
    if (memcmp(hmac.GetDigest(), authSession->Digest.data(), authSession->Digest.size()) != 0)
    {
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Authentication failed for account: %u ('%s') address: %s", account.Game.Id, authSession->RealmJoinTicket.c_str(), address.c_str());
        delayed_close_socket();
        return;
    }

    SHA256Hash keyData;
    keyData.UpdateData(account.Game.KeyData.data(), account.Game.KeyData.size());
    keyData.Finalize();

    HmacSha256 sessionKeyHmac(keyData.GetLength(), keyData.GetDigest());
    sessionKeyHmac.UpdateData(server_challenge_.AsByteArray(16).get(), 16);
    sessionKeyHmac.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    sessionKeyHmac.UpdateData(session_key_seed, 16);
    sessionKeyHmac.Finalize();

    uint8_t sessionKey[40];
    SessionKeyGenerator<SHA256Hash> sessionKeyGenerator(sessionKeyHmac.GetDigest(), sessionKeyHmac.GetLength());
    sessionKeyGenerator.Generate(sessionKey, 40);

    _sessionKey.SetBinary(sessionKey, 40);

    HmacSha256 encryptKeyGen(40, sessionKey);
    encryptKeyGen.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    encryptKeyGen.UpdateData(server_challenge_.AsByteArray(16).get(), 16);
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
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: World closed, denying client (%s).", get_remote_ip_address().to_string().c_str());
        delayed_close_socket();
        return;
    }

    if (authSession->RealmID != realm.Id.Realm)
    {
        SendAuthResponseError(ERROR_DENIED);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Client %s requested connecting with realm id %u but this realm has id %u set in config.",
            get_remote_ip_address().to_string().c_str(), authSession->RealmID, realm.Id.Realm);
        delayed_close_socket();
        return;
    }

    // Must be done before world_session is created
    bool wardenActive = sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED);
    if (wardenActive && account.Game.OS != "Win" && account.Game.OS != "Wn64" && account.Game.OS != "Mc64")
    {
        SendAuthResponseError(ERROR_DENIED);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Client %s attempted to log in using invalid client OS (%s).", address.c_str(), account.Game.OS.c_str());
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
            TC_LOG_DEBUG("network", "world_tcp_connection::HandleAuthSession: Sent Auth Response (Account IP differs. Original IP: %s, new IP: %s).", account.BattleNet.LastIP.c_str(), address.c_str());
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
            TC_LOG_DEBUG("network", "world_tcp_connection::HandleAuthSession: Sent Auth Response (Account country differs. Original country: %s, new country: %s).", account.BattleNet.LockCountry.c_str(), _ipCountry.c_str());
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
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Sent Auth Response (Account banned).");
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
        TC_LOG_DEBUG("network", "world_tcp_connection::HandleAuthSession: User tries to login but his security level is not enough");
        sScriptMgr->OnFailedAccountLogin(account.Game.Id);
        delayed_close_socket();
        return;
    }

    TC_LOG_DEBUG("network", "world_tcp_connection::HandleAuthSession: Client '%s' authenticated successfully from %s.", authSession->RealmJoinTicket.c_str(), address.c_str());

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

    query_processor_.AddQuery(world_session_->LoadPermissionsAsync().with_prepared_callback(std::bind(&world_tcp_connection::LoadSessionPermissionsCallback, this, std::placeholders::_1)));
    AsyncRead();
}

void world_tcp_connection::LoadSessionPermissionsCallback(prepared_query_result result)
{
    // RBAC must be loaded before adding session to check for skip queue permission
    world_session_->GetRBACData()->LoadFromDBCallback(result);

    send_packet_and_log_opcode(*world_packets::Auth::EnableEncryption(encrypt_key_, true).write());
}

void world_tcp_connection::HandleAuthContinuedSession(std::shared_ptr<world_packets::Auth::AuthContinuedSession> authSession)
{
    world_session::ConnectToKey key;
    key.Raw = authSession->Key;

    type_ = ConnectionType(key.Fields.ConnectionType);
    if (type_ != CONNECTION_TYPE_INSTANCE)
    {
        SendAuthResponseError(ERROR_DENIED);
        delayed_close_socket();
        return;
    }

    uint32_t accountId = uint32_t(key.Fields.AccountId);
    LoginDatabasePreparedStatement* stmt = login_database.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_CONTINUED_SESSION);
    stmt->setUInt32(0, accountId);

    query_processor_.AddQuery(login_database.async_query(stmt).with_prepared_callback(std::bind(&world_tcp_connection::HandleAuthContinuedSessionCallback, this, authSession, std::placeholders::_1)));
}

void world_tcp_connection::HandleAuthContinuedSessionCallback(std::shared_ptr<world_packets::Auth::AuthContinuedSession> authSession, prepared_query_result result)
{
    if (!result)
    {
        SendAuthResponseError(ERROR_DENIED);
        delayed_close_socket();
        return;
    }

    world_session::ConnectToKey key;
    key_ = key.Raw = authSession->Key;

    uint32_t accountId = uint32_t(key.Fields.AccountId);
    field* fields = result->Fetch();
    std::string login = fields[0].GetString();
    _sessionKey.SetHexStr(fields[1].GetCString());

    HmacSha256 hmac(40, _sessionKey.AsByteArray(40).get());
    hmac.UpdateData(reinterpret_cast<uint8_t const*>(&authSession->Key), sizeof(authSession->Key));
    hmac.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    hmac.UpdateData(server_challenge_.AsByteArray(16).get(), 16);
    hmac.UpdateData(continue_session_seed, 16);
    hmac.Finalize();

    if (memcmp(hmac.GetDigest(), authSession->Digest.data(), authSession->Digest.size()))
    {
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthContinuedSession: Authentication failed for account: %u ('%s') address: %s", accountId, login.c_str(), get_remote_ip_address().to_string().c_str());
        delayed_close_socket();
        return;
    }

    HmacSha256 encryptKeyGen(40, _sessionKey.AsByteArray(40).get());
    encryptKeyGen.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    encryptKeyGen.UpdateData(server_challenge_.AsByteArray(16).get(), 16);
    encryptKeyGen.UpdateData(encryption_key_seed, 16);
    encryptKeyGen.Finalize();

    // only first 16 bytes of the hmac are used
    memcpy(encrypt_key_, encryptKeyGen.GetDigest(), 16);

    send_packet_and_log_opcode(*world_packets::Auth::EnableEncryption(encrypt_key_, true).write());
    AsyncRead();
}

void world_tcp_connection::HandleConnectToFailed(world_packets::Auth::ConnectToFailed& connectToFailed)
{
    if (world_session_)
    {
        if (world_session_->PlayerLoading())
        {
            switch (connectToFailed.Serial)
            {
                case world_packets::Auth::ConnectToSerial::WorldAttempt1:
                    world_session_->SendConnectToInstance(world_packets::Auth::ConnectToSerial::WorldAttempt2);
                    break;
                case world_packets::Auth::ConnectToSerial::WorldAttempt2:
                    world_session_->SendConnectToInstance(world_packets::Auth::ConnectToSerial::WorldAttempt3);
                    break;
                case world_packets::Auth::ConnectToSerial::WorldAttempt3:
                    world_session_->SendConnectToInstance(world_packets::Auth::ConnectToSerial::WorldAttempt4);
                    break;
                case world_packets::Auth::ConnectToSerial::WorldAttempt4:
                    world_session_->SendConnectToInstance(world_packets::Auth::ConnectToSerial::WorldAttempt5);
                    break;
                case world_packets::Auth::ConnectToSerial::WorldAttempt5:
                {
                    TC_LOG_ERROR("network", "%s failed to connect 5 times to world socket, aborting login", world_session_->GetPlayerInfo().c_str());
                    world_session_->AbortLogin(world_packets::Character::LoginFailureReason::NoWorld);
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

void world_tcp_connection::handle_enable_encryption_ack()
{
    auth_crypt_.Init(encrypt_key_);
    if (type_ == CONNECTION_TYPE_REALM)
        sWorld->AddSession(world_session_);
    else
        sWorld->AddInstanceSocket(shared_from_this(), key_);
}

void world_tcp_connection::SendAuthResponseError(uint32_t code)
{
    world_packets::Auth::AuthResponse response;
    response.Result = code;
    send_packet_and_log_opcode(*response.write());
}

bool world_tcp_connection::handle_ping(world_packets::Auth::Ping& ping)
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
                    TC_LOG_ERROR("network", "world_tcp_connection::handle_ping: %s kicked for over-speed pings (address: %s)",
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
            TC_LOG_ERROR("network", "world_tcp_connection::handle_ping: peer sent CMSG_PING, but is not authenticated or got recently kicked, address = %s", get_remote_ip_address().to_string().c_str());
            return false;
        }
    }

    send_packet_and_log_opcode(*world_packets::Auth::Pong(ping.Serial).write());
    return true;
}
