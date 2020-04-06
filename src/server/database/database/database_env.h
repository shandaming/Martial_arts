/*
 * Copyright (C) 2020
 */

#ifndef DB_DATABASE_ENV_H
#define DB_DATABASE_ENV_H

#include "database_worker_pool.h"
#include "login_database.h"
#include "character_database.h"
#include "world_database.h"
#include "hotfix_database.h"

extern database_worker_pool<world_database_connection> world_database;
extern database_worker_pool<character_database_connection> character_database;
extern database_worker_pool<login_database_connection> login_database;
extern database_worker_pool<hotfix_database_connection> hotfix_database;

#endif
