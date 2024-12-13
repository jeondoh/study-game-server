#include <iostream>
#include "player.h"
#include "console.h"

__int32 playerId = 0;
list<Player*> playerList;

/// ���� ���� �� ID �Ҵ�
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

/// ���� Ŭ���̾�Ʈ �� ������
void showConnectCount()
{
	wcout << L"" << endl;
	wcout << L"" << endl;
	wcout << L"" << endl;
	wcout << L"Connect Client: " << playerList.size() << endl;
}

/// �÷��̾� ������
void renderPlayer()
{
	// ���� Ŭ���̾�Ʈ �� ������
	showConnectCount();
	// ��� �÷��̾� �̵�
	for (Player* p : playerList)
	{
		if (!(p->isDead))
		{
			spriteStringDraw(p->x, p->y, L"*");
		}
	}
}

/// Ŭ�� ���� ���� / ������ ��� player ���� ����
void setPlayerDead(Player* player)
{
	player->isDead = true;
}

/// Ư�� �÷��̾� ����
list<Player*>::iterator removePlayer(list<Player*>::iterator& it)
{
	list<Player*>::iterator iter = playerList.erase(it);
	delete* it;
	return iter;
}

/// �÷��̾� ����
void deleteAllPlayer()
{
	for (list<Player*>::iterator iter = playerList.begin(); iter != playerList.end();)
	{
		iter = removePlayer(iter);
	}
}
