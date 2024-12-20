#pragma once

// 플레이어 정보
struct Player
{
	// x, y 좌표
	int iX;
	int iY;
	// hp
	int hp;
	// 공격 속도
	// n 프레임 마다 총알 발사
	int attackSpeed;
	// 총알 속도
	int bulletSpeed;
};

//-------------------------------------------------------------
// player 정보 초기화
// 값은 하드코딩으로 지정
//
//-------------------------------------------------------------
void createPlayer();

//-------------------------------------------------------------
// player 조작
//
//-------------------------------------------------------------
void controlPlayer();

//-------------------------------------------------------------
// player 상태변경
//
//-------------------------------------------------------------
void updatePlayer();

//-------------------------------------------------------------
// player 사망체크
//
//-------------------------------------------------------------
bool isPlayerDead();

//-------------------------------------------------------------
// player 랜더링을 위해 buffer로 전달
//
//-------------------------------------------------------------
void renderPlayer();