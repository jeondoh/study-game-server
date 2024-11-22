#include <time.h>
#include <windows.h>

#include "console.h"
#include "game.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include "stage.h"

#pragma comment(lib, "winmm.lib")

// 게임 종료 여부
bool exitGame = false;
// 현재 스테이지 번호
int currentStage = 1;
// 게임 상태
GameStatus gameStatus = GameStatus::title;
// 커서(>) 위치
// 메뉴 선택할때 사용
Cursor cursor;

//////////////////////////////////////////////////////////////
// 파일 읽어오기
// 
//////////////////////////////////////////////////////////////
void loadGame()
{
	srand(static_cast<unsigned int>(time(nullptr)));
	timeBeginPeriod(1);
	// 콘솔 제어
	csInitial();
	// 적 정보 로드
	loadEnemyInfo();
	loadEnemyPatternInfo();
	// 커서 위치 초기화
	updateCursor(8, 8);
}

//////////////////////////////////////////////////////////////
// 키보드 입력부
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
// 게임 로직부
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
		// 스테이지 로드
		loadStageInfo(currentStage);
		// 로딩 완료시 상태 변경
		gameStatus = GameStatus::game;
		break;
	case game:
		// 플레이어 사망시 gameover 상태
		if (isPlayerDead())
		{
			freeEnemy();
			updateCursor(8, 8);
			gameStatus = GameStatus::gameover;
			break;
		}
		// 적군 전체 사망시 loading 상태
		if (isEnemyAllDead())
		{
			// 전체 스테이지 클리어시
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
// 랜더부
// 
//////////////////////////////////////////////////////////////
void renderGame()
{
	// 스크린 버퍼를 지움
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
	// 스크린 버퍼를 화면으로 출력
	bufferFlip();
}

//////////////////////////////////////////////////////////////
// 커서 컨트롤
// 메뉴 선택용도
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
		// new / load game 선택
		if (cursor.iY == 8)
		{
			gameStatus = GameStatus::loading;
		}
		// exit 선택
		else if (cursor.iY == 10)
		{
			exitGame = true;
		}
	}
}

//////////////////////////////////////////////////////////////
// 커서 위치변경
// 메뉴 선택용도
// 
//////////////////////////////////////////////////////////////
void updateCursor(int iX, int iY)
{
	cursor.iX = iX;
	cursor.iY = iY;
}

//////////////////////////////////////////////////////////////
// 커서 그리기
// 메뉴 선택용도
// 
//////////////////////////////////////////////////////////////
void renderCursor()
{
	spriteDraw(cursor.iX, cursor.iY, '>');
}

//////////////////////////////////////////////////////////////
// 게임 타이틀 화면
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
// 게임 로딩 화면
// 
//////////////////////////////////////////////////////////////
void renderLoading()
{
	spriteStringDraw(dfSCREEN_WIDTH / 2, 8, "Game Loading .............");
}

//////////////////////////////////////////////////////////////
// 게임 종료 화면
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
// 게임 종료 화면
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