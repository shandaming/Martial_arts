AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = world_server

world_server_SOURCES = main.cpp \
				 net/acceptor.cpp \
				 net/buffer.cpp \
				 net/channel.cpp \
				 net/event_loop_threadpool.cpp \
				 net/event_loop.cpp \
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
				 log/async_log.cpp \
				 log/log_stream.cpp \
				 common/thread.cpp \
				 common/timestamp.cpp

                         
AM_CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -Werror=non-virtual-dtor -Wno-unused-local-typedefs -Wno-maybe-uninitialized -Wold-style-cast -Wtrampolines
