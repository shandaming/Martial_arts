/*
 * Copyright (C) 2020
 */

#include <zlib.h>

#include "world_tcp_connection.h"

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
    server_challenge_.SetRand(8 * 16);
    memset(encrypt_key_, 0, sizeof(encrypt_key_));
    header_buffer_.Resize(sizeof(PacketHeader));
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

    _queryProcessor.add_query(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&world_tcp_connection::CheckIpCallback, this, std::placeholders::_1)));
}

void world_tcp_connection::CheckIpCallback(PreparedQueryResult result)
{
    if (result)
    {
        bool banned = false;
        do
        {
            Field* fields = result->Fetch();
            if (fields[0].GetUInt64() != 0)
                banned = true;

        } while (result->NextRow());

        if (banned)
        {
            TC_LOG_ERROR("network", "world_tcp_connection::CheckIpCallback: Sent Auth Response (IP %s banned).", GetRemoteIpAddress().to_string().c_str());
            DelayedCloseSocket();
            return;
        }
    }

    packet_buffer_.Resize(client_connection_initialize.length() + 1);

    //AsyncReadWithCallback(&world_tcp_connection::InitializeHandler);
set_initializeHandler_callback();

    message_buffer initializer;
    initializer.Write(server_connection_initialize.c_str(), server_connection_initialize.length());
    initializer.Write("\n", 1);

    // - IoContext.run thread, safe.
    QueuePacket(std::move(initializer));
}

void world_tcp_connection::InitializeHandler(std::error_code error, std::size_t transferedBytes)
{
    if (error)
    {
        CloseSocket();
        return;
    }

    GetReadBuffer().WriteCompleted(transferedBytes);

    message_buffer& packet = GetReadBuffer();
    if (packet.GetActiveSize() > 0)
    {
        if (packet_buffer_.GetRemainingSpace() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(packet.GetActiveSize(), packet_buffer_.GetRemainingSpace());
            packet_buffer_.Write(packet.GetReadPointer(), readHeaderSize);
            packet.ReadCompleted(readHeaderSize);

            if (packet_buffer_.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.GetActiveSize() == 0);
                AsyncReadWithCallback(&world_tcp_connection::InitializeHandler);
                return;
            }

            ByteBuffer buffer(std::move(packet_buffer_));
            std::string initializer = buffer.ReadString(client_connection_initialize.length());
            if (initializer != client_connection_initialize)
            {
                CloseSocket();
                return;
            }

            uint8_t terminator;
            buffer >> terminator;
            if (terminator != '\n')
            {
                CloseSocket();
                return;
            }

            compression_stream_ = new z_stream();
            compression_stream_->zalloc = (alloc_func)NULL;
            compression_stream_->zfree = (free_func)NULL;
            compression_stream_->opaque = (voidpf)NULL;
            compression_stream_->avail_in = 0;
            compression_stream_->next_in = NULL;
            int32 z_res = deflateInit2(compression_stream_, sWorld->getIntConfig(CONFIG_COMPRESSION), Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
            if (z_res != Z_OK)
            {
                CloseSocket();
                TC_LOG_ERROR("network", "Can't initialize packet compression (zlib: deflateInit) Error code: %i (%s)", z_res, zError(z_res));
                return;
            }

            packet_buffer_.Reset();
            HandleSendAuthSession();
            //AsyncRead();
set_read_handler_internal_callback();
            return;
        }
    }

    AsyncReadWithCallback(&world_tcp_connection::InitializeHandler);
}

bool world_tcp_connection::Update()
{
    EncryptablePacket* queued;
    message_buffer buffer(send_buffer_size_);
    while (_bufferQueue.Dequeue(queued))
    {
        uint32_t packetSize = queued->size();
        if (packetSize > min_size_for_compression && queued->NeedsEncryption())
            packetSize = compressBound(packetSize) + sizeof(CompressedWorldPacket);

        if (buffer.GetRemainingSpace() < packetSize + sizeof(PacketHeader))
        {
            QueuePacket(std::move(buffer));
            buffer.Resize(send_buffer_size_);
        }

        if (buffer.GetRemainingSpace() >= packetSize + sizeof(PacketHeader))
            WritePacketToBuffer(*queued, buffer);
        else    // single packet larger than 4096 bytes
        {
            message_buffer packetBuffer(packetSize + sizeof(PacketHeader));
            WritePacketToBuffer(*queued, packetBuffer);
            QueuePacket(std::move(packetBuffer));
        }

        delete queued;
    }

    if (buffer.GetActiveSize() > 0)
        QueuePacket(std::move(buffer));

    if (!BaseSocket::Update())
        return false;

    _queryProcessor.ProcessReadyQueries();

    return true;
}

