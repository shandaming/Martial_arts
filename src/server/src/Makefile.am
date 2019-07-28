AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = world_server

world_server_SOURCES = main.cpp \
				 net/acceptor.cpp \
				 net/buffer.cpp \
				 net/channel.cpp \
				 net/event_loop_threadpool.cpp \
				 net/event_loop.cpp \
				 net/event_loop_thread.cpp \
				 net/inet_address.cpp \
				 net/net_utils.cpp \
				 net/poller.cpp \
				 net/socket.cpp \
				 net/tcp_server.cpp \
				 net/tcp_connection.cpp \
				 net/timer.cpp \
				 net/timer_queue.cpp \
				 log/async_log.cpp \
				 log/logging.cpp \
				 log/log_file.cpp \
				 log/log_stream.cpp \
				 common/thread.cpp \
				 common/timestamp.cpp \
				 common/system_info.cpp \
				 common/string_utils.cpp \
				 database/update/update_fetcher.cpp \
				 database/update/db_update.cpp \
				 database/field.cpp \
				 database/database_loader.cpp \
				 database/mysql_connection.cpp \
				 database/mysql_pool.cpp \
				 database/prepared_statement_cpp \
				 database/query_result.cpp \
				 configure/builtin_config.cpp \
				 configure/config.cpp \
				 configure/ini_parser.cpp \
				 configure/revision.cpp \
				 configure/string_utils.cpp

world_server_LDADD = -lstdc++fs 
                         
AM_CXXFLAGS = -std=c++1z -pthread -Wall -Wextra -Werror=non-virtual-dtor -Wno-unused-local-typedefs -Wno-maybe-uninitialized -Wold-style-cast -Wtrampolines
