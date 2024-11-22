#include <iostream>

#include "enemy.h"
#include "bullet.h"
#include "console.h"

// 적
Enemy* enemy;
// 적 정보
EnemyInfo* enemyInfo;
// 적 개수(크기)
int enemySize;

//////////////////////////////////////////////////////////////
// load movePatternInfo.txt
// 
//////////////////////////////////////////////////////////////
void loadEnemyPatternInfo()
{
	FILE* enemyMoveFile;
	errno_t err;
	// movePatternInfo 파일 열기
	err = fopen_s(&enemyMoveFile, "movePatternInfo.txt", "r");
	// 파일 로드 실패
	if (err != 0)
	{
		return;
	}
	// move 파일 개수
	int fileCnt = 0;
	// move 파일 개수 읽기
	fscanf_s(enemyMoveFile, "%d ", &fileCnt);

	// 파일명(move.txt)에 맞는 정보 읽어오기
	int iCnt;
	char fileName[15];
	for (iCnt = 0; iCnt < fileCnt; ++iCnt)
	{
		fscanf_s(enemyMoveFile, "%s", fileName, sizeof(fileName));
		// enemyPattern 초기화를 위해 파일을 읽음
		FILE* moveFile;
		errno_t err;
		// move 파일 열기
		err = fopen_s(&moveFile, fileName, "r");
		// 파일 로드 실패
		if (err != 0)
		{
			return;
		}
		// 패턴 개수
		int moveCnt = 0;
		// 패턴 개수 읽기
		fscanf_s(moveFile, "%d ", &moveCnt);

		int jCnt;
		int iX;
		int iY;
		for (jCnt = 0; jCnt < moveCnt; ++jCnt)
		{
			fscanf_s(moveFile, "%d %d", &iX, &iY);
			// 이동경로 좌표 설정
			enemyInfo[iCnt].movePattern[jCnt].iX = iX;
			enemyInfo[iCnt].movePattern[jCnt].iY = iY;
			// 전체 패턴의 길이
			enemyInfo[iCnt].patternLength = moveCnt;
		}
		// 파일 닫기
		fclose(moveFile);
	}
	// 파일 닫기
	fclose(enemyMoveFile);
}

//////////////////////////////////////////////////////////////
// load enemyInfo.txt
// enemyInfo 초기화
// 
//////////////////////////////////////////////////////////////
void loadEnemyInfo()
{
	FILE* enemyInfoFile;
	errno_t err;
	// enemyInfo 파일 열기
	err = fopen_s(&enemyInfoFile, "enemyInfo.txt", "r");
	// 파일 로드 실패
	if (err != 0)
	{
		return;
	}
	// enemy 정보 파일 개수
	int fileCnt = 0;
	// enemy 파일 개수 읽기
	fscanf_s(enemyInfoFile, "%d ", &fileCnt);
	// enemyInfo 동적할당
	enemyInfo = (EnemyInfo*)malloc(fileCnt * sizeof(EnemyInfo));
	// 파일명(enemy.txt)에 맞는 정보 읽어오기
	int iCnt;
	char fileName[15];
	for (iCnt = 0; iCnt < fileCnt; ++iCnt)
	{
		fscanf_s(enemyInfoFile, "%s", fileName, sizeof(fileName));

		// enemyType 초기화를 위해 파일을 읽음
		FILE* enemyTypeFile;
		errno_t err;
		// enemy 파일 열기
		err = fopen_s(&enemyTypeFile, fileName, "r");
		// 파일 로드 실패
		if (err != 0)
		{
			return;
		}
		/*
		enemyType 읽기(enemy.txt)
		-- 파일 첫번째줄
			첫번째 = enemyAlphabat
			두번째 = hp
		-- 파일 두번째줄
			첫번째 = moveSpeed
			두번째 = attackSpeed
			세번째 = bulletSpeed
		*/
		char enemyAlphabat;
		int hp;
		int moveSpeed;
		int attackSpeed;
		int bulletSpeed;

		fscanf_s(enemyTypeFile, "%c %d ", &enemyAlphabat, sizeof(enemyAlphabat), &hp);
		fscanf_s(enemyTypeFile, "%d %d %d", &moveSpeed, &attackSpeed, &bulletSpeed);
		// enemyInfo 설정
		enemyInfo[iCnt].enemyAlphabat = enemyAlphabat;
		enemyInfo[iCnt].hp = hp;
		enemyInfo[iCnt].moveSpeed = moveSpeed;
		enemyInfo[iCnt].attackSpeed = attackSpeed;
		enemyInfo[iCnt].bulletSpeed = bulletSpeed;
		// 파일 닫기
		fclose(enemyTypeFile);
	}
	// 파일 닫기
	fclose(enemyInfoFile);
}

