#include <iostream>
#include <WinSock2.h>	//윈도우 환경이라

#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	//winsock.dll 로딩. 소켓 쓸수 있도록
	//ws2_32.dll 로딩, winsock -> bsd socket 윈도우에서 구현체
	WSAData wsaData;

	//초기화 함수. 윈도우라서 함
	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData); //옛날에 만들어진거라 소수연산 못해서 2.2 이렇게 씀


	if (Result != 0)
	{
		cout << "WSAStartup Error" << GetLastError() << endl;
		exit(-1);
	}

	//여기서 부터는 리눅스랑 같음
	//INET 형태로 TCP 소켓 만들어줘
	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "socket Error" << WSAGetLastError() << endl;
		exit(-1);
	}


	//학습용으로 LAN카드 하나
	//나중에는 이렇게 하면 안됨
	SOCKADDR_IN ListenSockAddr; //12바이트
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr)); //ListenSockAddr 비우기.

	//내가 쓸 소켓과 연결해줘
	ListenSockAddr.sin_family = AF_INET; //ipv4
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;
	ListenSockAddr.sin_port = htons(1234); //포트번호

	Result = bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	if (Result == SOCKET_ERROR)
	{
		cout << "bind Error" << WSAGetLastError() << endl;
		exit(-1);
	}

	//전화해
	Result = listen(ListenSocket, SOMAXCONN);
	if (Result == SOCKET_ERROR)
	{
		cout << "listen Error" << WSAGetLastError() << endl;
		exit(-1);
	}

	//받아
	SOCKADDR_IN ClientSockAddr;
	memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
	//외부 주소는 크기 다를 수 있기 때문에 확인
	int LengthClientSockAddr = sizeof(ClientSockAddr);


	while (true)
	{
		//blocking함수
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &LengthClientSockAddr);
		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "accept Error" << WSAGetLastError() << endl;
			exit(-1);
		}

		char Buffer[1024] = { 0, };

		//blocking 기본. 자료 올때까지 멈춰있음
		int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);

		//상대방이 전화 끊으면 정상종료 0
		if (RecvLength == 0)
		{
			cout << "disconnect" << WSAGetLastError() << endl;
			exit(-1);
		}

		//음수면 오류
		else if (RecvLength < 0)
		{
			cout << "disconnect" << endl;
			exit(-1);
		}

		//양수면 자료 받았다는 뜻
		cout << "client send data : " << Buffer << endl;

		int SentLength = send(ClientSocket, Buffer, sizeof(Buffer), 0);
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


		closesocket(ClientSocket);
	}


	closesocket(ListenSocket);





	//소켓이랑 밖의 주소랑 연결
	//bind (ListenSocket)

	WSACleanup();

	return 0;

}