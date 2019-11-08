

#include "AppenderDB.h"
#include "DatabaseEnv.h"
#include "LogMessage.h"
#include "PreparedStatement.h"

appender_DB::appender_DB(uint8 id, std::string const& name, log_level level, appender_flags /*flags*/, std::vector<char const*> /*extraArgs*/)
    : appender(id, name, level), realm_id_(0), enabled_(false) { }

void appender_DB::_write(log_message const* message)
{
    // Avoid infinite loop, PExecute triggers Logging with "sql.sql" type
    if (!enabled || (message->type.find("sql") != std::string::npos))
        return;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_LOG);
    stmt->setUInt64(0, message->mtime);
    stmt->setUInt32(1, realmId);
    stmt->setString(2, message->type);
    stmt->setUInt8(3, uint8(message->level));
    stmt->setString(4, message->text);
    LoginDatabase.Execute(stmt);
}

void appender_DB::set_realmId(uint32_t realm_id)
{
    enabled = true;
    realm_id_ = realm_id;
}
