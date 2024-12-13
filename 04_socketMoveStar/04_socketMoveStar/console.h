#pragma once

#define dfSCREEN_WIDTH		82		// 콘솔 가로 81칸 + NULL
#define dfSCREEN_HEIGHT		24		// 콘솔 세로 24칸

/// 콘솔 제어 준비작업
void csInitial();

/// 버퍼의 내용을 화면으로 찍어주는 함수
void bufferFlip();

/// 화면 버퍼를 지워주는 함수(프레임 잔상 제거)
/// 매 프레임 그림을 그리기 직전에 버퍼를 지워 준다.
void bufferClear();

/// 버퍼의 특정 위치에 원하는 문자열을 출력.
/// 입력 받은 X,Y 좌표에 아스키코드 하나를 출력 (버퍼에 그림) 
void spriteStringDraw(int iX, int iY, const wchar_t* string);