void world_tcp_connection::HandleSendAuthSession()
{
    BigNumber dosChallenge;
    dosChallenge.SetRand(32 * 8);

    WorldPackets::Auth::AuthChallenge challenge;
    memcpy(challenge.Challenge.data(), server_challenge_.AsByteArray(16).get(), 16);
    memcpy(challenge.DosChallenge.data(), dosChallenge.AsByteArray(32).get(), 32);
    challenge.DosZeroBits = 1;

    SendPacketAndLogOpcode(*challenge.Write());
}

void world_tcp_connection::OnClose()
{
    {
        std::lock_guard<std::mutex> sessionGuard(_worldSessionLock);
        world_session_ = nullptr;
    }
}

void world_tcp_connection::ReadHandler()
{
    if (!IsOpen())
        return;

    message_buffer& packet = GetReadBuffer();
    while (packet.GetActiveSize() > 0)
    {
        if (header_buffer_.GetRemainingSpace() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(packet.GetActiveSize(), header_buffer_.GetRemainingSpace());
            header_buffer_.Write(packet.GetReadPointer(), readHeaderSize);
            packet.ReadCompleted(readHeaderSize);

            if (header_buffer_.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }

            // We just received nice new header
            if (!ReadHeaderHandler())
            {
                CloseSocket();
                return;
            }
        }

        // We have full read header, now check the data payload
        if (packet_buffer_.GetRemainingSpace() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(packet.GetActiveSize(), packet_buffer_.GetRemainingSpace());
            packet_buffer_.Write(packet.GetReadPointer(), readDataSize);
            packet.ReadCompleted(readDataSize);

            if (packet_buffer_.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole data this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }
        }

        // just received fresh new payload
        ReadDataHandlerResult result = ReadDataHandler();
        header_buffer_.Reset();
        if (result != ReadDataHandlerResult::Ok)
        {
            if (result != ReadDataHandlerResult::WaitingForQuery)
                CloseSocket();

            return;
        }
    }

    AsyncRead();
}

void world_tcp_connection::SetWorldSession(WorldSession* session)
{
    std::lock_guard<std::mutex> sessionGuard(_worldSessionLock);
    world_session_ = session;
    authed_ = true;
}

bool world_tcp_connection::ReadHeaderHandler()
{
    ASSERT(header_buffer_.GetActiveSize() == sizeof(PacketHeader), "Header size " SZFMTD " different than expected " SZFMTD, header_buffer_.GetActiveSize(), sizeof(PacketHeader));

    PacketHeader* header = reinterpret_cast<PacketHeader*>(header_buffer_.GetReadPointer());

    if (!header->IsValidSize())
    {
        TC_LOG_ERROR("network", "world_tcp_connection::ReadHeaderHandler(): client %s sent malformed packet (size: %u)",
            GetRemoteIpAddress().to_string().c_str(), header->Size);
        return false;
    }

    packet_buffer_.Resize(header->Size);
    return true;
}

