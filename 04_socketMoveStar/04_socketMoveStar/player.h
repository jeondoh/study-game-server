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

/// 유저 생성 및 ID 할당
void createPlayer(Player* newPlayer, int sockFd, const char* ip, __int32 port);

/// 연결 클라이언트 수 랜더링
void showConnectCount();

/// 플레이어 랜더링
void renderPlayer();

/// 클라가 연결 종료 / 오류일 경우 player 상태 변경
void setPlayerDead(Player* player);

/// 특정 플레이어 삭제
list<Player*>::iterator removePlayer(list<Player*>::iterator& it);

/// 플레이어 삭제
void deleteAllPlayer();