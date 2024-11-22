#pragma once

#define BULLET_COUNT			50	// 총알 개수

// 총알 타입
enum BulletType
{
	PLAYER,
	ENEMY,
};

// 총알 방향
enum BulletDirection
{
	CENTER, // 일자
	LEFT,	// 좌측 대각
	RIGHT,	// 우측 대각
};

struct Bullet
{
	// 총알 x, y좌표
	int iX;
	int iY;
	// 총알 데미지
	// 현재는 1로 고정
	int damage;
	// 총알 속도
	int bulletSpeed;
	// 총알 visible 여부
	bool isVisible;
	// 총알 방향
	BulletDirection direction;
	// 총알 타입
	BulletType bulletType;
};

extern Bullet bullet[BULLET_COUNT];

//-------------------------------------------------------------
// bullet 생성
//
//-------------------------------------------------------------
void createBullet(const int iX, const int iY, const int speed, const BulletType bulletType);

//-------------------------------------------------------------
// bullet 상태 변경
//
//-------------------------------------------------------------
void updateBullet();

//-------------------------------------------------------------
// bullet 삭제(isVisible = false)
//
//-------------------------------------------------------------
void deleteBullet();
void deleteBullet(int index);

//-------------------------------------------------------------
// bullet 랜더링을 위해 buffer로 전달
//
//-------------------------------------------------------------
void renderBullet();