world_tcp_connection::ReadDataHandlerResult world_tcp_connection::ReadDataHandler()
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(header_buffer_.GetReadPointer());

    if (!_authCrypt.DecryptRecv(packet_buffer_.GetReadPointer(), header->Size, header->Tag))
    {
        TC_LOG_ERROR("network", "world_tcp_connection::ReadHeaderHandler(): client %s failed to decrypt packet (size: %u)",
            GetRemoteIpAddress().to_string().c_str(), header->Size);
        return ReadDataHandlerResult::Error;
    }

    WorldPacket packet(std::move(packet_buffer_), GetConnectionType());
    OpcodeClient opcode = packet.read<OpcodeClient>();
    if (uint32_t(opcode) >= uint32_t(NUM_OPCODE_HANDLERS))
    {
        TC_LOG_ERROR("network", "world_tcp_connection::ReadHeaderHandler(): client %s sent wrong opcode (opcode: %u)",
            GetRemoteIpAddress().to_string().c_str(), uint32_t(opcode));
        return ReadDataHandlerResult::Error;
    }

    packet.SetOpcode(opcode);

    if (sPacketLog->CanLogPacket())
        sPacketLog->LogPacket(packet, CLIENT_TO_SERVER, GetRemoteIpAddress(), GetRemotePort(), GetConnectionType());

    std::unique_lock<std::mutex> sessionGuard(_worldSessionLock, std::defer_lock);

    switch (opcode)
    {
        case CMSG_PING:
        {
            LogOpcodeText(opcode, sessionGuard);
            WorldPackets::Auth::Ping ping(std::move(packet));
            if (!ping.ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::ReadDataHandler(): client %s sent malformed CMSG_PING", GetRemoteIpAddress().to_string().c_str());
                return ReadDataHandlerResult::Error;
            }
            if (!HandlePing(ping))
                return ReadDataHandlerResult::Error;
            break;
        }
        case CMSG_AUTH_SESSION:
        {
            LogOpcodeText(opcode, sessionGuard);
            if (authed_)
            {
                // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                if (sessionGuard.try_lock())
                    TC_LOG_ERROR("network", "world_tcp_connection::ProcessIncoming: received duplicate CMSG_AUTH_SESSION from %s", world_session_->GetPlayerInfo().c_str());
                return ReadDataHandlerResult::Error;
            }

            std::shared_ptr<WorldPackets::Auth::AuthSession> authSession = std::make_shared<WorldPackets::Auth::AuthSession>(std::move(packet));
            if (!authSession->ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::ReadDataHandler(): client %s sent malformed CMSG_AUTH_SESSION", GetRemoteIpAddress().to_string().c_str());
                return ReadDataHandlerResult::Error;
            }
            HandleAuthSession(authSession);
            return ReadDataHandlerResult::WaitingForQuery;
        }
        case CMSG_AUTH_CONTINUED_SESSION:
        {
            LogOpcodeText(opcode, sessionGuard);
            if (authed_)
            {
                // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                if (sessionGuard.try_lock())
                    TC_LOG_ERROR("network", "world_tcp_connection::ProcessIncoming: received duplicate CMSG_AUTH_CONTINUED_SESSION from %s", world_session_->GetPlayerInfo().c_str());
                return ReadDataHandlerResult::Error;
            }

            std::shared_ptr<WorldPackets::Auth::AuthContinuedSession> authSession = std::make_shared<WorldPackets::Auth::AuthContinuedSession>(std::move(packet));
            if (!authSession->ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::ReadDataHandler(): client %s sent malformed CMSG_AUTH_CONTINUED_SESSION", GetRemoteIpAddress().to_string().c_str());
                return ReadDataHandlerResult::Error;
            }
            HandleAuthContinuedSession(authSession);
            return ReadDataHandlerResult::WaitingForQuery;
        }
        case CMSG_KEEP_ALIVE:
            LogOpcodeText(opcode, sessionGuard);
            break;
        case CMSG_LOG_DISCONNECT:
            LogOpcodeText(opcode, sessionGuard);
            packet.rfinish();   // contains uint32_t disconnectReason;
            break;
        case CMSG_ENABLE_NAGLE:
            LogOpcodeText(opcode, sessionGuard);
            SetNoDelay(false);
            break;
        case CMSG_CONNECT_TO_FAILED:
        {
            sessionGuard.lock();

            LogOpcodeText(opcode, sessionGuard);
            WorldPackets::Auth::ConnectToFailed connectToFailed(std::move(packet));
            if (!connectToFailed.ReadNoThrow())
            {
                TC_LOG_ERROR("network", "world_tcp_connection::ReadDataHandler(): client %s sent malformed CMSG_CONNECT_TO_FAILED", GetRemoteIpAddress().to_string().c_str());
                return ReadDataHandlerResult::Error;
            }
            HandleConnectToFailed(connectToFailed);
            break;
        }
        case CMSG_ENABLE_ENCRYPTION_ACK:
            LogOpcodeText(opcode, sessionGuard);
            HandleEnableEncryptionAck();
            break;
        default:
        {
            sessionGuard.lock();

            LogOpcodeText(opcode, sessionGuard);

            if (!world_session_)
            {
                TC_LOG_ERROR("network.opcode", "ProcessIncoming: Client not authed opcode = %u", uint32_t(opcode));
                return ReadDataHandlerResult::Error;
            }

            OpcodeHandler const* handler = opcodeTable[opcode];
            if (!handler)
            {
                TC_LOG_ERROR("network.opcode", "No defined handler for opcode %s sent by %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet.GetOpcode())).c_str(), world_session_->GetPlayerInfo().c_str());
                break;
            }

            // Our Idle timer will reset on any non PING opcodes.
            // Catches people idling on the login screen and any lingering ingame connections.
            world_session_->ResetTimeOutTime();

            // Copy the packet to the heap before enqueuing
            world_session_->QueuePacket(new WorldPacket(std::move(packet)));
            break;
        }
    }

    return ReadDataHandlerResult::Ok;
}

