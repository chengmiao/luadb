#pragma once
#include <string>

//消息类型，占用4bit
enum ENUM_MSG_TYPE
{
	MSG_TYPE_NONE			= 0,	//未知
	MSG_TYPE_EVENT			= 1,	//网络事件
	MSG_TYPE_CLIENT			= 2,	//客户端
	MSG_TYPE_GATE			= 3,	//网关
	MSG_TYPE_RANK_SERVER	= 4,	//排行榜服务器
	MSG_TYPE_CENTER_SERVER	= 5,	//中央服务器
	MSG_TYPE_BALANCE_SERVER	= 6,	//负载均衡服务器
	MSG_TYPE_COUNT,
};

#define SET_MSG_ID(type, id)	(((type << 12) & 0xf000) | (id & 0x0fff))
#define GET_MSG_TYPE(msgid)		((msgid >> 12) & 0x000f)
#define GET_MSG_ID(msgid)		(msgid & 0x0fff)

#define MSG_ID uint32_t
#define SESSION_ID uint64_t

#define MAX_PACKET_LEN (64 * 1024)

#pragma pack(push, 1)
struct NetHead
{
	uint32_t len;		//这个长度是指报文体的长度，没有包括报文头的长度
	//uint32_t msg_id;
};
#pragma pack(pop)

const size_t NET_HEAD_SIZE = sizeof(NetHead);

struct LogicPack
{
	SESSION_ID	sid;
	MSG_ID		msg_id;
	std::string data;
};

