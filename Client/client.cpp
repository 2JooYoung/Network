//Client.cpp

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

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "socket Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //설정
	ServerSockAddr.sin_port = htons(1234);

	//blocking
	Result = connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));
	if (Result == SOCKET_ERROR)
	{
		cout << "bind Error " << WSAGetLastError() << endl;
		exit(-1);
	}


	FILE* OutputFile = fopen("flower_2.png", "wb");

	char Buffer[1] = { 0, };
	size_t WriteSize = 0;
	int Count = 0;

	do
	{
		cout << ++Count << endl;

		int RecvBytes = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
		if (RecvBytes <= 0)
		{
			break;
		}

		WriteSize = fwrite(Buffer, sizeof(char), RecvBytes, OutputFile);
	} while (WriteSize > 0);

	fclose(OutputFile);

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}