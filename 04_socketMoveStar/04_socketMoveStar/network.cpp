#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "packetProtocol.h"
#include "network.h"
#include "game.h"
#include "player.h"
#include "ringbuffer.h"

using namespace std;

SOCKET listenSock;
int retval;

/// 서버소켓(non-blocking)을 listen 상태로
// winsock > bind > listen
void openServer()
{
	// winsock 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}
	// socket()
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
	{
		int errCode = WSAGetLastError();
		//wcout << L"socket() 에러코드: " << errCode << endl;
		return;
	}
	// non-blocking 소켓 전환
	u_long on = 1;
	retval = ioctlsocket(listenSock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		wcout << L"ioctlsocket() 에러코드: " << errCode << endl;
		return;
	}
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = bind(listenSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		//wcout << L"bind() 에러코드: " << errCode << endl;
		return;
	}
	// listen()
	retval = listen(listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		//wcout << L"listen() 에러코드: " << errCode << endl;
		return;
	}
}
/// 클라 수용 및 메시지 보내기 / 받기
void acceptClient()
{
	FD_SET rset;
	FD_SET wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(listenSock, &rset);
	for (Player* p : playerList)
	{
		if (p->isDead)
		{
			continue;
		}
		FD_SET(p->sock, &rset);
		// sendQ에 데이터가 있다면 wset등록
		if (p->sendQ.getUseSize() > 0)
		{
			FD_SET(p->sock, &wset);
		}
	}
	// select()
	TIMEVAL timeval;
	timeval.tv_sec = 0;
	timeval.tv_usec = 0;
	retval = select(0, &rset, &wset, NULL, &timeval);
	if (retval == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSAEWOULDBLOCK)
		{
			//wcout << L"select() 에러코드: " << errCode << endl;
			return;
		}
	}
	else if (retval > 0)
	{
		// 클라 접속 수용
		if (FD_ISSET(listenSock, &rset))
		{
			acceptProc();
		}
		for (Player* p : playerList)
		{
			if (p->isDead)
			{
				continue;
			}
			// 메시지 받기
			if (FD_ISSET(p->sock, &rset))
			{
				recvProc(p);
			}
			// 메시지 보내기
			if (FD_ISSET(p->sock, &wset))
			{
				sendProc(p);
			}
		}
	}

}

/// 클라 접속 수용, 
void acceptProc()
{
	SOCKET clientSock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	// 1. 클라이언트 accept
	addrlen = sizeof(clientaddr);
	clientSock = accept(listenSock, (SOCKADDR*)&clientaddr, &addrlen);
	if (clientSock == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSAEWOULDBLOCK)
		{
			wcout << L"accept() 에러코드: " << errCode << endl;
			return;
		}
	}
	// 2. player 생성 및 정보 세팅
	char clientIp[IP_SIZE];
	__int32 clientPort;

	inet_ntop(AF_INET, &clientaddr.sin_addr, clientIp, IP_SIZE);
	clientPort = ntohs(clientaddr.sin_port);
	// 플레이어 ID 할당
	Player* newPlayer = new Player();
	createPlayer(newPlayer, clientSock, clientIp, clientPort);
	// 3. newPlayer에게 ID할당 메시지 전송
	starIdAssignSend(newPlayer);
	// 4. newPlayer와 다른 유저들에게 별생성(1) 메시지 전송
	starCreateSend(newPlayer);
	// 5. newPlayer에게 다른 유저의 정보 메시지 전송
	for (Player* p : playerList)
	{
		if (p->isDead)
		{
			continue;
		}
		if (newPlayer->id != p->id)
		{
			starCreateUnicastSend(newPlayer, p);
		}
	}
}

/// ID 할당
void starIdAssignSend(Player* player)
{
	PPAssignStar pStar;
	pStar.type = PACKET_TYPE::TYPE_ID;
	pStar.id = player->id;
	pStar.x = player->x;
	pStar.y = player->y;
	// 링버퍼 저장
	sendUnicast((char*)&pStar, player);
}

/// 별 생성 send()
void starCreateSend(const Player* player)
{
	PPCreateStar cStar;
	cStar.type = PACKET_TYPE::TYPE_CREATE;
	cStar.id = player->id;
	cStar.x = player->x;
	cStar.y = player->y;
	// 링버퍼 저장
	sendBroadcast((char*)&cStar);
}

