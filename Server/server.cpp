#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>

#include "Packet.h"

#pragma comment(lib, "ws2_32")

void SendAll(SOCKET ReceiverSocket, char* Data, int Size)
{
    int TotalSendDataSize = 0;
    int WantSendDataSize = Size;
    int SentBytes = 0;
    int Count = 0;
    do
    {
        SentBytes = send(ReceiverSocket, Data + TotalSendDataSize, WantSendDataSize - TotalSendDataSize, 0);
        TotalSendDataSize += SentBytes;
        printf("Send %dBytes %d Count\n", SentBytes, ++Count);
    } while (TotalSendDataSize < WantSendDataSize);
}


int main()
{
    int PlayerX = 0;
    int PlayerY = 0;

    WSAData WsaData;
    WSAStartup(MAKEWORD(2, 2), &WsaData);

    SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN ListenAddr;
    ZeroMemory(&ListenAddr, sizeof(ListenAddr));
    ListenAddr.sin_family      = AF_INET;
    ListenAddr.sin_addr.s_addr = INADDR_ANY;
    ListenAddr.sin_port        = htons(31000);

    bind(ListenSocket, (SOCKADDR*)&ListenAddr, sizeof(ListenAddr));
    listen(ListenSocket, 5);

    printf("Server started. Port 31000. Waiting for client...\n");

    while (true)
    {
        // 클라이언트 접속 대기 (블로킹)
        SOCKADDR_IN ClientAddr;
        ZeroMemory(&ClientAddr, sizeof(ClientAddr));
        int ClientAddrLen = sizeof(ClientAddr);
        SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientAddr, &ClientAddrLen);

        char ClientIP[64] = { 0, };
        inet_ntop(AF_INET, &ClientAddr.sin_addr, ClientIP, sizeof(ClientIP));
        printf("Client connected: %s\n", ClientIP);

        PacketHeader SendHeader;
        PositionData SendPos;
        int WantSend = 0;
        int TotalSent = 0;
        int Sent = 0;
        int Running = 1;

        // -------------------------------------------------------
        // 이동 패킷 수신 루프
        // -------------------------------------------------------
        while (Running)
        {
            // 헤더 수신 (블로킹)
            PacketHeader RecvHeader;
            int RecvBytes = recv(ClientSocket, (char*)&RecvHeader, sizeof(RecvHeader), MSG_WAITALL);
            if (RecvBytes <= 0)
            {
                printf("Client disconnected.\n");
                break;
            }

            RecvHeader.Size = ntohl(RecvHeader.Size);
            RecvHeader.Code = ntohs(RecvHeader.Code);




            // 이동 처리
            if ((PacketType)RecvHeader.Code == PacketType::Move)
            {
                // 데이터 수신 (블로킹)
                MoveData Move;
                recv(ClientSocket, (char*)&Move, (int)RecvHeader.Size, MSG_WAITALL);

                int NewX = PlayerX;
                int NewY = PlayerY;

                switch (Move.Dir)
                {
                case 'W':
                case 'w':
                    NewY--;
                    break;
                case 'S':
                case 's':
                    NewY++;
                    break;
                case 'A':
                case 'a':
                    NewX--;
                    break;
                case 'D':
                case 'd':
                    NewX++;
                    break;
                }

                PlayerX = NewX;
                PlayerY = NewY;

                printf("Player [%c] -> (%d, %d)\n", Move.Dir, PlayerX, PlayerY);

                // 갱신된 위치 전송 (헤더)
                SendHeader.Size = htonl(sizeof(PositionData));
                SendHeader.Code = htons((unsigned short)PacketType::Position);

                SendPos.X = htonl((u_long)PlayerX);
                SendPos.Y = htonl((u_long)PlayerY);

                WantSend = sizeof(SendHeader);
                TotalSent = 0;

                do
                {
                    Sent = send(ClientSocket, (char*)&SendHeader + TotalSent, WantSend - TotalSent, 0);
                    if (Sent <= 0)
                    {
                        printf("send error\n");
                        Running = 0;
                        break;
                    }
                    TotalSent += Sent;
                } while (TotalSent < WantSend);

                if (!Running)
                {
                    break;
                }

                // 갱신된 위치 전송 (데이터)
                WantSend = sizeof(SendPos);
                TotalSent = 0;

                do
                {
                    Sent = send(ClientSocket, (char*)&SendPos + TotalSent, WantSend - TotalSent, 0);
                    if (Sent <= 0)
                    {
                        printf("send error\n");
                        Running = 0;
                        break;
                    }
                    TotalSent += Sent;
                } while (TotalSent < WantSend);
            }
            else if ((PacketType)RecvHeader.Code == PacketType::C2S_File)
            {
                //서버에서 파일 보내줌


                FILE* InputFile = fopen("카네이션.png", "rb");

                fseek(InputFile, 0, SEEK_END);
                unsigned long FileSize = ftell(InputFile);
                fseek(InputFile, 0, SEEK_SET);

                //header 전송
                PacketHeader FileHeader;
                FileHeader.Size = FileSize;
                FileHeader.Code = static_cast<unsigned int>(PacketType::S2C_File);

                FileHeader.Size = htonl(FileHeader.Size);
                FileHeader.Code = htons(FileHeader.Code);

                SendAll(ClientSocket, (char*)&FileHeader, sizeof(FileHeader));


                char Buffer[10240] = { 0, };
                size_t ReadSize = 0;
                int Count = 0;
                do
                {
                    printf("%d\n", ++Count);
                    ReadSize = fread(Buffer, sizeof(char), sizeof(Buffer), InputFile);
                    int SentBytes = send(ClientSocket, Buffer, (int)ReadSize, 0);
                    if (SentBytes <= 0)
                    {
                        break;
                    }

                } while (ReadSize > 0);

                fclose(InputFile);
            }
        }

        shutdown(ClientSocket, SD_BOTH);
        closesocket(ClientSocket);
    }


    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
