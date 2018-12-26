/*
 * Copyright (C) 2018
 */

#include <signal.h>
#include <locale>
#include <thread>

#include "net/inet_address.h"
#include "net/event_loop.h"
#include "net/tcp_server.h"
#include "exceptions.h"
#include "net/tcp_connection.h"
#include "log/log_stream.h"

void signal_handler(int sig)
{
	int saved_errno = errno;

	switch(sig)
	{
		case SIGSEGV:
			break;
		case SIGTERM:
			break;
		case SIGQUIT:
			break;
		case SIGINT:
			break;
		case SIGUSR1:
			break;
		case SIGUSR2:
			break;
		default:
			break;
	}

	errno = saved_errno;
}

void init_locale()
{
	std::locale locale("zh_CN.UTF-8");
	std::locale::global(locale);
}

void onMessage(const net::Tcp_connection_ptr& conn, net::Buffer* buf, Timestamp)
{
  conn->send(buf);
}

int main(int argc, char* argv[])
{
	struct sigaction terminate_handler;
	terminate_handler.sa_handler = signal_handler;
	terminate_handler.sa_flags = 0;
	sigemptyset(&terminate_handler.sa_mask);

	sigaction(SIGTERM, &terminate_handler, nullptr);
	sigaction(SIGINT, &terminate_handler, nullptr);

	terminate_handler.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &terminate_handler, nullptr);

	init_locale();

	LOG_INIT("./logs/lg", 409600)

	net::Inet_address addr(argv[1], static_cast<uint16_t>(atoi(argv[2])));
	net::Event_loop event_loop;

	net::Tcp_server tcp_server(&event_loop, addr, argv[0]);
	tcp_server.set_thread_num(std::thread::hardware_concurrency());
	tcp_server.set_connection_callback([](const net::Tcp_connection_ptr& conn)
			{
				if(conn->connected()) 
				{
					conn->set_tcp_no_delay(true);
				}; 
			});

	tcp_server.setMessage_callback(onMessage);
	tcp_server.start();
	event_loop.loop();

	return 0;
}