/// 별 생성 send()
void starCreateUnicastSend(Player* player, const Player* otherPlayer)
{
	PPCreateStar cStar;
	cStar.type = PACKET_TYPE::TYPE_CREATE;
	cStar.id = otherPlayer->id;
	cStar.x = otherPlayer->x;
	cStar.y = otherPlayer->y;
	// send()
	sendUnicast((char*)&cStar, player);
}

/// 한 유저에게 보내기 위함, 링버퍼에 send()할 내용 저장
void sendUnicast(const char* msg, Player* player)
{
	player->sendQ.enqueue(msg, PACKET_SIZE);
}

/// 전체 유저에게 send()
void sendBroadcast(const char* msg, const Player* player)
{
	// 전체 유저에게 boardcast
	if (player == nullptr)
	{
		for (Player* p : playerList)
		{
			if (p->isDead)
			{
				continue;
			}
			// 링버퍼 저장
			sendUnicast(msg, p);
		}
	}
	// 전달받은 player를 제외하고 boardcast
	else
	{
		for (Player* p : playerList)
		{
			if (player->id == p->id)
			{
				continue;
			}
			if (p->isDead)
			{
				continue;
			}
			// 링버퍼 저장
			sendUnicast(msg, p);
		}
	}
}

/// 별 삭제 send()
void starDeleteSend(Player* player)
{
	setPlayerDead(player);

	PPDeleteStar dStar;
	dStar.type = PACKET_TYPE::TYPE_DELETE;
	dStar.id = player->id;
	dStar.x = player->x;
	dStar.y = player->y;

	sendBroadcast((char*)&dStar, player);
}

/// 메시지 받기
void recvProc(Player* player)
{
	// 받을 수 있을 만큼 모두 받음
	char recvBuffer[DEFAULT_SIZE];
	int recvRes;
	int freeSize = player->recvQ.getFreeSize();
	// recv()
	recvRes = recv(player->sock, recvBuffer, freeSize, 0);
	if (recvRes == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSAEWOULDBLOCK)
		{
			// wcout << L"recv() 에러코드: " << errCode << endl;
			starDeleteSend(player);
			return;
		}
	}
	else if (recvRes == 0)
	{
		starDeleteSend(player);
	}
	else if (recvRes > 0)
	{
		// recv 받은만큼 링버퍼 저장
		player->recvQ.enqueue(recvBuffer, recvRes);
	}
	// 패킷처리
	while (1)
	{
		int peekRes;
		// peek로 확인
		// 패킷 사이즈가 PACKET_SIZE 고정이기 때문에 고정사이즈만큼 계속 꺼냄
		peekRes = player->recvQ.peek(recvBuffer, PACKET_SIZE);
		if (peekRes < PACKET_SIZE)
		{
			break;
		}
		else
		{
			// peek 한 만큼 move
			player->recvQ.moveFront(PACKET_SIZE);
			// 이동처리
			// player 좌표 변경 broadcast
			starMoveSend(player, recvBuffer);
		}

	}
}

/// 메시지 보내기
void sendProc(Player* player)
{
	int sendRes;
	// send()
	// 한번에 보낼수 있을 만큼 다보낸다.
	sendRes = send(player->sock, player->sendQ.getFrontBufferPtr(), player->sendQ.directDequeueSize(), 0);
	if (sendRes == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSAEWOULDBLOCK)
		{
			//wcout << L"send() 에러코드: " << errCode << endl;
			starDeleteSend(player);
			return;
		}
	}
	else if (sendRes > 0)
	{
		player->sendQ.moveFront(sendRes);
	}
}

/// 별 이동 send()
void starMoveSend(Player* player, char* buffer)
{
	PPMoveStar* mStar = (PPMoveStar*)buffer;
	player->x = mStar->x;
	player->y = mStar->y;
	// 링버퍼 저장
	sendBroadcast((char*)mStar, player);
}
/// 죽은 player 연결 끊기
void disConnect()
{
	for (list<Player*>::iterator iter = playerList.begin(); iter != playerList.end();)
	{
		if ((*iter)->isDead)
		{
			Player* player = *iter;
			closesocket(player->sock);
			iter = removePlayer(iter);
		}
		else
		{
			++iter;
		}
	}
}

/// 소켓 종료
void closeSocket()
{
	closesocket(listenSock);
	WSACleanup();
}
