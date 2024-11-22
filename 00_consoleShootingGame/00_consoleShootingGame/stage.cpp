#include <iostream>

#include "stage.h"
#include "enemy.h"
#include "player.h"

// 전체 스테이지 개수
int maxStageNum = 0;

//////////////////////////////////////////////////////////////
// 스테이지 파일명이 담긴 stageInfo.txt를 읽어오는 함수
// createEnemy(fileName) 호출
// createPlayer 호출
// 
//////////////////////////////////////////////////////////////
void loadStageInfo(const int stageNum)
{
	// 파일 읽기
	FILE* stageInfoFile;
	errno_t err;
	// 스테이지 파일 열기
	err = fopen_s(&stageInfoFile, "stageInfo.txt", "r");
	// 파일 로드 실패
	if (err != 0)
	{
		return;
	}
	// 스테이지 파일 개수
	int fileCnt = 0;
	// 스테이지 전체 개수 읽기
	fscanf_s(stageInfoFile, "%d ", &fileCnt);
	// 전체 스테이지 수 업데이트
	maxStageNum = fileCnt;
	// 현재 스테이지 번호와 맞는 파일명을 읽어온다.
	int iCnt;
	char fileName[15];
	for (iCnt = 1; iCnt <= fileCnt; ++iCnt)
	{
		fscanf_s(stageInfoFile, "%s", fileName, sizeof(fileName));
		if (iCnt == stageNum)
		{
			break;
		}
	}
	// 적 생성
	createEnemy(fileName);
	// 플레이어 생성
	createPlayer();
	// 파일 닫기
	fclose(stageInfoFile);
}