void world_tcp_connection::LogOpcodeText(OpcodeClient opcode, std::unique_lock<std::mutex> const& guard) const
{
    if (!guard)
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", GetRemoteIpAddress().to_string().c_str(), GetOpcodeNameForLogging(opcode).c_str());
    }
    else
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", (world_session_ ? world_session_->GetPlayerInfo() : GetRemoteIpAddress().to_string()).c_str(),
            GetOpcodeNameForLogging(opcode).c_str());
    }
}

void world_tcp_connection::SendPacketAndLogOpcode(WorldPacket const& packet)
{
    TC_LOG_TRACE("network.opcode", "S->C: %s %s", GetRemoteIpAddress().to_string().c_str(), GetOpcodeNameForLogging(static_cast<OpcodeServer>(packet.GetOpcode())).c_str());
    SendPacket(packet);
}

void world_tcp_connection::SendPacket(WorldPacket const& packet)
{
    if (!IsOpen())
        return;

    if (sPacketLog->CanLogPacket())
        sPacketLog->LogPacket(packet, SERVER_TO_CLIENT, GetRemoteIpAddress(), GetRemotePort(), GetConnectionType());

    _bufferQueue.Enqueue(new EncryptablePacket(packet, _authCrypt.IsInitialized()));
}

void world_tcp_connection::WritePacketToBuffer(EncryptablePacket const& packet, message_buffer& buffer)
{
    uint16 opcode = packet.GetOpcode();
    uint32_t packetSize = packet.size();

    // Reserve space for buffer
    uint8_t* headerPos = buffer.GetWritePointer();
    buffer.WriteCompleted(sizeof(PacketHeader));
    uint8_t* dataPos = buffer.GetWritePointer();
    buffer.WriteCompleted(sizeof(opcode));

    if (packetSize > min_size_for_compression && packet.NeedsEncryption())
    {
        CompressedWorldPacket cmp;
        cmp.UncompressedSize = packetSize + 2;
        cmp.UncompressedAdler = adler32(adler32(0x9827D8F1, (Bytef*)&opcode, 2), packet.contents(), packetSize);

        // Reserve space for compression info - uncompressed size and checksums
        uint8_t* compressionInfo = buffer.GetWritePointer();
        buffer.WriteCompleted(sizeof(CompressedWorldPacket));

        uint32_t compressedSize = CompressPacket(buffer.GetWritePointer(), packet);

        cmp.CompressedAdler = adler32(0x9827D8F1, buffer.GetWritePointer(), compressedSize);

        memcpy(compressionInfo, &cmp, sizeof(CompressedWorldPacket));
        buffer.WriteCompleted(compressedSize);
        packetSize = compressedSize + sizeof(CompressedWorldPacket);

        opcode = SMSG_COMPRESSED_PACKET;
    }
    else if (!packet.empty())
        buffer.Write(packet.contents(), packet.size());

    memcpy(dataPos, &opcode, sizeof(opcode));
    packetSize += 2 /*opcode*/;

    PacketHeader header;
    header.Size = packetSize;
    _authCrypt.EncryptSend(dataPos, header.Size, header.Tag);

    memcpy(headerPos, &header, sizeof(PacketHeader));
}

