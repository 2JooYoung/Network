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

#define TotalHeaderSize				4


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

#endif //__PACKET_H__
