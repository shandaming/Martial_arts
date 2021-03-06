/*
 * Copyright (C) 2020
 */

#ifndef _BATTLENET_RPC_ERROR_CODES_H
#define _BATTLENET_RPC_ERROR_CODES_H

enum battlenet_rpc_error_code : uint32_t
{
	ERROR_DENIED = 0x00000003,
	ERROR_BAD_VERSION = 0x0000001C,
	ERROR_SERVER_IS_PRIVATE = 0x00000021,
	ERROR_GAME_ACCOUNT_BANNED = 0x00000034,
	ERROR_RISK_ACCOUNT_LOCKED = 0x0000A413
};

#endif
