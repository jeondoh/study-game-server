#include <Windows.h>

#include "player.h"
#include "bullet.h"
#include "console.h"

Player player;

//-------------------------------------------------------------
// player ���� �ʱ�ȭ
// ���� �ϵ��ڵ����� ����
//
//-------------------------------------------------------------
void createPlayer()
{
	player.iX = dfSCREEN_WIDTH / 2;
	player.iY = dfSCREEN_HEIGHT - 2;
	player.hp = 8;
	player.attackSpeed = 6;
	player.bulletSpeed = 4;
}

//-------------------------------------------------------------
// player ����
//
//-------------------------------------------------------------
void controlPlayer()
{
	// ------- player �̵� -------
	// ����
	if (player.iX > 0 && GetAsyncKeyState(VK_LEFT))
	{
		--player.iX;
	}
	// ������
	else if (player.iX < dfSCREEN_WIDTH - 2 && GetAsyncKeyState(VK_RIGHT))
	{
		++player.iX;
	}
	// ------- player ���� -------
	if (GetAsyncKeyState(VK_SPACE))
	{
		// ���ݼӵ� ����
		if (frameCount % player.attackSpeed == 0)
		{
			createBullet(player.iX, player.iY - 1, player.bulletSpeed, BulletType::PLAYER);
		}
	}
}

//-------------------------------------------------------------
// player ���º���
//
//-------------------------------------------------------------
void updatePlayer()
{
	// �ǰ�����
	int iCnt;
	for (iCnt = 0; iCnt < BULLET_COUNT; iCnt++)
	{
		// bullet ������(�̵��ӵ�) ������ �� �ڸ��� ��� ���� ��� �������� ���������� ���°��� ����
		if (!bullet[iCnt].isVisible)
		{
			continue;
		}
		if (bullet[iCnt].bulletType == PLAYER)
		{
			continue;
		}
		if (player.iX == bullet[iCnt].iX && player.iY == bullet[iCnt].iY)
		{
			player.hp -= bullet[iCnt].damage;
			deleteBullet(iCnt);
			break;
		}
	}
}

//-------------------------------------------------------------
// player ���üũ
//
//-------------------------------------------------------------
bool isPlayerDead()
{
	if (player.hp <= 0)
	{
		return true;
	}

	return false;
}

//-------------------------------------------------------------
// player �������� ���� buffer�� ����
//
//-------------------------------------------------------------
void renderPlayer()
{
	// hp ����
	spriteDraw(dfSCREEN_WIDTH - 7, 0, 'H');
	spriteDraw(dfSCREEN_WIDTH - 6, 0, 'P');
	spriteDraw(dfSCREEN_WIDTH - 4, 0, ':');
	spriteDraw(dfSCREEN_WIDTH - 2, 0, player.hp + '0');
	// player �׸���
	spriteDraw(player.iX, player.iY, 'P');
}