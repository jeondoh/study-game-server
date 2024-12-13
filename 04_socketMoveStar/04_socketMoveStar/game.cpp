#include "game.h"
#include "network.h"
#include "console.h"

/// ���� listen ���·�
void startServer()
{
	openServer();
}
/// Ŭ��� �������
void network()
{
	// Ŭ�� ���� �� ���׸�Ʈ ������ / �ޱ�
	acceptClient();
	// ���� �÷��̾� ���� ����
	disConnect();
}

/// ������
void render()
{
	bufferClear();
	// �÷��̾� ������
	renderPlayer();
	bufferFlip();
}

/// ���� ����
void closeGame()
{
	// �÷��̾� ����
	deleteAllPlayer();
	// ���� ����
	closeSocket();
}

