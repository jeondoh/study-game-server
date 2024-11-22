#pragma once

#define BULLET_COUNT			50	// �Ѿ� ����

// �Ѿ� Ÿ��
enum BulletType
{
	PLAYER,
	ENEMY,
};

// �Ѿ� ����
enum BulletDirection
{
	CENTER, // ����
	LEFT,	// ���� �밢
	RIGHT,	// ���� �밢
};

struct Bullet
{
	// �Ѿ� x, y��ǥ
	int iX;
	int iY;
	// �Ѿ� ������
	// ����� 1�� ����
	int damage;
	// �Ѿ� �ӵ�
	int bulletSpeed;
	// �Ѿ� visible ����
	bool isVisible;
	// �Ѿ� ����
	BulletDirection direction;
	// �Ѿ� Ÿ��
	BulletType bulletType;
};

extern Bullet bullet[BULLET_COUNT];

//-------------------------------------------------------------
// bullet ����
//
//-------------------------------------------------------------
void createBullet(const int iX, const int iY, const int speed, const BulletType bulletType);

//-------------------------------------------------------------
// bullet ���� ����
//
//-------------------------------------------------------------
void updateBullet();

//-------------------------------------------------------------
// bullet ����(isVisible = false)
//
//-------------------------------------------------------------
void deleteBullet();
void deleteBullet(int index);

//-------------------------------------------------------------
// bullet �������� ���� buffer�� ����
//
//-------------------------------------------------------------
void renderBullet();