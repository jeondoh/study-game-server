#pragma once

// 전체 스테이지 개수
extern int maxStageNum;

//////////////////////////////////////////////////////////////
// 스테이지 파일명이 담긴 stageInfo.txt를 읽어오는 함수
// loadEnemy 호출
// createPlayer 호출
// 
//////////////////////////////////////////////////////////////
void loadStageInfo(const int stageNum);
