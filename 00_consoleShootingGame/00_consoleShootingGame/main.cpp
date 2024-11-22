#include <iostream>
#include <windows.h>

#include "game.h"
#include "console.h"

//////////////////////////////////////////////////////////////
// 파일에 대한 설명은 (0. 설정파일설명.txt)에 있습니다.
// 
//////////////////////////////////////////////////////////////
int main(void)
{
	// 1. 게임 로드
	loadGame();

	// 프레임 설정
	int iFrameCnt = 0;
	// 실제 시간
	DWORD dwFPSTick = timeGetTime();
	// 우리의 시간(기준시간) - dfSCREEN_FPS_MS 초
	DWORD ourTime = timeGetTime();
	// 경과된 시간 (20밀리초에서 넘어간 시간)
	DWORD dwOverTick = 0;

	while (1)
	{
		// 2. 키보드 입력부
		keyPress();
		// 3. 게임 로직부
		updateGame();
		if (exitGame)
		{
			return 0;
		}
		// 4. 랜더부
		renderGame();

		// 5. 프레임 맞추기 (50프레임)
		// 프레임 업데이트
		++iFrameCnt;
		// 게임 전체 프레임 업데이트
		++frameCount;
		dwOverTick = timeGetTime() - ourTime;
		// 최소 & 최대값 제한
		if (dwOverTick <= 0)
		{
			dwOverTick = 0;
		}
		else if (dwOverTick >= dfSCREEN_FPS_MS)
		{
			dwOverTick = dfSCREEN_FPS_MS;
		}
		// 남은 시간만큼 쉬어준다.
		Sleep(dfSCREEN_FPS_MS - dwOverTick);
		// 우리의 시간(기준시간)은 일정하게 흘러간다.
		ourTime += dfSCREEN_FPS_MS;
		// 초당 프레임 계산
		if (timeGetTime() - dwFPSTick >= 1000)
		{
			printf("FPS: %d \n", iFrameCnt);
			iFrameCnt = 0;
			dwFPSTick += 1000;
		}
	}

}
