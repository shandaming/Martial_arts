

#ifndef DB_APPENDERDB_H
#define DB_APPENDERDB_H

#include <vector>

#include "appender.h"

class appender_db: public appender
{
    public:
        typedef std::integral_constant<appender_type, APPENDER_DB>::type type_index;

        appender_db(uint8_t id, const std::string& name, log_level level, appender_flags flags, std::vector<const char*> extra_args);
        ~appender_db() {}

        void set_realm_id(uint32_t realm_id) override;
        appender_type get_type() const override { return type_index::value; }

    private:
	void _write(const log_message* message) override;

        uint32_t realm_id_;
        bool enabled_; 
};

#endif
