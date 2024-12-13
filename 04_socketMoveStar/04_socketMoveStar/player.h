#pragma once

#include "list"
#include "packetProtocol.h"
#include "ringbuffer.h"

using namespace std;

struct Player
{
	bool isDead;
	__int32 id;
	__int32 x;
	__int32 y;

	__int32 port;
	int sock;
	char ip[IP_SIZE];

	RingBuffer recvQ;
	RingBuffer sendQ;
};

extern list<Player*> playerList;

/// ���� ���� �� ID �Ҵ�
void createPlayer(Player* newPlayer, int sockFd, const char* ip, __int32 port);

/// ���� Ŭ���̾�Ʈ �� ������
void showConnectCount();

/// �÷��̾� ������
void renderPlayer();

/// Ŭ�� ���� ���� / ������ ��� player ���� ����
void setPlayerDead(Player* player);

/// Ư�� �÷��̾� ����
list<Player*>::iterator removePlayer(list<Player*>::iterator& it);

/// �÷��̾� ����
void deleteAllPlayer();