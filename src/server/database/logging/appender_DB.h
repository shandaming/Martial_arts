

#ifndef DB_APPENDERDB_H
#define DB_APPENDERDB_H

#include "Appender.h"

class appender_DB: public appender
{
    public:
        typedef std::integral_constant<appenderType, APPENDER_DB>::type typeIndex;

        appenderDB(uint8 id, std::string const& name, log_level level, appender_flags flags, std::vector<char const*> extraArgs);
        ~appender_DB() {}

        void set_realmId(uint32 realmId) override;
        appender_type get_type() const override { return typeIndex::value; }

    private:
	void write(log_message const* message) override;

        uint32_t realm_id_;
        bool enabled_; 
};

#endif
