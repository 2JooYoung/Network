#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Packet.h"

#pragma comment(lib, "ws2_32")

#define TotalPacketSize			9

const char Operators[5] = { '+', '-', '*', '/', '%' };

int main()
{
	srand(static_cast<unsigned int>(time(nullptr)));

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	ZeroMemory(&ServerSockAddr, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (PVOID)&ServerSockAddr.sin_addr.s_addr);
	ServerSockAddr.sin_port = htons(31000);

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	while (true)
	{
		char Message[1024] = { 0, };

		int FirstNumber = rand() % 99999999 + 1;
		int SecondNumber = rand() % 99999999 + 1;
		unsigned short Operator = rand() % 5;

		//size code  first   second
		//[][] [][] [][][][] [][][][]
		PacketHeader Header;
		Header.Size = 0;
		Header.Code = Operator;

		//[][][][] [][]
		char Data[1024] = { 0, };
		int DataCursor = 0;
		int Temp = htonl(FirstNumber);
		memcpy(&Data[DataCursor], &Temp, sizeof(int));
		DataCursor += sizeof(int);

		//[][][][] [][][][]
		Temp = htonl(SecondNumber);
		memcpy(&Data[DataCursor], &Temp, sizeof(int));
		DataCursor += sizeof(int);

		Header.Size = DataCursor;

		Header.Size = htons(Header.Size);
		Header.Code = htons(Header.Code);

		//Headerº¸³½´Ù. 4
		int WantSendBytes = TotalHeaderSize;
		int SentBytes = 0;
		int TotalSentBytes = 0;

		do
		{
			SentBytes = send(ServerSocket, (char*)(&Header + TotalSentBytes), WantSendBytes - TotalSentBytes, 0);
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


		WantSendBytes = DataCursor;
		SentBytes = 0;
		TotalSentBytes = 0;

		do
		{
			SentBytes = send(ServerSocket, (char*)(&Data + TotalSentBytes), WantSendBytes - TotalSentBytes, 0);
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


		char Buffer[1024] = { 0, };

		//Header Size = 4
		int WantRecvBytes = TotalHeaderSize;
		int RecvBytes = 0;
		int TotalRecvBytes = 0;
		PacketHeader RecvHeader;
		do
		{
			RecvBytes = recv(ServerSocket, (char*)(&RecvHeader + TotalRecvBytes), WantRecvBytes - TotalRecvBytes, 0);
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
			int WantRecvBytes = RecvHeader.Size;
			int RecvBytes = 0;
			int TotalRecvBytes = 0;
			PacketHeader RecvHeader = { 0, };
			do
			{
				RecvBytes = recv(ServerSocket, (char*)(&Buffer + TotalRecvBytes), WantRecvBytes - TotalRecvBytes, 0);
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

			//[][][][] [][][][] 
			long long Result = 0;
			memcpy(&Result, Buffer, RecvHeader.Size);

			Result = ntohll(Result);

			printf("%s=%s\n", Message, Buffer);
		}

		//RecvBytes = recv(ServerSocket, Buffer, WantRecvBytes, MSG_WAITALL);
	}

	shutdown(ServerSocket, SD_BOTH);

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}