/*
 * Copyright (C) 2018
 */

#ifndef STREAM_H
#define STREAM_H

constexpr int buf_len = 1024

class Stream
{
	public:
		void read(int fd)
		{
			while(wait_read_type_ > 0)
			{
				int remain = buf_len - recv_byte_;
				int rc = read(fd, recv_buf_ + recv_byte_, remain);
				if(rc <= 0)
				{
					if(errno == EINTR)
						continue;
					recv_byte_ = 0;
					return false;
				}

				wait_read_byte_ -= rc;
				recv_byte_ += rc;

				if(!parse_buffer())
					return false;
			}
			return true;
		}
		void write(int fd);
	private:
		char recv_buf_[buf_len] {0};
		char send_buf_[buf_len] {0};

		std::mutex m_;

		uint32_t recv_byte_ = 0;
		uint32_t wait_read_type_ = 0;
};

#endif
