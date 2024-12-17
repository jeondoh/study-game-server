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

/// 게임 업데이트 타이밍(프레임) 설정
bool setGameFrame();
/// 게임 로직 업데이트
void gameUpdate();
/// 플레이어 이동 로직
void playerMove(Session* session);
/// 플레이어 공격 로직
void playerAttack(Session* session);
/// 세션(플레이어) 삭제
void deleteSession(Session* session, unordered_map<uint16_t, Session*>::iterator* it);
/// 공격 범위에 있다면 데미지 입힘, 패킷생성
void isInAttackRange(Session* session, Session* otherSession, uint8_t xRange, uint8_t yRange, uint8_t damage);
