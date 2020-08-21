/*
 * Copyright (C) 2020
 */

#include "authentication_packets.h"

byte_buffer& operator<<(byte_buffer& data, const world_packets::auth::virtual_realm_name_info& virtual_realm_info)
{
	data.write_bit(virtual_realm_info.is_local);
	data.write_bit(virtual_realm_info.is_internal_realm);
	data.write_bits(virtual_realm_info.realm_name_actual.length(), 8);
	data.write_bits(virtual_realm_info.realm_name_normalized.length(), 8);
	data.flush_bits();

	data.write_string(virtual_realm_info.realm_name_actual);
	data.write_string(virtual_realm_info.realm_name_normalized);

	return data;
}

byte_buffer& operator<<(byte_buffer& data, const world_packets::auth::virtual_realm_info& virtual_realm_info)
{
	data << uint32_t(virtual_realm_info.realm_address);
	data << virtual_realm_info.realm_name_info;

	return data;
}
