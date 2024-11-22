#include <stdlib.h>

#include "bullet.h"
#include "console.h"

Bullet bullet[BULLET_COUNT];

//-------------------------------------------------------------
// bullet ����
//
//-------------------------------------------------------------
void createBullet(const int iX, const int iY, const int speed, const BulletType bulletType)
{
	int iCnt;
	for (iCnt = 0; iCnt < BULLET_COUNT; ++iCnt)
	{
		if (!bullet[iCnt].isVisible)
		{
			bullet[iCnt].iX = iX;
			bullet[iCnt].iY = iY;
			bullet[iCnt].damage = 1;
			bullet[iCnt].bulletSpeed = speed;
			bullet[iCnt].bulletType = bulletType;
			bullet[iCnt].isVisible = true;
			// Ÿ�Ժ� �Ѿ� ���� ����
			switch (bulletType)
			{
			case PLAYER:
				bullet[iCnt].direction = BulletDirection::CENTER;
				break;
			case ENEMY:
				// ��������
				bullet[iCnt].direction = (BulletDirection)(rand() % 3);
				break;
			}
			break;
		}
	}
}

//-------------------------------------------------------------
// bullet ���� ����
//
//-------------------------------------------------------------
void updateBullet()
{
	int iCnt;
	for (iCnt = 0; iCnt < BULLET_COUNT; ++iCnt)
	{
		if (!bullet[iCnt].isVisible)
		{
			continue;
		}
		// bullet �ӵ� ����
		if (frameCount % bullet[iCnt].bulletSpeed != 0)
		{
			continue;
		}
		// bullet ��ǥ ����
		switch (bullet[iCnt].direction)
		{
			// ���� �밢
		case LEFT:
			bullet[iCnt].iX -= 1;
			bullet[iCnt].iY += 1;
			break;
		case RIGHT:
			// ���� �밢
			bullet[iCnt].iX += 1;
			bullet[iCnt].iY += 1;
			break;
			// �߾�
		case CENTER:
			if (bullet[iCnt].bulletType == PLAYER)
			{
				bullet[iCnt].iY -= 1;
			}
			else if (bullet[iCnt].bulletType == ENEMY)
			{
				bullet[iCnt].iY += 1;
			}
			break;
		}
		// ȭ�� ���� ���ϰ�� visible ����
		if (bullet[iCnt].iX < 0 || bullet[iCnt].iY < 0
			|| bullet[iCnt].iX >= dfSCREEN_WIDTH - 1 || bullet[iCnt].iY >= dfSCREEN_HEIGHT)
		{
			deleteBullet(iCnt);
		}
	}
}

//-------------------------------------------------------------
// bullet ��Ȱ��ȭ(isVisible = false)
//
//-------------------------------------------------------------
void deleteBullet()
{
	int iCnt;
	for (iCnt = 0; iCnt < BULLET_COUNT; ++iCnt)
	{
		bullet[iCnt].isVisible = false;
	}
}

void deleteBullet(int index)
{
	bullet[index].isVisible = false;
}

//-------------------------------------------------------------
// bullet �������� ���� buffer�� ����
//
//-------------------------------------------------------------
void renderBullet()
{
	int iCnt;
	for (iCnt = 0; iCnt < BULLET_COUNT; ++iCnt)
	{
		if (bullet[iCnt].isVisible)
		{
			switch (bullet[iCnt].bulletType)
			{
			case PLAYER:
				spriteDraw(bullet[iCnt].iX, bullet[iCnt].iY, '^');
				break;
			case ENEMY:
				spriteDraw(bullet[iCnt].iX, bullet[iCnt].iY, '*');
				break;
			}
		}
	}
}