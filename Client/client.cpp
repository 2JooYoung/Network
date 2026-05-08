#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Packet.h"

#pragma comment(lib, "ws2_32")


const char Operators[5] = { '+', '-', '*', '/', '%' };

int main3()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hostent* HostInfo = gethostbyname("naver.com");

	printf("%s\n", HostInfo->h_name);
	printf("%s\n", HostInfo->h_addrtype == AF_INET ? "IP v4" : "IP v6");

	while (*HostInfo->h_aliases != nullptr)
	{
		printf("%s\n", *HostInfo->h_aliases);
		HostInfo->h_aliases++;
	}

	//[][][]

	//ip v4
	//[][][][]
	//[][][][]
	//[][][][]
	//ip v6
	//[][][][][][][][][][][][][][][][]
	//[][][][][][][][][][][][][][][][]

	while (*HostInfo->h_addr_list != nullptr)
	{
		IN_ADDR Addr;
		Addr.s_addr = *(ULONG*)*HostInfo->h_addr_list;
		printf("%s\n", inet_ntoa(Addr));
		HostInfo->h_addr_list++;
	}


	WSACleanup();

	return 0;
}


int main()
{
	srand(static_cast<unsigned int>(time(nullptr)));

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hostent* HostInfo = gethostbyname("login.calculate.edu");

	char ServerIP[1024] = { 0, };
	IN_ADDR Addr;
	Addr.s_addr = *(ULONG*)*HostInfo->h_addr_list;
	sprintf_s(ServerIP, "%s", inet_ntoa(Addr));
	printf("%s\n", ServerIP);



	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	ZeroMemory(&ServerSockAddr, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ServerIP, (PVOID)&ServerSockAddr.sin_addr.s_addr);
	ServerSockAddr.sin_port = htons(31000);

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	while (true)
	{
		int FirstNumber = rand() % (RAND_MAX - 1) + 1;
		int SecondNumber = rand() % (RAND_MAX - 1) + 1;
		unsigned short OperatorIndex = rand() % 5;

		printf("%d%c%d", FirstNumber, Operators[OperatorIndex], SecondNumber);

		//size code  first   second
		//[][] [][] [][][][] [][][][]
		PacketHeader Header;
		Header.Size = sizeof(int) + sizeof(int);
		Header.Code = OperatorIndex;



		//byte order : host to network
		Header.Size = htons(Header.Size);
		Header.Code = htons(Header.Code);

		//Headerº¸³½´Ù. 4
		int WantSendBytes = sizeof(Header);
		int SentBytes = 0;
		int TotalSentBytes = 0;

		do
		{
			SentBytes = send(ServerSocket, (char*)(&Header) + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
			if (SentBytes == 0)
			{
				printf("connection close");
				exit(-1);
			}
			else if (SentBytes < 0)
			{
				printf("send error");
				exit(-1);
			}
			TotalSentBytes += SentBytes;
		} while (TotalSentBytes < WantSendBytes);

		
		//Data
		//[][][][] 
		char Data[1024] = { 0, };
		int DataCursor = 0;
		int Temp = htonl(FirstNumber);
		memcpy(&Data[DataCursor], &Temp, sizeof(int));
		DataCursor += sizeof(int);

		//[][][][] [][][][]
		Temp = htonl(SecondNumber);
		memcpy(&Data[DataCursor], &Temp, sizeof(int));
		DataCursor += sizeof(int);

		WantSendBytes = ntohs(Header.Size);
		SentBytes = 0;
		TotalSentBytes = 0;

		do
		{
			SentBytes = send(ServerSocket, (char*)(&Data) + TotalSentBytes, WantSendBytes - TotalSentBytes, 0);
			if (SentBytes == 0)
			{
				printf("connection close");
				exit(-1);
			}
			else if (SentBytes < 0)
			{
				printf("send error");
				exit(-1);
			}
			TotalSentBytes += SentBytes;
		} while (TotalSentBytes < WantSendBytes);


		PacketHeader RecvHeader;
		int WantRecvBytes = sizeof(RecvHeader);
		int RecvBytes = 0;
		int TotalRecvBytes = 0;
		do
		{
			RecvBytes = recv(ServerSocket, (char*)(&RecvHeader) + TotalRecvBytes, WantRecvBytes - TotalRecvBytes, 0);
			if (RecvBytes == 0)
			{
				printf("recv connection close");
				exit(-1);
			}
			else if (RecvBytes < 0)
			{
				printf("recv error");
				exit(-1);
			}
			TotalRecvBytes += RecvBytes;
		} while (TotalRecvBytes < WantRecvBytes);

		RecvHeader.Size = ntohs(RecvHeader.Size);
		RecvHeader.Code = ntohs(RecvHeader.Code);

		if (static_cast<PacketType>(RecvHeader.Code) == PacketType::Result)
		{
			//Data
			long long Result = 0;

			int WantRecvBytes = RecvHeader.Size;
			int RecvBytes = 0;
			int TotalRecvBytes = 0;
			PacketHeader RecvHeader = { 0, };
			do
			{
				RecvBytes = recv(ServerSocket, (char*)(&Result) + TotalRecvBytes, WantRecvBytes - TotalRecvBytes, 0);
				if (RecvBytes == 0)
				{
					printf("recv connection close");
					exit(-1);
				}
				else if (RecvBytes < 0)
				{
					printf("recv error");
					exit(-1);
				}
				TotalRecvBytes += RecvBytes;
			} while (TotalRecvBytes < WantRecvBytes);

			Result = ntohll(Result);

			printf("=%lld\n", Result);
		}

		//RecvBytes = recv(ServerSocket, Buffer, WantRecvBytes, MSG_WAITALL);
	}

	shutdown(ServerSocket, SD_BOTH);

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}