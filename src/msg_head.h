#pragma once

#pragma pack(push, 1)
struct GMsgHead
{
	uint32_t len;		//这个长度是指报文体的长度，没有包括报文头的长度
};
#pragma pack(pop)
