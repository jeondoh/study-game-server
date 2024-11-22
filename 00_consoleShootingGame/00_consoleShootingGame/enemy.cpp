#include <iostream>

#include "enemy.h"
#include "bullet.h"
#include "console.h"

// ��
Enemy* enemy;
// �� ����
EnemyInfo* enemyInfo;
// �� ����(ũ��)
int enemySize;

//////////////////////////////////////////////////////////////
// load movePatternInfo.txt
// 
//////////////////////////////////////////////////////////////
void loadEnemyPatternInfo()
{
	FILE* enemyMoveFile;
	errno_t err;
	// movePatternInfo ���� ����
	err = fopen_s(&enemyMoveFile, "movePatternInfo.txt", "r");
	// ���� �ε� ����
	if (err != 0)
	{
		return;
	}
	// move ���� ����
	int fileCnt = 0;
	// move ���� ���� �б�
	fscanf_s(enemyMoveFile, "%d ", &fileCnt);

	// ���ϸ�(move.txt)�� �´� ���� �о����
	int iCnt;
	char fileName[15];
	for (iCnt = 0; iCnt < fileCnt; ++iCnt)
	{
		fscanf_s(enemyMoveFile, "%s", fileName, sizeof(fileName));
		// enemyPattern �ʱ�ȭ�� ���� ������ ����
		FILE* moveFile;
		errno_t err;
		// move ���� ����
		err = fopen_s(&moveFile, fileName, "r");
		// ���� �ε� ����
		if (err != 0)
		{
			return;
		}
		// ���� ����
		int moveCnt = 0;
		// ���� ���� �б�
		fscanf_s(moveFile, "%d ", &moveCnt);

		int jCnt;
		int iX;
		int iY;
		for (jCnt = 0; jCnt < moveCnt; ++jCnt)
		{
			fscanf_s(moveFile, "%d %d", &iX, &iY);
			// �̵���� ��ǥ ����
			enemyInfo[iCnt].movePattern[jCnt].iX = iX;
			enemyInfo[iCnt].movePattern[jCnt].iY = iY;
			// ��ü ������ ����
			enemyInfo[iCnt].patternLength = moveCnt;
		}
		// ���� �ݱ�
		fclose(moveFile);
	}
	// ���� �ݱ�
	fclose(enemyMoveFile);
}

//////////////////////////////////////////////////////////////
// load enemyInfo.txt
// enemyInfo �ʱ�ȭ
// 
//////////////////////////////////////////////////////////////
void loadEnemyInfo()
{
	FILE* enemyInfoFile;
	errno_t err;
	// enemyInfo ���� ����
	err = fopen_s(&enemyInfoFile, "enemyInfo.txt", "r");
	// ���� �ε� ����
	if (err != 0)
	{
		return;
	}
	// enemy ���� ���� ����
	int fileCnt = 0;
	// enemy ���� ���� �б�
	fscanf_s(enemyInfoFile, "%d ", &fileCnt);
	// enemyInfo �����Ҵ�
	enemyInfo = (EnemyInfo*)malloc(fileCnt * sizeof(EnemyInfo));
	// ���ϸ�(enemy.txt)�� �´� ���� �о����
	int iCnt;
	char fileName[15];
	for (iCnt = 0; iCnt < fileCnt; ++iCnt)
	{
		fscanf_s(enemyInfoFile, "%s", fileName, sizeof(fileName));

		// enemyType �ʱ�ȭ�� ���� ������ ����
		FILE* enemyTypeFile;
		errno_t err;
		// enemy ���� ����
		err = fopen_s(&enemyTypeFile, fileName, "r");
		// ���� �ε� ����
		if (err != 0)
		{
			return;
		}
		/*
		enemyType �б�(enemy.txt)
		-- ���� ù��°��
			ù��° = enemyAlphabat
			�ι�° = hp
		-- ���� �ι�°��
			ù��° = moveSpeed
			�ι�° = attackSpeed
			����° = bulletSpeed
		*/
		char enemyAlphabat;
		int hp;
		int moveSpeed;
		int attackSpeed;
		int bulletSpeed;

		fscanf_s(enemyTypeFile, "%c %d ", &enemyAlphabat, sizeof(enemyAlphabat), &hp);
		fscanf_s(enemyTypeFile, "%d %d %d", &moveSpeed, &attackSpeed, &bulletSpeed);
		// enemyInfo ����
		enemyInfo[iCnt].enemyAlphabat = enemyAlphabat;
		enemyInfo[iCnt].hp = hp;
		enemyInfo[iCnt].moveSpeed = moveSpeed;
		enemyInfo[iCnt].attackSpeed = attackSpeed;
		enemyInfo[iCnt].bulletSpeed = bulletSpeed;
		// ���� �ݱ�
		fclose(enemyTypeFile);
	}
	// ���� �ݱ�
	fclose(enemyInfoFile);
}

