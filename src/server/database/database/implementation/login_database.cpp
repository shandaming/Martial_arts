/*
 * Copyright (C) 2020
 */

#include "login_database.h"
#include "mysql_prepared_statement.h"

void login_database_connection::do_prepare_statements()
{
	if(!reconnection_)
		stmts_.resize(MAX_LOGINDATABASE_STATEMENTS);

	prepare_statement(LOGIN_SEL_IP_INFO, "SELECT unbandate > UNIX_TIMESTAMP() OR unbandate = bandate AS banned, NULL as country FROM ip_banned WHERE ip = ?", CONNECTION_ASYNC);
	prepare_statement(LOGIN_INS_LOG, "INSERT INTO logs (time, realm, type, level, string) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);

	prepare_statement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME, "SELECT a.id, a.sessionkey, ba.last_ip, ba.locked, ba.lock_country, a.expansion, a.mutetime, ba.locale, a.recruiter, a.os, ba.id, aa.gmLevel" "bab.unbandate > UNIX_TIMESTAMP() OR bab.unbandate = bab.bandate, ab.unbandate > UNIX_TIMESTAMP() OR ab.unbandate = ab.bandate, r.id" "FROM account a LEFT JOIN account r ON a.id = r.recruiter LEFT JOIN battlenet_accounts ba ON a.battlenet_account = ba.id " "LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID IN (-1, ?) LEFT JOIN battlenet_account_bans bab ON ba.id = bab.id LEFT JOIN account_banned ab ON a.id = ab.id AND ab.active = 1" " WHERE a.username = ? ORDER BY aa.RealmID DESC LIMIT 1", CONNECTION_ASYNC);

prepare_statement(LOGIN_UPD_ACCOUNT_INFO_CONTINUED_SESSION, "UPDATE account SET sessionkey = ? WHERE id = ?", CONNECTION_ASYNC);

prepare_statement(LOGIN_SEL_ACCOUNT_INFO_CONTINUED_SESSION, "SELECT username, sessionkey FROM account WHERE id = ?", CONNECTION_ASYNC);

prepare_statement(LOGIN_UPD_MUTE_TIME_LOGIN, "UPDATE account SET mutetime = ? WHERE id = ?", CONNECTION_ASYNC);

prepare_statement(LOGIN_UPD_LAST_IP, "UPDATE account SET last_ip = ? WHERE username = ?", CONNECTION_ASYNC);

prepare_statement(LOGIN_UPD_LAST_ATTEMPT_IP, "UPDATE account SET last_attempt_ip = ? WHERE username = ?", CONNECTION_ASYNC);
}

login_database_connection::login_database_connection(mysql_connection_info& conn_info) : mysql_connection(conn_info) {}

login_database_connection::login_database_connection(producer_consumer_queue<sql_operation*>* q, mysql_connection_info& conn_info) : mysql_connection(q, conn_info) {}

login_database_connection::~login_database_connection() {}
