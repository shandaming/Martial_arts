/*
 * Copyright (C) 2018
 */

#ifndef PACKET_PROTOCOL_H
#define PACKET_PROTOCOL_H

#include <cstdint>

//包头标志
#define PACKET_HEAD_FLAG	0x80

//包尾标志,2个字节
#define PACKET_EOF_FLAG		0x8080

#define RESULT_OK		0x01 //通知结果 OK
#define RESULT_FAILD		0x00 //通知结果 失败

//包头部长度
#define PACKET_HEAD_LEN		sizeof(Packet_head)


#pragma pack(1)

/*
 * 通知结构
 *
 * 网络包发送并处理后是否回复请求者
 */
enum Notice
{
	NOTICE_REQUEST = 0 ,	//请求,必须回复
	NOTICE_RESPONSE,		//回复
	NOTICE_NOTIFY,			//通知不需要回复
	NOTICE_NOTIFY_RESPONSE	//通知一定要回复
};

/*
 * 帧结构 2字节
 */
struct Frame
{
	uint16_t compress : 1;		// 是否使用压缩： 0 - 不使用，1 -使用
	uint16_t encrypt : 1;		// 加密算法： 0 - 不使用，1 - 使用
	uint16_t encrypt_method : 3;	// 加密算法方式: 0 -RC4????
	uint16_t notice_flag : 2;	//0：请求,必须回复；1：回复，2：通知不需要回复；3: 通知一定要回复
	uint16_t protocol_type	: 1;	//0：tcp；1：udp
	uint16_t request_type : 2;		//0：正常请求，1：转发,2:转发再转发
};

//包头部信息 9字节
struct Packet_head
{
	uint16_t len;			//数据长度 不包含包头和2字节包尾标志长度
	uint8_t version;		//协议版本号	
	uint16_t msg_type;		//帧类型
	Frame flag;				//帧标志位
	uint16_t packet_seq;	//包序号
	uint8_t data [] ;		//数据部分,占位
};

#pragma pack()

#endif
