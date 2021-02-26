/*
 * Copyright (C) 2018
 */

#include "remote_control.h"

int main()
{
	Remote_control* rc = new Concrete_remote(new Rca_tv());
	Remote_control* rc1 = new Concrete_remote(new Sony_tv());

	rc->on();
	rc->off();
	rc->set_channel();

	rc1->on();
	rc1->off();
	rc1->set_channel();
}
