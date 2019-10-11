/*
 * Copyright (C) 2018
 */

#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H

#include <map>
#include <string>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include "utils.h"
#include "threadpool.h"

struct Descriptor
{
        Descriptor() = default;
	Descriptor(int fd, sockaddr_in client_addr, uint32_t timeout) :
			fd(fd), addr(client_addr), timeout(timeout), last_active(time(0)) {}

	Descriptor(const Descriptor& d) : 
		fd(d.fd), addr(d.addr), timeout(d.timeout), last_active(d.last_active) {}
	Descriptor& operator=(const Descriptor& d)
	{
		fd = d.fd;
		addr = d.addr;
		timeout = d.timeout;
                last_active = d.last_active;
	}

	Descriptor(Descriptor&& d) : fd(std::move(d.fd)), addr(std::move(d.addr)), timeout(std::move(d.timeout)), last_active(std::move(d.last_active)) {}
	Descriptor& operator=(Descriptor&& d)
	{
		fd = std::move(d.fd);
		addr = std::move(d.addr);
		timeout = std::move(d.timeout);
                last_active = std::move(d.last_active);
	}
set_wait_read_byte(int n) { wait_read_byte = n; }
bool TInputStreamsHandler::handle_read( TEventHandler * object ){
	while ( this->m_wait_read_byte > 0 ){
		int remain  = INPUT_SOCKET_BUFFER_LEN"1024" - m_recv_byte ;
		int rc		= read( fd , m_buffer + m_recv_byte, remain );
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
bool TInputStreamsHandler::handle_write( TEventHandler * object ){
	//wait_write();
	TAutoLock autolock(&m_bool_lock);
	if ( m_out_stream.get_buffer_len() > 0 )
	{
		int rc = 0 ;
		while ( ( rc = write( this->get_handle() , m_out_stream.get_buffer() ,m_out_stream.get_buffer_len() )) <=0 )
		{
			if ( errno == EINTR ) continue;
			break;
		}
		if ( rc == (int)m_out_stream.get_buffer_len() )
		{
			m_out_stream.reset();
			this->get_event_scheduler()->modify_handler_event( this ,EVENT_READ );
			m_finish = true ;
			
		}else if ( rc > 0 )
		{
			m_out_stream.memory_move( rc , m_out_stream.get_buffer_len() - rc  );
			WL(LL_ERROR ,"send data error = %d str=%s\n",errno , strerror(errno));
			//this->get_event_scheduler()->modify_handler_event( this ,EVENT_WRITE | EPOLLONESHOT );
		}else //rc<= 0 
		{
			if (  errno == EWOULDBLOCK || errno == EAGAIN  ){
				//this->get_event_scheduler()->modify_handler_event( this ,EVENT_WRITE | EPOLLONESHOT );
				WL(LL_ERROR ,"send data error = %d str=%s\n",errno , strerror(errno));
			}else{
				WL(LL_ERROR ,"send data error = %d str=%s\n",errno , strerror(errno));
			}
		}
		
	}
	//broadcast_write();
	return true ;
}


	bool heart_beat()
	{
		// 如果在超时期间没有任何操作则返回false以指示服务器关闭socket
		if(static_cast<time_t>(last_active + timeout) < time(0))
		{
			printf("connection %s:%d has time out\n", net::ntop(addr).get(), ntohs(addr.sin_port));
			return false;
		}
		return true;
	}
	bool server_close();
	bool client_close();

	int fd;
	sockaddr_in addr;
	uint32_t timeout;
        time_t last_active;

	char buf[1024];
};

class Server
{
	public:
		Server(const std::string& listen_addr, uint16_t listen_port,
				uint32_t timeout_secs);
		~Server();

		void event_loop();

		bool set_non_blocking(int fd);

		bool handle_accept();
		bool handle_client(epoll_event ev);
		void remove_client(Descriptor& fd);
	private:
		int listenfd_, epollfd_;
		epoll_event events_[64];

		std::map<int, Descriptor> clients_;
		uint32_t timeout_secs_;
		time_t last_socket_check_;

		Threadpool pool_;
};

#endif
