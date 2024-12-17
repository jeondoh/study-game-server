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

	// socket listen ����
	openServer();

	while (!isShutdown)
	{
		// ��Ʈ��ũ �ۼ��� ó��
		netIOProcess();
		// ���� ���� ������Ʈ
		gameUpdate();
	}
}

