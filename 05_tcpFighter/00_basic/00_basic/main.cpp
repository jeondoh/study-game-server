#pragma comment(lib, "winmm.lib")
#include <cstdint>
#include <Windows.h>
#include <iostream>
#include "network.h"
#include "contents.h"

using namespace std;

int wmain()
{
	wcout.imbue(locale("kor"));
	srand(static_cast<uint16_t>(time(nullptr)));
	timeBeginPeriod(1);

	wcout << L"SERVER START!!!" << endl;

	// socket listen 상태
	openServer();

	while (!isShutdown)
	{
		// 네트워크 송수신 처리
		netIOProcess();
		// 게임 로직 업데이트
		gameUpdate();
	}
}

