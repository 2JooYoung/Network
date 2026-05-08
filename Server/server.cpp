//server.cpp

#include <stdio.h>
#include <cstdlib>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32")



int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//TCP, Stream
	//IP 프로토콜 중 TCP를 사용하겠다 그중 STREAM을 사용하겠다
	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ListenSockAddr;
	ZeroMemory(&ListenSockAddr, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;

	//주소 막 넣으면 되니까 해킹에 취약
	//ListenSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//PVOID : void* 임
	inet_pton(AF_INET, "127.0.0.1", (PVOID)&ListenSockAddr.sin_addr.s_addr);

	ListenSockAddr.sin_port = htons(31000);


	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 0);

	while (true)
	{
		SOCKADDR_IN ClientSockAddr;
		ZeroMemory(&ListenSockAddr, sizeof(ListenSockAddr));
		int ClientSockAddrLength = sizeof(ClientSockAddr);

		//blocking
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);


		char Buffer[1024] = { 0, };

		//OS Buffer에서 가져온다
		//recv
		int RecvBytes = recv(ClientSocket, Buffer, sizeof(Buffer), 0);
		if (RecvBytes == 0)
		{
			//connection close
		}
		else if (RecvBytes < 0)
		{
			//Error
		}
		else //5byte 1,1,1,1 , Server랑 Client send, recv 횟수가 같지 않음
		{
			//네트워크 상태가 안 좋으면 1byte씩 갈수도 있다. 0보다 크다고 해서 무조건 다 받은것이 아님
		}

		//Packet Parse


		//Header 20byte, 1바이트?
		//OS Buffer 집어 넣는다. nagle algorithm
		//send
		int SentBytes = send(ClientSocket, Buffer, RecvBytes, 0);
		if (SentBytes == 0)
		{
			//OS Buffer에서 못 집어 넣음
		}
		else if (SentBytes < 0)
		{
			//Error
		}

		else
		{
			//한방에 다갈까?
		}

		shutdown(ClientSocket, SD_BOTH);
	}

	closesocket(ListenSocket);


	WSACleanup();

	return 0;
}