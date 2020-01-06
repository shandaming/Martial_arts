/*
 * Copyright (C) 2019
 */

#include <iostream>

#include "common/configuration/revision.h"
#include "common/cmdline/option_description.h"
#include "common/cmdline/cmdline.h"
#include "common/cmdline/variables_map.h"
#include "database/database/mysql_threading.h"
#include "database/database/database_loader.h"

variables_map get_console_arguments(int argc, char** argv)
{
	options_description all("Allowed options");
	all.add_options()
		("help,h", "print usage message")
		("version,v", "print version build info");

	variables_map vm;
	store(cmdline(argc, argv).options(all).run(), vm);

	if(vm.count("help"))
	{
		std::cout << all << std::endl;
	}
	else if(vm.count("version"))
	{
		std::cout << revision::get_full_version() << std::endl;
	}
	return vm;
}

bool start_db()
{
	mysql::library_init();

	database_loader loader("worldserver", databaseloacer::database_none);
	loader.add_database(login_database, "login");
	loader.add_database(character_database, "character");
	loader.add_database(world_database, "world");
	loader.add_database(hotfix_database, "hotfix");

	if(!loader.load())
		return false;

	world_database.execute("update version set core_version = '%s', core_revision = '%s'", get_full_version(), get_hash());

	return true;
};

int main(int argc, char* argv[])
{
	signal(SIGABRT, &abort_handler);

	auto vm = get_console_arguments(argc, argv);
	if(vm.count("help") || vm.count("version"))
		return 0;
}