uint32_t world_tcp_connection::CompressPacket(uint8_t* buffer, WorldPacket const& packet)
{
    uint32_t opcode = packet.GetOpcode();
    uint32_t bufferSize = deflateBound(compression_stream_, packet.size() + sizeof(uint16));

    compression_stream_->next_out = buffer;
    compression_stream_->avail_out = bufferSize;
    compression_stream_->next_in = (Bytef*)&opcode;
    compression_stream_->avail_in = sizeof(uint16);

    int32 z_res = deflate(compression_stream_, Z_NO_FLUSH);
    if (z_res != Z_OK)
    {
        TC_LOG_ERROR("network", "Can't compress packet opcode (zlib: deflate) Error code: %i (%s, msg: %s)", z_res, zError(z_res), compression_stream_->msg);
        return 0;
    }

    compression_stream_->next_in = (Bytef*)packet.contents();
    compression_stream_->avail_in = packet.size();

    z_res = deflate(compression_stream_, Z_SYNC_FLUSH);
    if (z_res != Z_OK)
    {
        TC_LOG_ERROR("network", "Can't compress packet data (zlib: deflate) Error code: %i (%s, msg: %s)", z_res, zError(z_res), compression_stream_->msg);
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

    explicit AccountInfo(Field* fields)
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

void world_tcp_connection::HandleAuthSession(std::shared_ptr<WorldPackets::Auth::AuthSession> authSession)
{
    // Get the account information from the auth database
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME);
    stmt->setInt32(0, int32(realm.Id.Realm));
    stmt->setString(1, authSession->RealmJoinTicket);

    _queryProcessor.AddQuery(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&world_tcp_connection::HandleAuthSessionCallback, this, authSession, std::placeholders::_1)));
}

