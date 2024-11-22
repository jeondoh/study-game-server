#include <stdlib.h>

#include "bullet.h"
#include "console.h"

Bullet bullet[BULLET_COUNT];

//-------------------------------------------------------------
// bullet 생성
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
			// 타입별 총알 방향 설정
			switch (bulletType)
			{
			case PLAYER:
				bullet[iCnt].direction = BulletDirection::CENTER;
				break;
			case ENEMY:
				// 랜덤으로
				bullet[iCnt].direction = (BulletDirection)(rand() % 3);
				break;
			}
			break;
		}
	}
}

//-------------------------------------------------------------
// bullet 상태 변경
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
		// bullet 속도 설정
		if (frameCount % bullet[iCnt].bulletSpeed != 0)
		{
			continue;
		}
		// bullet 좌표 설정
		switch (bullet[iCnt].direction)
		{
			// 좌측 대각
		case LEFT:
			bullet[iCnt].iX -= 1;
			bullet[iCnt].iY += 1;
			break;
		case RIGHT:
			// 우측 대각
			bullet[iCnt].iX += 1;
			bullet[iCnt].iY += 1;
			break;
			// 중앙
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
		// 화면 범위 밖일경우 visible 변경
		if (bullet[iCnt].iX < 0 || bullet[iCnt].iY < 0
			|| bullet[iCnt].iX >= dfSCREEN_WIDTH - 1 || bullet[iCnt].iY >= dfSCREEN_HEIGHT)
		{
			deleteBullet(iCnt);
		}
	}
}

//-------------------------------------------------------------
// bullet 비활성화(isVisible = false)
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
// bullet 랜더링을 위해 buffer로 전달
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