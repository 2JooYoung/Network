#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Packet.h"

#pragma comment(lib, "ws2_32")

#define TotalPacketSize			9

const char Operators[5] = { '+', '-', '*', '/', '%' };


//int main()
//{
//	//host byte order(little, big)
//	int Data = 0x12345678;
//
//	//network byte order(big endian)
//	printf("%x\n", Data);
//	printf("%x\n", htonl(Data));
//	printf("%x\n", ntohl( htonl(Data)));
//}

//size code data
//[][] [][] [][][][][][]..


int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//TCP, Stream
	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ListenSockAddr;
	ZeroMemory(&ListenSockAddr, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;
	//ListenSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_pton(AF_INET, "192.168.0.95", (PVOID)&ListenSockAddr.sin_addr.s_addr);

	ListenSockAddr.sin_port = htons(31000);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 0);

	while (true)
	{
		SOCKADDR_IN ClientSockAddr;
		ZeroMemory(&ClientSockAddr, sizeof(ClientSockAddr));
		int ClientSockAddrLength = sizeof(ClientSockAddr);
		//bloking
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);
		while (true)
		{
			PacketHeader Header;
			//header
			int RecvBAytes = recv(ClientSocket, (char*)&Header, sizeof(Header), MSG_WAITALL);
			if (RecvBAytes <= 0)
			{
				break;
			}

			Header.Size = ntohs(Header.Size);
			Header.Code = ntohs(Header.Code);

			long long Result = 0;

			//[][][][] [][][][]
			TwoNumber Data;
			recv(ClientSocket, (char*)&Data, Header.Size, MSG_WAITALL);
			Data.First = ntohl(Data.First);
			Data.Second = ntohl(Data.Second);

			switch (static_cast<PacketType>(Header.Code))
			{
			case PacketType::Plus:
				Result = Data.First + Data.Second;
				break;
			case PacketType::Minus:
				Result = Data.First - Data.Second;
				break;
			case PacketType::Divide:
				Result = Data.First / Data.Second;
				break;
			case PacketType::Multiply:
				Result = Data.First * Data.Second;
				break;
			case PacketType::Remainder:
				Result = Data.First % Data.Second;
				break;
			}


			printf("%d%c%d=%lld\n", Data.First, Operators[Header.Code], Data.Second, Result);

			PacketHeader SendPacketHeader;
			SendPacketHeader.Size = sizeof(Result);
			SendPacketHeader.Code = static_cast<unsigned short>(PacketType::Result);

			SendPacketHeader.Size = htons(SendPacketHeader.Size);
			SendPacketHeader.Code = htons(SendPacketHeader.Code);

			int WantSendBytes = sizeof(SendPacketHeader);
			int SentBytes = 0;
			int TotalSentBytes = 0;
			do
			{
				SentBytes = send(ClientSocket, (char*)(&SendPacketHeader) + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
				if (SentBytes == 0)
				{
					printf("connection close");
					break;
				}
				else if (SentBytes < 0)
				{
					printf("send error");
					break;
				}
				TotalSentBytes += SentBytes;
			} while (TotalSentBytes < WantSendBytes);


			//Data
			//[][][][] [][][][]
			Result = htonll(Result);

			WantSendBytes = sizeof(Result);
			SentBytes = 0;
			TotalSentBytes = 0;
			do
			{
				SentBytes = send(ClientSocket, (char*)(&Result) + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
				if (SentBytes == 0)
				{
					printf("connection close");
					break;
				}
				else if (SentBytes < 0)
				{
					printf("send error");
					break;
				}
				TotalSentBytes += SentBytes;
			} while (TotalSentBytes < WantSendBytes);
		}

		shutdown(ClientSocket, SD_BOTH);
		closesocket(ClientSocket);
	}



	closesocket(ListenSocket);



	WSACleanup();

	return 0;
}