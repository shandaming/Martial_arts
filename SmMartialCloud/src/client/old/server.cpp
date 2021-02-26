/*
 * Copyright (C) 2018
 */

#include <unistd.h>
#include <fcntl.h>
#include "server.h"
#include "utils.h"
#include "exceptions.h"
#include "byte_buffer.h"
#include "rio.h"

bool TInputStreamsHandler::handle_read(int fd){
	while ( this->m_wait_read_byte > 0 ){
		int remain  = INPUT_SOCKET_BUFFER_LEN - m_recv_byte ;
		int rc		= read(fd , m_buffer + m_recv_byte, remain );
		if ( rc <= 0 ) {
			if ( errno == EINTR ) continue ;
			//WL(LL_ERROR ,"error = %d str=%s %d",errno , strerror(errno) , remain );
			m_recv_byte = 0 ;
			return false ;
		}
		m_wait_read_byte		-= rc ;
		m_recv_byte				+= rc ;
		if ( !parse_buffer() ) return false ;
	}
	return true ;
}

Server::Server(const std::string& listen_addr, 
		uint16_t listen_port, uint32_t timeout_secs):
			listenfd_(-1), epollfd_(-1), timeout_secs_(timeout_secs),
			last_socket_check_(0)
{
	sockaddr_in sin{0};
	int ret = net::pton(listen_addr.c_str(), sin);
	if(ret <= 0)
	{
		Error("字符串IP转换失败！");
		return;
	}
        sin.sin_family = AF_INET;
	sin.sin_port = htons(listen_port);

	listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd_ <= 0)
	{
		Error("创建socket失败！");
		return;
	}
//设置快速重启和地址重复利用 选项
	int optval;
	if(setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
	{
		close(listenfd_);
		Error("设置socket属性失败！");
		return;
	}

	//小包不延时发送
//禁用 Nagle 算法
	setsockopt(listenfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(int));

//设置心跳
    setsockopt(listenfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(int));  

	int keepIdle = 30;//当没有数据包传输后，等多少秒开始KeepAlive
	int keepInterval = 15;//发送keepalive 时间间隔
	int keepCount = 2;//最大检测次数

	int rc	= setsockopt(listenfd_, SOL_TCP, TCP_KEEPINTVL, &keep_idle, sizeof(int));
	rc += setsockopt(listenfd_, SOL_TCP, TCP_KEEPIDLE, &keep_interval, sizeof(int));
	rc += setsockopt(listenfd_, SOL_TCP, TCP_KEEPCNT, &keep_count, sizeof(int)); 
//

// 设置快速关闭
	struct linger linger; 
	linger.l_onoff = 1; 
	linger.l_linger = 0; 
	setsockopt(listenfd_, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)); 

		int on = 5; 
		setsockopt ( listenfd_, SOL_TCP, TCP_DEFER_ACCEPT, &on, sizeof (int));
		setsockopt ( listenfd_, SOL_SOCKET, TCP_DEFER_ACCEPT, &on, sizeof (int));
	// 设置延时接受新连接,有数据发送时才激活接收连接
	int timeout = 5;
	setsockopt(listenfd_, IPPROTO_TCP, TCP_DEFER_ACCEPT, &timeout, sizeof(timeout));

//设置接收缓存区的大小
		int buf= 4096 ;/*32*1024*/;//脡猫脰脙脦陋32K 
		setsockopt( listenfd_, SOL_SOCKET, SO_RCVBUF, &buf, sizeof(int));
//设置发送缓冲区的大小
		setsockopt( listenfd_, SOL_SOCKET, SO_SNDBUF, &buf, sizeof(int));

	if(bind(listenfd_, reinterpret_cast<struct sockaddr*>(&sin), sizeof(sin)) == -1)
	{
		close(listenfd_);
		Error("绑定IP和端口错误！");
                exit(1);
		return;
	}

	if(listen(listenfd_, SOMAXCONN) == -1)
	{
		close(listenfd_);
		Error("监听端口失败！");
		return;
	}

	epollfd_ = epoll_create(5);
	if(epollfd_ == -1)
	{
		close(listenfd_);
		Error("创建epoll失败！");
		return;
	}

	epoll_event e;
	e.events = EPOLLIN;
	e.data.fd = listenfd_;

	if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, listenfd_, &e) == -1)
	{
		close(listenfd_);
		close(epollfd_);
		Error("添加监听描述到epoll队列错误！");
		return;
	}
}

Server::~Server()
{
	if(listenfd_ != -1)
		close(listenfd_);
	if(epollfd_ != -1)
		close(epollfd_);
	//delete[] events_;
}

void Server::event_loop()
{
	while(1)
	{
		int fds = epoll_wait(epollfd_, events_, 64, 1000);
		if(fds != -1)
		{
			for(size_t i = 0; i < fds; ++i)
			{
				if(events_[i].data.fd == listenfd_)
					handle_accept();
				else
					handle_client(events_[i]);
			}
		}

		// 每秒执行一次清理并删除超时socket
		if((last_socket_check_ + 1) < time(0) && clients_.size() > 0)
		{
			for(auto it = clients_.begin(); it != clients_.end(); ++it)
			{
				//fd = it->second();
				if(!it->second.heart_beat())
				{
					// 没有心跳了就从clients_里删除
					it = clients_.erase(it);
					close(it->second.fd);
				}
			}
			last_socket_check_ = time(0);
		}
	}
}

bool Server::set_non_blocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if(flags == -1)
		return false;
	flags |= O_NONBLOCK;
	if(fcntl(fd, F_SETFL, flags) == -1)
		return false;
	return true;
}

bool Server::handle_accept()
{
	sockaddr_in client_sin;
        socklen_t client_len = sizeof(client_sin);

	int fd = accept(listenfd_, reinterpret_cast<struct sockaddr*>(&client_sin), &client_len);
	if(fd == -1)
	{
		printf("接受连接失败！");
		close(fd);
		return false;
	}
	if(!set_non_blocking(fd))
	{
		printf("设置异步IO失败！");
		close(fd);
		return false;
	}

	Descriptor clientfd(fd, client_sin, timeout_secs_);

	epoll_event e;
	e.events = EPOLLIN | EPOLLRDHUP | EPOLLET |EPOLLONESHOT;
	e.data.fd = clientfd.fd;

	if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, clientfd.fd, &e) == -1)
	{
		printf("添加描述符到epoll失败！");
		close(fd);
		close(epollfd_);
		return false;
	}

	clients_[fd] = clientfd;
			
	printf("new client: %s:%d\n", net::ntop(clientfd.addr).get(), ntohs(clientfd.addr.sin_port));
			return true;
}

bool Server::handle_client(epoll_event ev)
{
	auto it = clients_.find(ev.data.fd);
	if(it == clients_.end())
	{
		Error("未发现描述符！");
		return false;
	}

        // https://github.com/MalwareTech/SimpleEpollServer
	if(ev.events & EPOLLIN)
	{
        /*
		if(!read(it->second.fd))
		{
            clients_.erase(it);
			close(it->second.fd);
			return false;
		}
        */
                pool_.enqueue(read_event, it->second.fd);
	}

	if(ev.events & EPOLLRDHUP)
	{
        clients_.erase(it);
	    close(it->second.fd);
		return false;
	}

	if(ev.events & EPOLLOUT)
	{
		/*
		if(!it->write_ready())
		{
			clients_.erase(it);
			close(it->second.fd);
			return false;
		}
		*/
	}
	return true;
}

void Server::remove_client(Descriptor& fd)
{
	auto it = clients_.find(fd.fd);
	clients_.erase(it);
}
