/*
 * Copyright (C) 2020
 */

#ifndef NET_NETWORK_THREAD_H
#define NET_NETWORK_THREAD_H

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <algorithm>

#include "errors.h"
#include "log.h"

#include "socket.h"

template<typename SocketType>
class network_thread
{
public:
	network_thread() : connections_(0), stopped_(false), thread_(nullptr) {}

	virtual ~network_thread()
	{
		stop();
		if(thread_)
		{
			wait();
			delete thread_;
		}
	}

	void stop()
	{
		stopped_ = true;
	}

	bool start()
	{
		if(thread_)
			return false;
		thread_ = new std::thread(&network_thread::run(), this);
		return true;
	}

	void wait()
	{
		ASSERT(thread_);

		thread_->join();
		delete thread_;
		thread_ = nullptr;
	}

	int32_t get_connection_count() const { return connections_; }

	virtual void add_socket(std::shared_ptr<SocketType> sock)
	{
		std::lock_guard<std::mutex> lk(new_socket_lock_);

		++connections_;
		new_sockets_.push_back(sock);
		socket_added(sock);
	}

	//socket* get_socket_for_accept() { return accept_socket_; }
protected:
	virtual void socket_added(std::shared_ptr<SocketType>) {}
	virtual void socket_removed(std::shared_ptr<SocketType>) {}

	void add_new_sockets()
	{
		std::lock_guard<std::mutex> lk(new_socket_lock_);

		if(new_sockets_.empty())
			return;

		for(auto sock : new_sockets_)
		{
			if(!sock->is_open())
			{
				socket_removed(sock);
				--connections_;
			}
			else
				sockets_.push_back(sock);
		}
		new_sockets_.clear();
	}

	void run()
	{
		LOG_DEBUG("network", "Network Thread Starting");

		// 定时器

		LOG_DEBUG("network", "Network Thread exits.");
		new_sockets_.clear();
		sockets_.clear();
	}

	void update()
	{
		if(stopped_)
			return;

		// 定时器

		add_new_sockets();

		sockets_.erase(std::remove_if(sockets_.begin(), sockets_.end(),
					[this](std::shared_ptr<SocketType> sock)
					{
						if(!sock->update())
						{
							if(sock->is_open())
								sock->close_socket();

							this->socket_removed(sock);
							--this->connections_;
							return true;
						}
						return false;
					}
					), sockets_.end());
	}
private:
	typedef std::vector<std::shared_ptr<SocketType>> socket_container;

	std::atomic<int32_t> connections_;
	std::atomic<bool> stopped_;

	std::thread* thread_;
	socket_container sockets_;

	std::mutex new_socket_lock_;
	socket_container new_sockets_;
};

#endif
