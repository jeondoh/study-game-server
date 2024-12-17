#pragma once

#include <cstdint>
#include "contents.h"

extern uint32_t acceptNum;
extern bool isShutdown;

/// ���� �ʱ�ȭ �� listen ����
void openServer();
/// ��Ʈ��ũ �ۼ��� ó��
void netIOProcess();
/// Ŭ���̾�Ʈ ���� accept, ���ǻ���
void acceptProc();
/// �޽��� ���Ÿ�Ժ��� �б�ó��
void packetProc(Session* session, const char* buffer, uint8_t byType);
/// �޽��� recv
void recvProc(Session* session);
/// �޽��� send
void sendProc(Session* session);

/// ����(�÷��̾�) ����
void createSession(const char* ip, uint32_t sockFd, uint16_t port);
/// ���� ���ӽ� �̹� ���ӵǾ� �ִ� �ٸ� ĳ������ ���� ��������
void getOtherSession(uint16_t sessionId);
/// �̹� ���ӵǾ� �ִ� ĳ���͵鿡�� ��� ������ ���� ���� ����
void createOtherSession(uint16_t sessionId);
/// ĳ���� �̵� ���� ó�� ����
/// ����� Ŭ�󿡼� ���� �̵�, �������� �� �ݿ�
void moveStartPacket(Session* session, const char* buffer);
/// ĳ���� �̵� ���� ó�� ����
/// ����� Ŭ�󿡼� ���� ó��, �������� �� �ݿ�
void moveStopPacket(Session* session, const char* buffer);
/// ĳ���� ����01 ó�� ����
void attack01Packet(Session* session, const char* buffer);
/// ĳ���� ����02 ó�� ����
void attack02Packet(Session* session, const char* buffer);
/// ĳ���� ����03 ó�� ����
void attack03Packet(Session* session, const char* buffer);
/// ������ ó��
void damagePacket(Session* session, Session* otherSession);
/// ���޹��� msg �����ۿ� ����
void sendUnicast(uint8_t byType, int32_t size, const char* msg, Session* pSession);
/// ���� ��ü���� msg ����
/// session�� nullptr�� �ƴҽ� �ش� ���� ������ ��ü ����
void sendBroadCast(uint8_t byType, int32_t size, const char* msg, const Session* pSession = nullptr);
/// ���� ���� ����
void disconnectSession(Session* session, unordered_map<uint16_t, Session*>::iterator* it = nullptr);
