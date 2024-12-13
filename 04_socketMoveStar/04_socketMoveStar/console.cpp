#include <windows.h>
#include <iostream>
#include "console.h"

using namespace std;

HANDLE  hConsole;
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];

/// 콘솔 제어 준비작업
void csInitial()
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	// 화면의 커서를 안보이게끔 설정
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	// 콘솔화면 (스텐다드 아웃풋) 핸들을 구함
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
}

/// 콘솔 화면의 커서를 X, Y 좌표로 이동
void csMoveCursor(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	// 원하는 위치로 커서를 이동
	SetConsoleCursorPosition(hConsole, stCoord);
}

/// 버퍼의 내용을 화면으로 찍어주는 함수
void bufferFlip()
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		csMoveCursor(0, iCnt);
		cout << szScreenBuffer[iCnt];
	}
}

/// 화면 버퍼를 지워주는 함수(프레임 잔상 제거)
/// 매 프레임 그림을 그리기 직전에 버퍼를 지워 준다.
void bufferClear()
{
	int iCnt;
	memset(szScreenBuffer, ' ', dfSCREEN_WIDTH * dfSCREEN_HEIGHT);

	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		szScreenBuffer[iCnt][dfSCREEN_WIDTH - 1] = '\0';
	}

}

/// 버퍼의 특정 위치에 원하는 문자열을 출력.
/// 입력 받은 X,Y 좌표에 아스키코드 하나를 출력 (버퍼에 그림) 
void spriteStringDraw(int iX, int iY, const wchar_t* string)
{
	if (iX + wcslen(string) >= dfSCREEN_WIDTH)
	{
		return;
	}

	memcpy(&szScreenBuffer[iY][iX], string, wcslen(string));
}