//////////////////////////////////////////////////////////////
// load stage.txt
// loadStageInfo 에서 호출됨
// 
//////////////////////////////////////////////////////////////
void createEnemy(const char* fileName)
{
	FILE* stageFile;
	errno_t err;
	// 스테이지 정보 파일 열기
	err = fopen_s(&stageFile, fileName, "r");
	// 파일 로드 실패
	if (err != 0)
	{
		return;
	}
	// enemy 개수
	int enemyCnt = 0;
	// enemy 개수 읽기
	fscanf_s(stageFile, "%d", &enemyCnt);
	// enemy 동적할당
	enemySize = enemyCnt;
	enemy = (Enemy*)malloc(enemyCnt * sizeof(Enemy));
	// enemy 정보 삽입
	int iY = 0;
	int eCnt = 0;
	// 줄단위로 읽기
	char enemyLine[dfSCREEN_WIDTH];
	// enemy 배치정보, 타입 가져오기
	while (fgets(enemyLine, sizeof(enemyLine), stageFile))
	{
		int iX = 0;
		int iCnt;

		for (iCnt = 0; enemyLine[iCnt] != '\0'; ++iCnt)
		{
			// 스페이스(x좌표)
			if (enemyLine[iCnt] == 0x20)
			{
				++iX;
			}
			// 엔터(y좌표)
			else if (enemyLine[iCnt] == 0x0A)
			{
				++iY;
			}
			// enemy 정보 설정
			else
			{
				// 버퍼 오버플로 경고 방지
				if (eCnt > enemyCnt)
				{
					break;
				}
				// 적 좌표 설정
				enemy[eCnt].iX = iX;
				enemy[eCnt].iY = iY;
				// 적 패턴 인덱스 설정
				enemy[eCnt].currentPoint = 0;
				// 적 정보는 숫자로 구분
				int index = enemyLine[iCnt] - '0';
				// 적 정보 설정
				enemy[eCnt].enemyInfo = &enemyInfo[index];
				// 적 현재 hp
				enemy[eCnt].currentHp = enemy[eCnt].enemyInfo->hp;
				++eCnt;
			}
		}
	}
	// 파일 닫기
	fclose(stageFile);
}

//////////////////////////////////////////////////////////////
// enemy 상태변경
// 
//////////////////////////////////////////////////////////////
void updateEnemy()
{
	int iCnt;
	for (iCnt = 0; iCnt < enemySize; ++iCnt)
	{
		// hp가 0(사망)일 경우 업데이트 X
		if (enemy[iCnt].currentHp == 0)
		{
			continue;
		}
		// 피격판정
		int jCnt;
		for (jCnt = 0; jCnt < BULLET_COUNT; jCnt++)
		{
			// bullet 프레임(이동속도) 때문에 그 자리에 계속 있을 경우 데미지가 지속적으로 들어가는것을 방지
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
		// 움직임값에 따른 이동
		if (frameCount % enemy[iCnt].enemyInfo->moveSpeed == 0)
		{
			// 이동경로 패턴 인덱스
			int patternIndex = enemy[iCnt].currentPoint;
			// 이동 좌표
			int iX = enemy[iCnt].enemyInfo->movePattern[patternIndex].iX;
			int iY = enemy[iCnt].enemyInfo->movePattern[patternIndex].iY;
			// 현재 좌표 업데이트
			enemy[iCnt].iX += iX;
			enemy[iCnt].iY += iY;
			// 현재 이동 지점 업데이트
			enemy[iCnt].currentPoint = ++patternIndex % enemy[iCnt].enemyInfo->patternLength;
			// 공격속도 설정
			if (frameCount % enemy[iCnt].enemyInfo->attackSpeed == 0)
			{
				createBullet(enemy[iCnt].iX, enemy[iCnt].iY + 1, enemy[iCnt].enemyInfo->bulletSpeed, BulletType::ENEMY);
			}
		}
	}
}

//-------------------------------------------------------------
// enemy 사망체크
//
//-------------------------------------------------------------
bool isEnemyAllDead()
{
	// 사망한 적 개수
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
// enemy 랜더링을 위해 buffer로 전달
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
// enemy 동적할당 해제
// 
//////////////////////////////////////////////////////////////
void freeEnemy()
{
	free(enemy);
}