void world_tcp_connection::HandleAuthSessionCallback(std::shared_ptr<WorldPackets::Auth::AuthSession> authSession, PreparedQueryResult result)
{
    // Stop if the account is not found
    if (!result)
    {
        // We can not log here, as we do not know the account. Thus, no accountId.
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Sent Auth Response (unknown account).");
        DelayedCloseSocket();
        return;
    }

    RealmBuildInfo const* buildInfo = sRealmList->GetBuildInfo(realm.Build);
    if (!buildInfo)
    {
        SendAuthResponseError(ERROR_BAD_VERSION);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Missing auth seed for realm build %u (%s).", realm.Build, GetRemoteIpAddress().to_string().c_str());
        DelayedCloseSocket();
        return;
    }

    AccountInfo account(result->Fetch());

    // For hook purposes, we get Remoteaddress at this point.
    std::string address = GetRemoteIpAddress().to_string();

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
        DelayedCloseSocket();
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
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_ATTEMPT_IP);
    stmt->setString(0, address);
    stmt->setString(1, authSession->RealmJoinTicket);
    LoginDatabase.Execute(stmt);
    // This also allows to check for possible "hack" attempts on account

    stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_ACCOUNT_INFO_CONTINUED_SESSION);
    stmt->setString(0, _sessionKey.AsHexStr());
    stmt->setUInt32(1, account.Game.Id);
    LoginDatabase.Execute(stmt);

    // First reject the connection if packet contains invalid data or realm state doesn't allow logging in
    if (sWorld->IsClosed())
    {
        SendAuthResponseError(ERROR_DENIED);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: World closed, denying client (%s).", GetRemoteIpAddress().to_string().c_str());
        DelayedCloseSocket();
        return;
    }

    if (authSession->RealmID != realm.Id.Realm)
    {
        SendAuthResponseError(ERROR_DENIED);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Client %s requested connecting with realm id %u but this realm has id %u set in config.",
            GetRemoteIpAddress().to_string().c_str(), authSession->RealmID, realm.Id.Realm);
        DelayedCloseSocket();
        return;
    }

    // Must be done before WorldSession is created
    bool wardenActive = sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED);
    if (wardenActive && account.Game.OS != "Win" && account.Game.OS != "Wn64" && account.Game.OS != "Mc64")
    {
        SendAuthResponseError(ERROR_DENIED);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Client %s attempted to log in using invalid client OS (%s).", address.c_str(), account.Game.OS.c_str());
        DelayedCloseSocket();
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
            DelayedCloseSocket();
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
            DelayedCloseSocket();
            return;
        }
    }

    int64 mutetime = account.Game.MuteTime;
    //! Negative mutetime indicates amount of seconds to be muted effective on next login - which is now.
    if (mutetime < 0)
    {
        mutetime = time(NULL) + llabs(mutetime);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME_LOGIN);
        stmt->setInt64(0, mutetime);
        stmt->setUInt32(1, account.Game.Id);
        LoginDatabase.Execute(stmt);
    }

    if (account.IsBanned())
    {
        SendAuthResponseError(ERROR_GAME_ACCOUNT_BANNED);
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthSession: Sent Auth Response (Account banned).");
        sScriptMgr->OnFailedAccountLogin(account.Game.Id);
        DelayedCloseSocket();
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
        DelayedCloseSocket();
        return;
    }

    TC_LOG_DEBUG("network", "world_tcp_connection::HandleAuthSession: Client '%s' authenticated successfully from %s.", authSession->RealmJoinTicket.c_str(), address.c_str());

    // Update the last_ip in the database as it was successful for login
    stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_IP);

    stmt->setString(0, address);
    stmt->setString(1, authSession->RealmJoinTicket);

    LoginDatabase.Execute(stmt);

    // At this point, we can safely hook a successful login
    sScriptMgr->OnAccountLogin(account.Game.Id);

    authed_ = true;
    world_session_ = new WorldSession(account.Game.Id, std::move(authSession->RealmJoinTicket), account.BattleNet.Id, shared_from_this(), account.Game.Security,
        account.Game.Expansion, mutetime, account.Game.OS, account.BattleNet.Locale, account.Game.Recruiter, account.Game.IsRectuiter);

    // Initialize Warden system only if it is enabled by config
    if (wardenActive)
        world_session_->InitWarden(&_sessionKey);

    _queryProcessor.AddQuery(world_session_->LoadPermissionsAsync().WithPreparedCallback(std::bind(&world_tcp_connection::LoadSessionPermissionsCallback, this, std::placeholders::_1)));
    AsyncRead();
}

void world_tcp_connection::LoadSessionPermissionsCallback(PreparedQueryResult result)
{
    // RBAC must be loaded before adding session to check for skip queue permission
    world_session_->GetRBACData()->LoadFromDBCallback(result);

    SendPacketAndLogOpcode(*WorldPackets::Auth::EnableEncryption(encrypt_key_, true).Write());
}

void world_tcp_connection::HandleAuthContinuedSession(std::shared_ptr<WorldPackets::Auth::AuthContinuedSession> authSession)
{
    WorldSession::ConnectToKey key;
    key.Raw = authSession->Key;

    type_ = ConnectionType(key.Fields.ConnectionType);
    if (type_ != CONNECTION_TYPE_INSTANCE)
    {
        SendAuthResponseError(ERROR_DENIED);
        DelayedCloseSocket();
        return;
    }

    uint32_t accountId = uint32_t(key.Fields.AccountId);
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_CONTINUED_SESSION);
    stmt->setUInt32(0, accountId);

    _queryProcessor.AddQuery(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&world_tcp_connection::HandleAuthContinuedSessionCallback, this, authSession, std::placeholders::_1)));
}

