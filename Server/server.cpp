#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#define TotalPacketSize			9

const char Operators[5] = { '+', '-', '*', '/', '%' };



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
	inet_pton(AF_INET, "127.0.0.1", (PVOID)&ListenSockAddr.sin_addr.s_addr);

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
			char Buffer[1024] = { 0, };
			int WantSendBytes = TotalPacketSize;

			//recv
			int RecvBAytes = recv(ClientSocket, Buffer, WantSendBytes, MSG_WAITALL);
			if (RecvBAytes <= 0)
			{
				break;
			}

			//process
			std::string Packet(Buffer);

			int OperatorPosition = 0;
			char Operator = 0;

			for (const auto& CheckOperator : Operators)
			{
				OperatorPosition = static_cast<int>(Packet.find(CheckOperator));
				Operator = CheckOperator;
				if (OperatorPosition != std::string::npos)
				{
					break;
				}
			}

			std::string FirstStringNumber = Packet.substr(0, OperatorPosition);
			std::string SecondStringNumber = Packet.substr(OperatorPosition + 1, Packet.length() - OperatorPosition);

			//std::cout << FirstStringNumber << "+" << SecondStringNumber << std::endl;

			int FirstNumber = std::stoi(FirstStringNumber);
			int SecondNumber = std::stoi(SecondStringNumber);

			int Result = 0;

			switch (Operator)
			{
			case '+':
				Result = FirstNumber + SecondNumber;
				break;
			case '-':
				Result = FirstNumber - SecondNumber;
				break;
			case '/':
				Result = FirstNumber / SecondNumber;
				break;
			case '*':
				Result = FirstNumber * SecondNumber;
				break;
			case '%':
				Result = FirstNumber % SecondNumber;
				break;
			}

			printf("%d%c%d=%d\n", FirstNumber, Operator, SecondNumber, Result);

			//send
			int SentBytes = 0;
			int TotalSentBytes = 0;

			char Message[1024] = { 0, };

			sprintf_s(Message, "%d", Result);

			do
			{
				SentBytes = send(ClientSocket, &Message[TotalSentBytes], WantSendBytes - TotalSentBytes, 0);
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
		}

		printf("send complete");
		shutdown(ClientSocket, SD_BOTH);
		closesocket(ClientSocket);
	}



	closesocket(ListenSocket);



	WSACleanup();

	return 0;
}