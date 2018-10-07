/*
 * Copyright (C) 2018
 */

#ifndef PACKET_H
#define PACKET_H

#define GET_UINT(A) (*(( unsigned int *) (A)))
#define GET_INT(A) (*((int *) (A)))

#define GET_USHORT(A) (*((unsigned short *) (A)))
#define GET_SHORT(A) (*((short *) (A)))

#define GET_BYTE(A) (*((unsigned char *) (A)))

#define GET_INT64(A) (*((long long *) (A)))

#define SET_INT64(T,A)	*((long long *) (T))= (long long) (A)

#define SET_INT(T,A)	*((int *) (T))= (int) (A)
#define SET_UINT(T,A)	*((unsigned int *) (T))= (unsigned int) (A)
#define SET_SHORT(T,A)	*((short *) (T))= (short) (A)
#define SET_USHORT(T,A)	*((unsigned short *) (T))= (unsigned short) (A)
#define SET_BYTE(T,A)	*((unsigned char *) (T))= (unsigned char) (A)

class Packet
{
	public:
		Packet(uint8_t* data);

		void assign(uint8_t* data);
		void assign_body(uint16_t packet_type, uint8_t* body, uint32_t body_len);

		/*******************setter*************************/

		// 设置包协议版本号
		void set_protocal_version(char version) 
		{
			SET_BYTE(packet_data + 2, version); 
		}

		// 设置包类型
		void set_packet_type(uint16_t packet_type)
		{
			SET_USHORT(packet_data + 3, htons(packet_type));
		}

		// 设置包长度
		void set_data_len(uint32_t len)
		{
			SET_USHORT(packet_data, htons(len));
		}

		// 设置包尾
		void set_packet_end()
		{
			uint16_t packet_end = PACKET_EOF_FLAG;

			memcpy(packet_data + PACKET_HEAD_LEN + get_data_len(), &packet_end, sizeof(uint16_t));
		}

		// 设置包序号
		void set_packet_seq( uint16_t seq )
		{
			SET_UINT( packet_data + 7 , htons(seq) );
		}

		// 构建包头
		void build_packet_head( uint16_t packet_type )
		{
			set_protocal_version( PACKET_VERSTION );
			set_packet_type( packet_type );
		}
		
		/*******************getter*************************/

		uint16_t get_packet_len()
		{
			return htons( GET_USHORT( packet_data )) + PACKET_HEAD_LEN  ;
		}

		uint16_t get_data_len()
		{
			return htons(GET_USHORT( packet_data ));
		}

		uint16_t get_packet_type()
		{
			return htons(GET_USHORT( packet_data + 3 ));
		}
		uint8_t get_protocal_version()
		{
			return GET_BYTE( packet_data + 2 );
		}
		uint8_t get_enc_key()
		{
			return packet_data [ 5 ]; // ???
		}
		void set_enc_key( uint8_t key )
		{
			packet_data [ 5 ] = key ; // ???
		}

		uint16_t get_packet_seq()
		{
			return htons(GET_UINT( packet_data + 7 ));
		}
		uint8_t* get_packet()
		{
			return packet_data ;
		}
		uint8_t* get_data()
		{
			return packet_data + PACKET_HEAD_LEN ;
		}
	private:
		uint8_t* packet_data_;
};

constexpr int queue_packet_len = 512;

class Packet
{
		union Params
		{
			int		fd ;
			int		table_id;
		};
	public:
		Packet(uint8_t* buf, int len)
		{
			assert(len <= queue_packet_len);
			memcpy(data_, buf, len);
		}

		void reset()
		{
			handler			= NULL ;
			parms.fd		= -1 ;
		}

		/***************************getter****************************/

		uint16_t get_packet_len()
		{
			int len = GET_USHORT( data_ ) ;
			return ntohs( len + PACKET_HEAD_LEN )  ;
		}
		
		uint16_t get_data_len()
		{
			return ntohs(GET_USHORT( data_ ));
		}

		uint16_t get_packet_type()
		{
			return ntohs(GET_USHORT( data_ + 3 ));
		}

		uint8_t get_protocal_version()
		{
			return GET_BYTE( data_ + 2 );
		}

		uint32_t get_packet_seq()
		{
			return ntohl(GET_UINT( data_ + 7 ));
		}

		uint8_t * get_packet()
		{
			return data_ ;
		}

		uint8_t * get_data()
		{
			return data_ + PACKET_HEAD_LEN ;
		}

		int get_fd()
		{
			return parms.fd ;
		}

		int get_table_id()
		{
			return parms.table_id ;
		}

		void * get_handler()
		{
			return handler ;
		}

		/***************************setter****************************/

		void set_data_len( uint16_t data_len )
		{
			SET_USHORT( data_ , htons(data_len) );
		}

		void set_fd( int fd_ )
		{
			parms.fd = fd_ ;
		}

		void set_table_id( int table_id )
		{
			parms.table_id = table_id ;
		}

		void set_handler( void * handler_ )
		{
			handler = handler_ ;
		}
		
		bool assign_buffer( int fd_ , void * handler_ , void * buffer , uint16_t len )
		{
			if ( len >= queue_packet_len ) return false ;
			set_fd( fd_ );
			set_handler( handler_ );
			
			memcpy( data_ , buffer , len );
			return true ;
		}

		void set_packet_type( uint16_t packet_type )
		{
			SET_USHORT( data_ + 3 , htons(packet_type) );
		}
	private:
		uint8_t data_[queue_packet_len];

		Params	parms ;
		void *  handler;
};

#endif
