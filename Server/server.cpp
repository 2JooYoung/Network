//Server.cpp

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	//ws2_32.dll 로딩, winsock -> bsd socket 윈도우에서 구현체
	WSAData  wsaData;

	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (Result != 0)
	{
		cout << "WSAStartup Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "socket Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //설정
	ListenSockAddr.sin_port = htons(1234);

	Result = bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));
	if (Result == SOCKET_ERROR)
	{
		cout << "bind Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	Result = listen(ListenSocket, SOMAXCONN);
	if (Result == SOCKET_ERROR)
	{
		cout << "listen Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ClientSockAddr;
	memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
	int LengthClientSockAddr = sizeof(ClientSockAddr);


	while (true)
	{
		//blocking 함수
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &LengthClientSockAddr);

		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "accept Error " << WSAGetLastError() << endl;
			exit(-1);
		}

		FILE* InputFile = fopen("flower.png", "rb");

		char Buffer[10240] = { 0, };
		size_t ReadSize = 0;
		int Count = 0;
		do
		{
			cout << ++Count << endl;
			ReadSize = fread(Buffer, sizeof(char), sizeof(Buffer), InputFile);
			int SentBytes = send(ClientSocket, Buffer, ReadSize, 0);
			if (SentBytes <= 0)
			{
				break;
			}

		} while (ReadSize > 0);

		fclose(InputFile);


		closesocket(ClientSocket);
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}