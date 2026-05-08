#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

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

		int FirstNumber = rand() % 9999 + 1;
		int SecondNumber = rand() % 9999 + 1;
		char Operator = Operators[rand() % 5];

		sprintf_s(Message, "%d%c%d", FirstNumber, Operator, SecondNumber);

		//고정 패킷 사이즈 프로그래밍
		// 
		//Stream이니깐
		//보내기로 한 총 패킷 크기
		int WantSendBytes = TotalPacketSize;
		//지금 보낸 자료 크기
		int SentBytes = 0;
		//현재까지 보낸 자료 크기
		int TotalSentBytes = 0;

		do
		{
			SentBytes = send(ServerSocket, &Message[TotalSentBytes], WantSendBytes - TotalSentBytes, 0);
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

		//Stream이니깐
		//받기로 한 총 패킷 크기
		int WantRecvBytes = TotalPacketSize;
		//지금 받은 자료 크기
		int RecvBytes = 0;
		//현재까지 보낸 자료 크기
		int TotalRecvBytes = 0;
		do
		{
			RecvBytes = recv(ServerSocket, &Buffer[TotalRecvBytes], WantRecvBytes - TotalRecvBytes, 0);
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

		printf("%s=%s\n", Message, Buffer);

		//RecvBytes = recv(ServerSocket, Buffer, WantRecvBytes, MSG_WAITALL);
	}

	shutdown(ServerSocket, SD_BOTH);

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}