

#include "appender_db.h"
#include "database_env.h"
#include "log_message.h"
#include "prepared_statement.h"

appender_db::appender_db(uint8_t id, const std::string& name, log_level level, appender_flags, std::vector<const char*>)
    : appender(id, name, level), realm_id_(0), enabled_(false) { }

void appender_db::_write(const log_message* message)
{
    // Avoid infinite loop, PExecute triggers Logging with "sql.sql" type
    if (!enabled_ || (message->type.find("sql") != std::string::npos))
        return;

    login_database_prepared_statement* stmt = login_database.get_prepared_statement(LOGIN_INS_LOG);
    stmt->set_uint64(0, message->mtime);
    stmt->set_uint32(1, realm_id_);
    stmt->set_string(2, message->type);
    stmt->set_uint8(3, uint8(message->level));
    stmt->set_string(4, message->text);
    login_database.execute(stmt);
}

void appender_db::set_realm_id(uint32_t realm_id)
{
    enabled_ = true;
    realm_id_ = realm_id;
}
