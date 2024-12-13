#include <windows.h>
#include <iostream>
#include "console.h"

using namespace std;

HANDLE  hConsole;
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];

/// �ܼ� ���� �غ��۾�
void csInitial()
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	// ȭ���� Ŀ���� �Ⱥ��̰Բ� ����
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;

	// �ܼ�ȭ�� (���ٴٵ� �ƿ�ǲ) �ڵ��� ����
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
}

/// �ܼ� ȭ���� Ŀ���� X, Y ��ǥ�� �̵�
void csMoveCursor(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	// ���ϴ� ��ġ�� Ŀ���� �̵�
	SetConsoleCursorPosition(hConsole, stCoord);
}

/// ������ ������ ȭ������ ����ִ� �Լ�
void bufferFlip()
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		csMoveCursor(0, iCnt);
		cout << szScreenBuffer[iCnt];
	}
}

/// ȭ�� ���۸� �����ִ� �Լ�(������ �ܻ� ����)
/// �� ������ �׸��� �׸��� ������ ���۸� ���� �ش�.
void bufferClear()
{
	int iCnt;
	memset(szScreenBuffer, ' ', dfSCREEN_WIDTH * dfSCREEN_HEIGHT);

	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		szScreenBuffer[iCnt][dfSCREEN_WIDTH - 1] = '\0';
	}

}

/// ������ Ư�� ��ġ�� ���ϴ� ���ڿ��� ���.
/// �Է� ���� X,Y ��ǥ�� �ƽ�Ű�ڵ� �ϳ��� ��� (���ۿ� �׸�) 
void spriteStringDraw(int iX, int iY, const wchar_t* string)
{
	if (iX + wcslen(string) >= dfSCREEN_WIDTH)
	{
		return;
	}

	memcpy(&szScreenBuffer[iY][iX], string, wcslen(string));
}
