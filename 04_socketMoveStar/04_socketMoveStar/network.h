#pragma once
#include "player.h"

/// ��������(non-blocking)�� listen ���·� ����
void openServer();
/// Ŭ�� ���� �� ���׸�Ʈ ������ / �ޱ�
void acceptClient();
/// Ŭ�� ���� ����
void acceptProc();
/// ID �Ҵ� send()
void starIdAssignSend(Player* player);
/// �� ���� send()
void starCreateSend(const Player* player);
/// �� ���� send()
void starCreateUnicastSend(Player* player, const Player* otherPlayer);
/// �� �������� ������ ����, �����ۿ� send()�� ���� ����
void sendUnicast(const char* msg, Player* player);
/// ��ü �������� send()
void sendBroadcast(const char* msg, const Player* player = nullptr);
/// �� ���� send()
void starDeleteSend(Player* player);
/// �޽��� �ޱ�
void recvProc(Player* player);
/// �޽��� ������
void sendProc(Player* player);
/// �� �̵� send()
void starMoveSend(Player* player, char* buffer);
/// ���� player ���� ����
void disConnect();
/// ���� ����
void closeSocket();
