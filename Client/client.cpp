#define _WINSOCK_DEPRECATED_NO_WARNINGS
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



	char Buffer[1024] = "Hello World";
	int SentLength = send(ServerSocket, Buffer, sizeof(Buffer), 0);
	if (SentLength == 0)
	{
		cout << "send disconnect " << endl;
		exit(-1);
	}
	else if (SentLength < 0)
	{
		cout << "send Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	//blocking
	int RecvLength = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
	if (RecvLength == 0)
	{
		cout << "recv disconnect " << endl;
		exit(-1);
	}
	else if (RecvLength < 0)
	{
		cout << "recv Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	cout << "server send data : " << Buffer << endl;


	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}