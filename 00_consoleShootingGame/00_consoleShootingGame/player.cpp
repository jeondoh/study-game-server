#include <Windows.h>

#include "player.h"
#include "bullet.h"
#include "console.h"

Player player;

//-------------------------------------------------------------
// player 정보 초기화
// 값은 하드코딩으로 지정
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
// player 조작
//
//-------------------------------------------------------------
void controlPlayer()
{
	// ------- player 이동 -------
	// 왼쪽
	if (player.iX > 0 && GetAsyncKeyState(VK_LEFT))
	{
		--player.iX;
	}
	// 오른쪽
	else if (player.iX < dfSCREEN_WIDTH - 2 && GetAsyncKeyState(VK_RIGHT))
	{
		++player.iX;
	}
	// ------- player 공격 -------
	if (GetAsyncKeyState(VK_SPACE))
	{
		// 공격속도 설정
		if (frameCount % player.attackSpeed == 0)
		{
			createBullet(player.iX, player.iY - 1, player.bulletSpeed, BulletType::PLAYER);
		}
	}
}

//-------------------------------------------------------------
// player 상태변경
//
//-------------------------------------------------------------
void updatePlayer()
{
	// 피격판정
	int iCnt;
	for (iCnt = 0; iCnt < BULLET_COUNT; iCnt++)
	{
		// bullet 프레임(이동속도) 때문에 그 자리에 계속 있을 경우 데미지가 지속적으로 들어가는것을 방지
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
// player 사망체크
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
// player 랜더링을 위해 buffer로 전달
//
//-------------------------------------------------------------
void renderPlayer()
{
	// hp 정보
	spriteDraw(dfSCREEN_WIDTH - 7, 0, 'H');
	spriteDraw(dfSCREEN_WIDTH - 6, 0, 'P');
	spriteDraw(dfSCREEN_WIDTH - 4, 0, ':');
	spriteDraw(dfSCREEN_WIDTH - 2, 0, player.hp + '0');
	// player 그리기
	spriteDraw(player.iX, player.iY, 'P');
}