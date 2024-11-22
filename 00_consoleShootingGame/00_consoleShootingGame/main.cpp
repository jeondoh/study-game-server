#include <iostream>
#include <windows.h>

#include "game.h"
#include "console.h"

//////////////////////////////////////////////////////////////
// ���Ͽ� ���� ������ (0. �������ϼ���.txt)�� �ֽ��ϴ�.
// 
//////////////////////////////////////////////////////////////
int main(void)
{
	// 1. ���� �ε�
	loadGame();

	// ������ ����
	int iFrameCnt = 0;
	// ���� �ð�
	DWORD dwFPSTick = timeGetTime();
	// �츮�� �ð�(���ؽð�) - dfSCREEN_FPS_MS ��
	DWORD ourTime = timeGetTime();
	// ����� �ð� (20�и��ʿ��� �Ѿ �ð�)
	DWORD dwOverTick = 0;

	while (1)
	{
		// 2. Ű���� �Էº�
		keyPress();
		// 3. ���� ������
		updateGame();
		if (exitGame)
		{
			return 0;
		}
		// 4. ������
		renderGame();

		// 5. ������ ���߱� (50������)
		// ������ ������Ʈ
		++iFrameCnt;
		// ���� ��ü ������ ������Ʈ
		++frameCount;
		dwOverTick = timeGetTime() - ourTime;
		// �ּ� & �ִ밪 ����
		if (dwOverTick <= 0)
		{
			dwOverTick = 0;
		}
		else if (dwOverTick >= dfSCREEN_FPS_MS)
		{
			dwOverTick = dfSCREEN_FPS_MS;
		}
		// ���� �ð���ŭ �����ش�.
		Sleep(dfSCREEN_FPS_MS - dwOverTick);
		// �츮�� �ð�(���ؽð�)�� �����ϰ� �귯����.
		ourTime += dfSCREEN_FPS_MS;
		// �ʴ� ������ ���
		if (timeGetTime() - dwFPSTick >= 1000)
		{
			printf("FPS: %d \n", iFrameCnt);
			iFrameCnt = 0;
			dwFPSTick += 1000;
		}
	}

}
