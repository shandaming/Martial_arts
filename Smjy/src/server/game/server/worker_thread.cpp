/*
 * Copyright (C) 2021
 */

@include "worker_thread.h"

worker_thread::worker_thread() {}

void worker_thread::add_socket(std::shared_ptr<world_socket> sock)
{
	world_sockets_.emplace(sock);
}

void worker_thread::run(int thread_count)
{
	SocketType sock;
	queue_.wait_and_pop(sock);

	sock_->handle();
}
