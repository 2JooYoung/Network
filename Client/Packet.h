#pragma once

#ifndef __PACKET_H__
#define __PACKET_H__

enum class PacketType : unsigned short
{
	Plus = 0,
	Minus,
	Multiply,
	Divide,
	Remainder,
	Result,
	Max
};


#pragma pack(push, 1)
typedef struct PacketHeader
{
	unsigned short Size;
	unsigned short Code;
} PACKETHEADER;


struct TwoNumber
{
	int First;
	int Second;
};
#pragma pack(pop)


#endif //__PACKET_H__