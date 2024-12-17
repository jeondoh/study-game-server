#pragma once

#include "ringbuffer.h"
#include <cstdint>
#include <unordered_map>

using namespace std;

struct Session
{
	SOCKET socket;
	uint16_t sessionId;
	RingBuffer recvQ;
	RingBuffer sendQ;

	uint8_t state;
	uint32_t action;
	uint8_t direction;

	int16_t xPos;
	int16_t yPos;
	int8_t hp;
};

extern unordered_map<uint16_t, Session*> sessionMap;

/// ���� ������Ʈ Ÿ�̹�(������) ����
bool setGameFrame();
/// ���� ���� ������Ʈ
void gameUpdate();
/// �÷��̾� �̵� ����
void playerMove(Session* session);
/// �÷��̾� ���� ����
void playerAttack(Session* session);
/// ����(�÷��̾�) ����
void deleteSession(Session* session, unordered_map<uint16_t, Session*>::iterator* it);
/// ���� ������ �ִٸ� ������ ����, ��Ŷ����
void isInAttackRange(Session* session, Session* otherSession, uint8_t xRange, uint8_t yRange, uint8_t damage);