//////////////////////////////////////////////////////////////
// load stage.txt
// loadStageInfo ���� ȣ���
// 
//////////////////////////////////////////////////////////////
void createEnemy(const char* fileName)
{
	FILE* stageFile;
	errno_t err;
	// �������� ���� ���� ����
	err = fopen_s(&stageFile, fileName, "r");
	// ���� �ε� ����
	if (err != 0)
	{
		return;
	}
	// enemy ����
	int enemyCnt = 0;
	// enemy ���� �б�
	fscanf_s(stageFile, "%d", &enemyCnt);
	// enemy �����Ҵ�
	enemySize = enemyCnt;
	enemy = (Enemy*)malloc(enemyCnt * sizeof(Enemy));
	// enemy ���� ����
	int iY = 0;
	int eCnt = 0;
	// �ٴ����� �б�
	char enemyLine[dfSCREEN_WIDTH];
	// enemy ��ġ����, Ÿ�� ��������
	while (fgets(enemyLine, sizeof(enemyLine), stageFile))
	{
		int iX = 0;
		int iCnt;

		for (iCnt = 0; enemyLine[iCnt] != '\0'; ++iCnt)
		{
			// �����̽�(x��ǥ)
			if (enemyLine[iCnt] == 0x20)
			{
				++iX;
			}
			// ����(y��ǥ)
			else if (enemyLine[iCnt] == 0x0A)
			{
				++iY;
			}
			// enemy ���� ����
			else
			{
				// ���� �����÷� ��� ����
				if (eCnt > enemyCnt)
				{
					break;
				}
				// �� ��ǥ ����
				enemy[eCnt].iX = iX;
				enemy[eCnt].iY = iY;
				// �� ���� �ε��� ����
				enemy[eCnt].currentPoint = 0;
				// �� ������ ���ڷ� ����
				int index = enemyLine[iCnt] - '0';
				// �� ���� ����
				enemy[eCnt].enemyInfo = &enemyInfo[index];
				// �� ���� hp
				enemy[eCnt].currentHp = enemy[eCnt].enemyInfo->hp;
				++eCnt;
			}
		}
	}
	// ���� �ݱ�
	fclose(stageFile);
}

//////////////////////////////////////////////////////////////
// enemy ���º���
// 
//////////////////////////////////////////////////////////////
void updateEnemy()
{
	int iCnt;
	for (iCnt = 0; iCnt < enemySize; ++iCnt)
	{
		// hp�� 0(���)�� ��� ������Ʈ X
		if (enemy[iCnt].currentHp == 0)
		{
			continue;
		}
		// �ǰ�����
		int jCnt;
		for (jCnt = 0; jCnt < BULLET_COUNT; jCnt++)
		{
			// bullet ������(�̵��ӵ�) ������ �� �ڸ��� ��� ���� ��� �������� ���������� ���°��� ����
			if (!bullet[jCnt].isVisible)
			{
				continue;
			}
			if (bullet[jCnt].bulletType == ENEMY)
			{
				continue;
			}
			if (enemy[iCnt].iX == bullet[jCnt].iX && enemy[iCnt].iY == bullet[jCnt].iY)
			{
				enemy[iCnt].currentHp -= bullet[jCnt].damage;
				deleteBullet(jCnt);
				break;
			}
		}
		// �����Ӱ��� ���� �̵�
		if (frameCount % enemy[iCnt].enemyInfo->moveSpeed == 0)
		{
			// �̵���� ���� �ε���
			int patternIndex = enemy[iCnt].currentPoint;
			// �̵� ��ǥ
			int iX = enemy[iCnt].enemyInfo->movePattern[patternIndex].iX;
			int iY = enemy[iCnt].enemyInfo->movePattern[patternIndex].iY;
			// ���� ��ǥ ������Ʈ
			enemy[iCnt].iX += iX;
			enemy[iCnt].iY += iY;
			// ���� �̵� ���� ������Ʈ
			enemy[iCnt].currentPoint = ++patternIndex % enemy[iCnt].enemyInfo->patternLength;
			// ���ݼӵ� ����
			if (frameCount % enemy[iCnt].enemyInfo->attackSpeed == 0)
			{
				createBullet(enemy[iCnt].iX, enemy[iCnt].iY + 1, enemy[iCnt].enemyInfo->bulletSpeed, BulletType::ENEMY);
			}
		}
	}
}

//-------------------------------------------------------------
// enemy ���üũ
//
//-------------------------------------------------------------
bool isEnemyAllDead()
{
	// ����� �� ����
	int deadEnemyCnt = 0;
	int iCnt;
	for (iCnt = 0; iCnt < enemySize; ++iCnt)
	{
		if (enemy[iCnt].currentHp <= 0)
		{
			++deadEnemyCnt;
		}
	}
	if (enemySize == deadEnemyCnt)
	{
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////
// enemy �������� ���� buffer�� ����
// 
//////////////////////////////////////////////////////////////
void renderEnemy()
{
	int iCnt;
	for (iCnt = 0; iCnt < enemySize; ++iCnt)
	{
		if (enemy[iCnt].currentHp != 0)
		{
			spriteDraw(enemy[iCnt].iX, enemy[iCnt].iY, enemy[iCnt].enemyInfo->enemyAlphabat);
		}
	}
}

//////////////////////////////////////////////////////////////
// enemy �����Ҵ� ����
// 
//////////////////////////////////////////////////////////////
void freeEnemy()
{
	free(enemy);
}