void world_tcp_connection::HandleAuthContinuedSessionCallback(std::shared_ptr<WorldPackets::Auth::AuthContinuedSession> authSession, PreparedQueryResult result)
{
    if (!result)
    {
        SendAuthResponseError(ERROR_DENIED);
        DelayedCloseSocket();
        return;
    }

    WorldSession::ConnectToKey key;
    key_ = key.Raw = authSession->Key;

    uint32_t accountId = uint32_t(key.Fields.AccountId);
    Field* fields = result->Fetch();
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
        TC_LOG_ERROR("network", "world_tcp_connection::HandleAuthContinuedSession: Authentication failed for account: %u ('%s') address: %s", accountId, login.c_str(), GetRemoteIpAddress().to_string().c_str());
        DelayedCloseSocket();
        return;
    }

    HmacSha256 encryptKeyGen(40, _sessionKey.AsByteArray(40).get());
    encryptKeyGen.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
    encryptKeyGen.UpdateData(server_challenge_.AsByteArray(16).get(), 16);
    encryptKeyGen.UpdateData(encryption_key_seed, 16);
    encryptKeyGen.Finalize();

    // only first 16 bytes of the hmac are used
    memcpy(encrypt_key_, encryptKeyGen.GetDigest(), 16);

    SendPacketAndLogOpcode(*WorldPackets::Auth::EnableEncryption(encrypt_key_, true).Write());
    AsyncRead();
}

void world_tcp_connection::HandleConnectToFailed(WorldPackets::Auth::ConnectToFailed& connectToFailed)
{
    if (world_session_)
    {
        if (world_session_->PlayerLoading())
        {
            switch (connectToFailed.Serial)
            {
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt1:
                    world_session_->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt2);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt2:
                    world_session_->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt3);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt3:
                    world_session_->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt4);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt4:
                    world_session_->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt5);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt5:
                {
                    TC_LOG_ERROR("network", "%s failed to connect 5 times to world socket, aborting login", world_session_->GetPlayerInfo().c_str());
                    world_session_->AbortLogin(WorldPackets::Character::LoginFailureReason::NoWorld);
                    break;
                }
                default:
                    return;
            }
        }
        //else
        //{
        //    transfer_aborted when/if we get map node redirection
        //    SendPacketAndLogOpcode(*WorldPackets::Auth::ResumeComms().Write());
        //}
    }
}

void world_tcp_connection::HandleEnableEncryptionAck()
{
    _authCrypt.Init(encrypt_key_);
    if (type_ == CONNECTION_TYPE_REALM)
        sWorld->AddSession(world_session_);
    else
        sWorld->AddInstanceSocket(shared_from_this(), key_);
}

void world_tcp_connection::SendAuthResponseError(uint32_t code)
{
    WorldPackets::Auth::AuthResponse response;
    response.Result = code;
    SendPacketAndLogOpcode(*response.Write());
}

bool world_tcp_connection::HandlePing(WorldPackets::Auth::Ping& ping)
{
    using namespace std::chrono;

    if (_LastPingTime == steady_clock::time_point())
    {
        _LastPingTime = steady_clock::now();
    }
    else
    {
        steady_clock::time_point now = steady_clock::now();

        steady_clock::duration diff = now - _LastPingTime;

        _LastPingTime = now;

        if (diff < seconds(27))
        {
            ++over_speed_ping_;

            uint32_t maxAllowed = sWorld->getIntConfig(CONFIG_MAX_OVERSPEED_PINGS);

            if (maxAllowed && over_speed_ping_ > maxAllowed)
            {
                std::unique_lock<std::mutex> sessionGuard(_worldSessionLock);

                if (world_session_ && !world_session_->HasPermission(rbac::RBAC_PERM_SKIP_CHECK_OVERSPEED_PING))
                {
                    TC_LOG_ERROR("network", "world_tcp_connection::HandlePing: %s kicked for over-speed pings (address: %s)",
                        world_session_->GetPlayerInfo().c_str(), GetRemoteIpAddress().to_string().c_str());

                    return false;
                }
            }
        }
        else
            over_speed_ping_ = 0;
    }

    {
        std::lock_guard<std::mutex> sessionGuard(_worldSessionLock);

        if (world_session_)
        {
            world_session_->SetLatency(ping.Latency);
            world_session_->ResetClientTimeDelay();
        }
        else
        {
            TC_LOG_ERROR("network", "world_tcp_connection::HandlePing: peer sent CMSG_PING, but is not authenticated or got recently kicked, address = %s", GetRemoteIpAddress().to_string().c_str());
            return false;
        }
    }

    SendPacketAndLogOpcode(*WorldPackets::Auth::Pong(ping.Serial).Write());
    return true;
}
