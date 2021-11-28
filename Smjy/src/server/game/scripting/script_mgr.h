/*
 * Copyright (C) 2020
 */

#ifndef _SCRIPT_MGR_H
#define _SCRIPT_MGR_H

#include <cstdint>

class script_mgr
{
public:
	static script_mgr* instance();

	void on_open_state_change(bool open);

	void on_failed_account_login(uint32_t account_id);

	void on_account_login(uint32_t account_id);
private:
	script_mgr();
	~script_mgr();
};

#define SCRIPT_MGR script_mgr::instance()

#endif
