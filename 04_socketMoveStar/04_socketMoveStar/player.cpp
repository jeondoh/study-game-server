#include <iostream>
#include "player.h"
#include "console.h"

__int32 playerId = 0;
list<Player*> playerList;

/// 유저 생성 및 ID 할당
void createPlayer(Player* newPlayer, int sockFd, const char* ip, __int32 port)
{
	newPlayer->isDead = false;
	newPlayer->id = playerId;
	newPlayer->x = dfSCREEN_WIDTH / 2;
	newPlayer->y = dfSCREEN_HEIGHT / 2;
	newPlayer->sock = sockFd;
	strncpy_s(newPlayer->ip, ip, IP_SIZE);
	newPlayer->port = port;
	++playerId;

	playerList.push_back(newPlayer);
}

/// 연결 클라이언트 수 랜더링
void showConnectCount()
{
	wcout << L"" << endl;
	wcout << L"" << endl;
	wcout << L"" << endl;
	wcout << L"Connect Client: " << playerList.size() << endl;
}

/// 플레이어 랜더링
void renderPlayer()
{
	// 연결 클라이언트 수 랜더링
	showConnectCount();
	// 상대 플레이어 이동
	for (Player* p : playerList)
	{
		if (!(p->isDead))
		{
			spriteStringDraw(p->x, p->y, L"*");
		}
	}
}

/// 클라가 연결 종료 / 오류일 경우 player 상태 변경
void setPlayerDead(Player* player)
{
	player->isDead = true;
}

/// 특정 플레이어 삭제
list<Player*>::iterator removePlayer(list<Player*>::iterator& it)
{
	list<Player*>::iterator iter = playerList.erase(it);
	delete* it;
	return iter;
}

/// 플레이어 삭제
void deleteAllPlayer()
{
	for (list<Player*>::iterator iter = playerList.begin(); iter != playerList.end();)
	{
		iter = removePlayer(iter);
	}
}
