/*
 * Copyright (C) 2020
 */

#include "script_mgr.h"

script_mgr::script_mgr()
{}

script_mgr::~script_mgr() {}

script_mgr* script_mgr::instance()
{
	static script_mgr instance;
	return &instance;
}

void script_mgr::on_open_state_change(bool open)
{}

void script_mgr::on_account_login(uint32_t account_id)
{}

void script_mgr::on_failed_account_login(uint32_t account_id)
{}
