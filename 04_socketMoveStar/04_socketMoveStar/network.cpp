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

/// ��������(non-blocking)�� listen ���·�
// winsock > bind > listen
void openServer()
{
	// winsock �ʱ�ȭ
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
		//wcout << L"socket() �����ڵ�: " << errCode << endl;
		return;
	}
	// non-blocking ���� ��ȯ
	u_long on = 1;
	retval = ioctlsocket(listenSock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		wcout << L"ioctlsocket() �����ڵ�: " << errCode << endl;
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
		//wcout << L"bind() �����ڵ�: " << errCode << endl;
		return;
	}
	// listen()
	retval = listen(listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		//wcout << L"listen() �����ڵ�: " << errCode << endl;
		return;
	}
}
/// Ŭ�� ���� �� �޽��� ������ / �ޱ�
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
		// sendQ�� �����Ͱ� �ִٸ� wset���
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
			//wcout << L"select() �����ڵ�: " << errCode << endl;
			return;
		}
	}
	else if (retval > 0)
	{
		// Ŭ�� ���� ����
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
			// �޽��� �ޱ�
			if (FD_ISSET(p->sock, &rset))
			{
				recvProc(p);
			}
			// �޽��� ������
			if (FD_ISSET(p->sock, &wset))
			{
				sendProc(p);
			}
		}
	}

}

/// Ŭ�� ���� ����, 
void acceptProc()
{
	SOCKET clientSock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	// 1. Ŭ���̾�Ʈ accept
	addrlen = sizeof(clientaddr);
	clientSock = accept(listenSock, (SOCKADDR*)&clientaddr, &addrlen);
	if (clientSock == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSAEWOULDBLOCK)
		{
			wcout << L"accept() �����ڵ�: " << errCode << endl;
			return;
		}
	}
	// 2. player ���� �� ���� ����
	char clientIp[IP_SIZE];
	__int32 clientPort;

	inet_ntop(AF_INET, &clientaddr.sin_addr, clientIp, IP_SIZE);
	clientPort = ntohs(clientaddr.sin_port);
	// �÷��̾� ID �Ҵ�
	Player* newPlayer = new Player();
	createPlayer(newPlayer, clientSock, clientIp, clientPort);
	// 3. newPlayer���� ID�Ҵ� �޽��� ����
	starIdAssignSend(newPlayer);
	// 4. newPlayer�� �ٸ� �����鿡�� ������(1) �޽��� ����
	starCreateSend(newPlayer);
	// 5. newPlayer���� �ٸ� ������ ���� �޽��� ����
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

/// ID �Ҵ�
void starIdAssignSend(Player* player)
{
	PPAssignStar pStar;
	pStar.type = PACKET_TYPE::TYPE_ID;
	pStar.id = player->id;
	pStar.x = player->x;
	pStar.y = player->y;
	// ������ ����
	sendUnicast((char*)&pStar, player);
}

/// �� ���� send()
void starCreateSend(const Player* player)
{
	PPCreateStar cStar;
	cStar.type = PACKET_TYPE::TYPE_CREATE;
	cStar.id = player->id;
	cStar.x = player->x;
	cStar.y = player->y;
	// ������ ����
	sendBroadcast((char*)&cStar);
}

/// �� ���� send()
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

/// �� �������� ������ ����, �����ۿ� send()�� ���� ����
void sendUnicast(const char* msg, Player* player)
{
	player->sendQ.enqueue(msg, PACKET_SIZE);
}

/// ��ü �������� send()
void sendBroadcast(const char* msg, const Player* player)
{
	// ��ü �������� boardcast
	if (player == nullptr)
	{
		for (Player* p : playerList)
		{
			if (p->isDead)
			{
				continue;
			}
			// ������ ����
			sendUnicast(msg, p);
		}
	}
	// ���޹��� player�� �����ϰ� boardcast
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
			// ������ ����
			sendUnicast(msg, p);
		}
	}
}

/// �� ���� send()
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

/// �޽��� �ޱ�
void recvProc(Player* player)
{
	// ���� �� ���� ��ŭ ��� ����
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
			// wcout << L"recv() �����ڵ�: " << errCode << endl;
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
		// recv ������ŭ ������ ����
		player->recvQ.enqueue(recvBuffer, recvRes);
	}
	// ��Ŷó��
	while (1)
	{
		int peekRes;
		// peek�� Ȯ��
		// ��Ŷ ����� PACKET_SIZE �����̱� ������ ���������ŭ ��� ����
		peekRes = player->recvQ.peek(recvBuffer, PACKET_SIZE);
		if (peekRes < PACKET_SIZE)
		{
			break;
		}
		else
		{
			// peek �� ��ŭ move
			player->recvQ.moveFront(PACKET_SIZE);
			// �̵�ó��
			// player ��ǥ ���� broadcast
			starMoveSend(player, recvBuffer);
		}

	}
}

/// �޽��� ������
void sendProc(Player* player)
{
	int sendRes;
	// send()
	// �ѹ��� ������ ���� ��ŭ �ٺ�����.
	sendRes = send(player->sock, player->sendQ.getFrontBufferPtr(), player->sendQ.directDequeueSize(), 0);
	if (sendRes == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if (errCode != WSAEWOULDBLOCK)
		{
			//wcout << L"send() �����ڵ�: " << errCode << endl;
			starDeleteSend(player);
			return;
		}
	}
	else if (sendRes > 0)
	{
		player->sendQ.moveFront(sendRes);
	}
}

/// �� �̵� send()
void starMoveSend(Player* player, char* buffer)
{
	PPMoveStar* mStar = (PPMoveStar*)buffer;
	player->x = mStar->x;
	player->y = mStar->y;
	// ������ ����
	sendBroadcast((char*)mStar, player);
}
/// ���� player ���� ����
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

/// ���� ����
void closeSocket()
{
	closesocket(listenSock);
	WSACleanup();
}
