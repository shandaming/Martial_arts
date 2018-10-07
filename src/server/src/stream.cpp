/*
 * Copyright (C) 2018
 */

#include "stream.h"
#include "packet_protocol.h"

void encrypt(BYTE * buffer, WORD len, BYTE key)
{
	for( WORD i=0; i<len; ++i)
		buffer[i] = ((buffer[i])^key)+key;
}

void decrypt(BYTE * buffer, WORD len, BYTE key)
{
	for( WORD i=0; i<len; ++i)
		buffer[i] = (buffer[i]-key)^key; 
}

		void Stream::read(int fd)
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

bool Stream::parse_buffer(){
	while (recv_byte_ >= PACKET_HEAD_LEN )
	{
		uint32_t data_len	= htons( GET_USHORT( recv_buf_ )) ; //数据长度
		uint32_t packet_len	= data_len + PACKET_HEAD_LEN ;
		if ( packet_len > INPUT_SOCKET_BUFFER_LEN ) 
		{
			m_recv_byte = 0 ;
			return false ;
		}
		if ( recv_byte_ < packet_len ) 
			return true ;
		
		// 处理包协议之后的数据
		process_message( htons( GET_USHORT( recv_buf_ + 3 )) , packet_len  );
		recv_byte_ -= packet_len ;
		if ( recv_byte_ > 0 ) {
			memmove( recv_buf_ , recv_buf_ + packet_len , recv_byte_ );
		}
	}
	return true ;
		
}
void Stream::process_message( uint16_t packet_type , uint16_t packet_len  ){
	
	
	m_player->reflash_active_time();
#ifdef ENCRYPT
	if ( !(	   packet_type >= MSG_HTTP_NOTIFY_RELOAD 
			|| packet_type == MSG_REQ_NOTIFY_TRADE_STATUS ) ){
		decrypt( m_buffer + PACKET_HEAD_LEN , packet_len - PACKET_HEAD_LEN , m_buffer [ 5 ]  );
	}
	
#endif
	switch ( packet_type )
	{
		case MSG_HTTP_GET_ONLINE_PLAYERS:
			{
				get_online_players();
				break;
			}
		case MSG_HTTP_RUNNING_INFO:
			{
				get_running_info();
				break;
			}
		case MSG_REQ_RECHARGE:
			{
				fill_gold();
				break;
			}
		case MSG_REQ_ACTIVE_GOLD_PIG:
			{
				active_gold_pig();
				break;
			}
		case MSG_REQ_LUCKY:
			{
				proc_lucky();
				break;
			}
		case MSG_REQ_HEARTBEAT:
			{
				SET_USHORT( m_buffer + 3 , HTONS( MSG_RESP_HEARTBEAT) );
				this->write_out_stream( m_buffer , packet_len );
				break;
			}
		
		//登录处理
		case MSG_REQ_LOGIN:
		case MSG_REQ_SK_LOGIN:
		case MSG_REQ_USER_DETAIL:
		case MSG_REQ_EMAIL_LIST:
		case MSG_REQ_TASK_LIST:
		case MSG_REQ_EXCH_RECORD:
		case MSG_HTTP_NOTIFY_RELOAD:
			{
				TReqPacket * packet = ( TReqPacket *)TGlobalObject::get_quque_packet_pool()->malloc_memory() ;
				if ( !packet->assign_buffer( htonl( get_address().get_long_addr()),m_player ,m_buffer ,packet_len ) ){
					TGlobalObject::get_quque_packet_pool()->free_memory( packet );
					return ;
				}
				TGlobalObject::get_login_thread()->push_message( packet );
				//printf("login\n");
				break;
			}
		//注册处理
		case MSG_REQ_REGISTER:
		case MSG_REQ_FAST_REGISTER:
		case MSG_REQ_UPDATE_USER_PASSWORD:
		case MSG_REQ_UPDATE_PASSWORD:
		case MSG_REQ_UPDATE_USER:
		case MSG_REQ_BIND_PHONE:
		case MSG_REQ_RECV_TASK_BONUS:
		case MSG_REQ_EXCH_COMMODITY:
		case MSG_REQ_RECOMMEND_AWARD:
			{
				TReqPacket * packet = ( TReqPacket *)TGlobalObject::get_quque_packet_pool()->malloc_memory() ;
				if ( !packet->assign_buffer( htonl( get_address().get_long_addr()),m_player ,m_buffer ,packet_len ) ){
					TGlobalObject::get_quque_packet_pool()->free_memory( packet );
					return ;
				}
				TGlobalObject::get_register_thread()->push_message( packet );
				break;
			}
		//获取在线人数
		case MSG_REQ_QUERY_ONLINE:
			{
				get_online();
				break;
			}
		case MSG_REQ_ENTER_ROOM:
		case MSG_REQ_ROOM_LIST:
		case MSG_REQ_CHANGE_TABLE:
		case MSG_REQ_APPLY_PWD_TABLE:
			{
				TReqPacket * packet = ( TReqPacket *)TGlobalObject::get_quque_packet_pool()->malloc_memory() ;
				if ( !packet->assign_buffer( m_player->get_table_id(),m_player ,m_buffer ,packet_len ) ){
					TGlobalObject::get_quque_packet_pool()->free_memory( packet );
					return ;
				}
				TGlobalObject::get_choose_table_thread()->push_message( packet );
				break;
			}
		case MSG_REQ_NOTIFY_CHARGE_RESULT:
		case MSG_REQ_CHARGE_INFO:
		case MSG_REQ_NOTIFY_TRADE_STATUS: 
		case MSG_REQ_CREATE_CHARGE_ORDER:
			{
				TReqPacket * packet = ( TReqPacket *)TGlobalObject::get_quque_packet_pool()->malloc_memory() ;
				if ( !packet->assign_buffer( htonl( get_address().get_long_addr()),m_player ,m_buffer ,packet_len ) ){
					TGlobalObject::get_quque_packet_pool()->free_memory( packet );
					return ;
				}
				TGlobalObject::get_charge_thread()->push_message( packet ); // TChargeThread
				break;
			}
		case MSG_HTTP_DISABLE_CONN:
			{
				TGlobalObject::get_remote_lisent_handler()->disable_connect();
				break;
			}
		case MSG_REQ_BROADCAST:
			{
				broadcast( packet_len );
				break;
			}
		case MSG_SYSTEM_BROADCAST:
			{
				system_broadcast( packet_len );
				break;
			}
		case MSG_REQ_LEAVE_ROOM:
			{
				if (m_player->get_table_id() == INVALID_TABLE ) break;
				
			}
		case MSG_REQ_INIT_ROULETTE:
			{
				init_roulette();
				break;
			}
		case MSG_REQ_INIT_SLOT:
			{
				init_slot();
				break;
			}
		case MSG_REQ_BET_ROULETTE:
			{
				bet_roulette();
				break;
			}
		
		case MSG_REQ_BET_SLOT:
			{
				bet_slot();
				break;
			}
		case MSG_REQ_COMMODITY_LIST:
			{
				get_commodity_list();
				break;
			}
		case MSG_REQ_EXCH_LIST:
			{
				get_exch_commodity_list();
				break;
			}
		case MSG_REQ_SORT:
			{
				get_ranking_list();
				break;
			}
		default:
			{
				//printf("TInputStreamsHandler packet_type = %d\n",packet_type );
				//桌子上的指令
				TReqPacket * packet = ( TReqPacket *)TGlobalObject::get_quque_packet_pool()->malloc_memory() ;
				//packet->assign_buffer( m_player->get_table_id(),m_player ,m_buffer ,packet_len );
				if ( !packet->assign_buffer( m_player->get_table_id(),m_player ,m_buffer ,packet_len ) ){
					TGlobalObject::get_quque_packet_pool()->free_memory( packet );
					return ;
				}
				scheduler_table_thread( packet ) ;
				
				break;
			}
	}
}
