/*
 * Copyright (C) 2018
 */

#ifndef LOG_H
#define LOG_H

namespace lg
{
	class Log
	{
		public:
			static Log* instance();

			void Initialize(Trinity::Asio::IoContext* ioContext);
			void SetSynchronous();  // Not threadsafe - should only be called from main() after all threads are joined
			void LoadFromConfig();
			void Close();
			bool ShouldLog(std::string const& type, LogLevel level) const;
			bool SetLogLevel(std::string const& name, char const* level, bool isLogger = true);

			template<typename Format, typename... Args>
			inline void outMessage(std::string const& filter, LogLevel const level, Format&& fmt, Args&&... args)
			{
			    outMessage(filter, level, Trinity::StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...));
			}

			template<typename Format, typename... Args>
			void outCommand(uint32 account, Format&& fmt, Args&&... args)
			{
			    if (!ShouldLog("commands.gm", LOG_LEVEL_INFO))
				return;

			    outCommand(Trinity::StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...), std::to_string(account));
			}

			void outCharDump(char const* str, uint32 account_id, uint64 guid, char const* name);

			void SetRealmId(uint32 id);

			template<class AppenderImpl>
			void RegisterAppender()
			{
			    using Index = typename AppenderImpl::TypeIndex;
			    RegisterAppender(Index::value, &CreateAppender<AppenderImpl>);
			}

			std::string const& GetLogsDir() const { return m_logsDir; }
			std::string const& GetLogsTimestamp() const { return m_logsTimestamp; }
		private:
			Log();
			~Log();
			Log(Log const&) = delete;
			Log(Log&&) = delete;
			Log& operator=(Log const&) = delete;
			Log& operator=(Log&&) = delete;

			static std::string GetTimestampStr();
			void write(std::unique_ptr<LogMessage>&& msg) const;

			Logger const* GetLoggerByType(std::string const& type) const;
			Appender* GetAppenderByName(std::string const& name);
			uint8 NextAppenderId();
			void CreateAppenderFromConfig(std::string const& name);
			void CreateLoggerFromConfig(std::string const& name);
			void ReadAppendersFromConfig();
			void ReadLoggersFromConfig();
			void RegisterAppender(uint8 index, AppenderCreatorFn appenderCreateFn);
			void outMessage(std::string const& filter, LogLevel const level, std::string&& message);
			void outCommand(std::string&& message, std::string&& param1);

			std::unordered_map<uint8, AppenderCreatorFn> appenderFactory;
			std::unordered_map<uint8, std::unique_ptr<Appender>> appenders;
			std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;
			uint8 AppenderId;
			LogLevel lowestLogLevel;

			std::string m_logsDir;
			std::string m_logsTimestamp;

			Trinity::Asio::IoContext* _ioContext;
			Trinity::Asio::Strand* _strand;
	};
}

#endif
