#include <time.h>
#include <windows.h>

#include "console.h"
#include "game.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include "stage.h"

#pragma comment(lib, "winmm.lib")

// ���� ���� ����
bool exitGame = false;
// ���� �������� ��ȣ
int currentStage = 1;
// ���� ����
GameStatus gameStatus = GameStatus::title;
// Ŀ��(>) ��ġ
// �޴� �����Ҷ� ���
Cursor cursor;

//////////////////////////////////////////////////////////////
// ���� �о����
// 
//////////////////////////////////////////////////////////////
void loadGame()
{
	srand(static_cast<unsigned int>(time(nullptr)));
	timeBeginPeriod(1);
	// �ܼ� ����
	csInitial();
	// �� ���� �ε�
	loadEnemyInfo();
	loadEnemyPatternInfo();
	// Ŀ�� ��ġ �ʱ�ȭ
	updateCursor(8, 8);
}

//////////////////////////////////////////////////////////////
// Ű���� �Էº�
// 
//////////////////////////////////////////////////////////////
void keyPress()
{
	switch (gameStatus)
	{
	case title:
		controlCursor();
		break;
	case loading:
		break;
	case game:
		controlPlayer();
		break;
	case gameclear:
		controlCursor();
		break;
	case gameover:
		controlCursor();
		break;
	}
}

//////////////////////////////////////////////////////////////
// ���� ������
// 
//////////////////////////////////////////////////////////////
void updateGame()
{
	switch (gameStatus)
	{
	case title:
		break;
	case loading:
		deleteBullet();
		// �������� �ε�
		loadStageInfo(currentStage);
		// �ε� �Ϸ�� ���� ����
		gameStatus = GameStatus::game;
		break;
	case game:
		// �÷��̾� ����� gameover ����
		if (isPlayerDead())
		{
			freeEnemy();
			updateCursor(8, 8);
			gameStatus = GameStatus::gameover;
			break;
		}
		// ���� ��ü ����� loading ����
		if (isEnemyAllDead())
		{
			// ��ü �������� Ŭ�����
			if (currentStage == maxStageNum)
			{
				updateCursor(8, 8);
				gameStatus = GameStatus::gameclear;
			}
			else
			{
				++currentStage;
				freeEnemy();
				gameStatus = GameStatus::loading;
			}
			break;
		}
		updatePlayer();
		updateEnemy();
		updateBullet();
		break;
	case gameclear:
		currentStage = 1;
		break;
	case gameover:
		break;
	}
}

//////////////////////////////////////////////////////////////
// ������
// 
//////////////////////////////////////////////////////////////
void renderGame()
{
	// ��ũ�� ���۸� ����
	bufferClear();

	switch (gameStatus)
	{
	case title:
		renderCursor();
		renderTitle();
		break;
	case loading:
		renderLoading();
		break;
	case game:
		renderEnemy();
		renderPlayer();
		renderBullet();
		break;
	case gameclear:
		renderCursor();
		renderGameClear();
		break;
	case gameover:
		renderCursor();
		renderGameOver();
		break;
	}
	// ��ũ�� ���۸� ȭ������ ���
	bufferFlip();
}

//////////////////////////////////////////////////////////////
// Ŀ�� ��Ʈ��
// �޴� ���ÿ뵵
// 
//////////////////////////////////////////////////////////////
void controlCursor()
{
	if (GetAsyncKeyState(VK_UP))
	{
		updateCursor(8, 8);
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		updateCursor(8, 10);
	}
	else if (GetAsyncKeyState(VK_RETURN))
	{
		// new / load game ����
		if (cursor.iY == 8)
		{
			gameStatus = GameStatus::loading;
		}
		// exit ����
		else if (cursor.iY == 10)
		{
			exitGame = true;
		}
	}
}

//////////////////////////////////////////////////////////////
// Ŀ�� ��ġ����
// �޴� ���ÿ뵵
// 
//////////////////////////////////////////////////////////////
void updateCursor(int iX, int iY)
{
	cursor.iX = iX;
	cursor.iY = iY;
}

//////////////////////////////////////////////////////////////
// Ŀ�� �׸���
// �޴� ���ÿ뵵
// 
//////////////////////////////////////////////////////////////
void renderCursor()
{
	spriteDraw(cursor.iX, cursor.iY, '>');
}

//////////////////////////////////////////////////////////////
// ���� Ÿ��Ʋ ȭ��
// 
//////////////////////////////////////////////////////////////
void renderTitle()
{
	spriteStringDraw(8, 2, "================================");
	spriteStringDraw(10, 5, "Text Shooting Game");
	spriteStringDraw(10, 8, "Start Game - Stage 1");
	spriteStringDraw(10, 10, "Exit Game");
	spriteStringDraw(8, 13, "================================");
}

//////////////////////////////////////////////////////////////
// ���� �ε� ȭ��
// 
//////////////////////////////////////////////////////////////
void renderLoading()
{
	spriteStringDraw(dfSCREEN_WIDTH / 2, 8, "Game Loading .............");
}

//////////////////////////////////////////////////////////////
// ���� ���� ȭ��
// 
//////////////////////////////////////////////////////////////
void renderGameClear()
{
	spriteStringDraw(8, 2, "================================");
	spriteStringDraw(10, 5, "!!! GAME CLEAR !!!");
	spriteStringDraw(10, 8, "New Game - Stage 1");
	spriteStringDraw(10, 10, "Exit Game");
	spriteStringDraw(8, 15, "================================");
}

//////////////////////////////////////////////////////////////
// ���� ���� ȭ��
// 
//////////////////////////////////////////////////////////////
void renderGameOver()
{
	spriteStringDraw(8, 2, "================================");
	spriteStringDraw(10, 5, "GAME OVER");
	spriteStringDraw(10, 8, "Load Game - One more Coin");
	spriteStringDraw(10, 10, "Exit Game");
	spriteStringDraw(8, 15, "================================");
}