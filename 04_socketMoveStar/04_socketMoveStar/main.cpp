#include <iostream>
#include "console.h"
#include "game.h"

using namespace std;

int wmain()
{
	wcout.imbue(locale("kor"));
	// 콘솔제어
	csInitial();
	// 서버 listen 상태로
	startServer();
	// 로직
	while (1)
	{
		// 클라와 소켓통신
		network();
		// 플레이어 랜더링
		render();
	}
	// 게임 종료
	closeGame();
}
