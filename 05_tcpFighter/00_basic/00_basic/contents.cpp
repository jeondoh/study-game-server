#include <Windows.h>
#include <iostream>
#include "network.h"
#include "contents.h"
#include "makeProtocol.h"
#include "packetDefine.h"

unordered_map<uint16_t, Session*> sessionMap;
static DWORD oldTime = timeGetTime();

/// 게임 업데이트 타이밍(프레임) 설정
bool setGameFrame()
{
	if ((timeGetTime() - oldTime) < (1000 / CONTENTS_FPS))
	{
		return true;
	}
	oldTime += (1000 / CONTENTS_FPS);
	return false;
}

/// 게임 로직 업데이트
void gameUpdate()
{
	// CONTENTS_FPS로 게임 업데이트 타이밍 설정
	if (setGameFrame())
	{
		return;
	}
	// 컨텐츠 로직 처리
	for (auto it = sessionMap.begin(); it != sessionMap.end();)
	{
		Session* session = it->second;
		if (session->hp <= 0)
		{
			// 사망처리
			disconnectSession(session, &it);
		}
		else
		{
			// 플레이어 이동 로직
			playerMove(session);
			// 플레이어 공격 로직
			playerAttack(session);
			// iterator ++
			++it;
		}
	}
}

/// 플레이어 이동 로직
void playerMove(Session* session)
{
	if (session->state != dfCHAR_STATE_MOVE_START)
	{
		return;
	}
	// 현재 플레이어 action에 따른 처리
	switch (session->action)
	{
		// 왼쪽 이동
	case dfPACKET_MOVE_DIR_LL:
	{
		int xPosition = session->xPos - dfPACKET_MOVE_FRAME_UNIT_X;
		if (xPosition <= dfRANGE_MOVE_LEFT)
		{
			break;
		}
		session->xPos = xPosition;
		wcout << L"# gameRun:LL # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;
		break;
	}
	// 왼쪽위 이동
	case dfPACKET_MOVE_DIR_LU:
	{
		int xPosition = session->xPos - dfPACKET_MOVE_FRAME_UNIT_X;
		int yPosition = session->yPos - dfPACKET_MOVE_FRAME_UNIT_Y;
		if (xPosition <= dfRANGE_MOVE_LEFT || yPosition <= dfRANGE_MOVE_TOP)
		{
			break;
		}
		session->xPos = xPosition;
		session->yPos = yPosition;
		wcout << L"# gameRun:LU # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;

		break;
	}
	// 위 이동
	case dfPACKET_MOVE_DIR_UU:
	{
		int yPosition = session->yPos - dfPACKET_MOVE_FRAME_UNIT_Y;
		if (yPosition <= dfRANGE_MOVE_TOP)
		{
			break;
		}
		session->yPos = yPosition;
		wcout << L"# gameRun:UU # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;
		break;
	}
	// 오른쪽 위 이동
	case dfPACKET_MOVE_DIR_RU:
	{
		int xPosition = session->xPos + dfPACKET_MOVE_FRAME_UNIT_X;
		int yPosition = session->yPos - dfPACKET_MOVE_FRAME_UNIT_Y;
		if (xPosition >= dfRANGE_MOVE_RIGHT || yPosition <= dfRANGE_MOVE_TOP)
		{
			break;
		}
		session->xPos = xPosition;
		session->yPos = yPosition;
		wcout << L"# gameRun:RU # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;
		break;
	}
	// 오른쪽 이동
	case dfPACKET_MOVE_DIR_RR:
	{
		int xPosition = session->xPos + dfPACKET_MOVE_FRAME_UNIT_X;
		if (xPosition >= dfRANGE_MOVE_RIGHT)
		{
			break;
		}
		session->xPos = xPosition;
		wcout << L"# gameRun:RR # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;
		break;
	}
	// 오른쪽 아래 이동
	case dfPACKET_MOVE_DIR_RD:
	{
		int xPosition = session->xPos + dfPACKET_MOVE_FRAME_UNIT_X;
		int yPosition = session->yPos + dfPACKET_MOVE_FRAME_UNIT_Y;
		if (xPosition >= dfRANGE_MOVE_RIGHT || yPosition >= dfRANGE_MOVE_BOTTOM)
		{
			break;
		}
		session->xPos = xPosition;
		session->yPos = yPosition;
		wcout << L"# gameRun:RU # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;
		break;
	}
	// 아래 이동
	case dfPACKET_MOVE_DIR_DD:
	{
		int yPosition = session->yPos + dfPACKET_MOVE_FRAME_UNIT_Y;
		if (yPosition >= dfRANGE_MOVE_BOTTOM)
		{
			break;
		}
		session->yPos = yPosition;
		wcout << L"# gameRun:DD # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;
		break;
	}
	// 왼쪽 아래 이동
	case dfPACKET_MOVE_DIR_LD:
	{
		int xPosition = session->xPos - dfPACKET_MOVE_FRAME_UNIT_X;
		int yPosition = session->yPos + dfPACKET_MOVE_FRAME_UNIT_Y;
		if (xPosition <= dfRANGE_MOVE_LEFT || yPosition >= dfRANGE_MOVE_BOTTOM)
		{
			break;
		}
		session->xPos = xPosition;
		session->yPos = yPosition;
		wcout << L"# gameRun:LD # SessionID: " << session->sessionId << " / X: " << session->xPos << " / Y: " << session->yPos << endl;
		break;
	}
	}
}

/// 플레이어 공격 로직
void playerAttack(Session* session)
{
	if (session->state != dfCHAR_STATE_ATTACK)
	{
		return;
	}
	for (auto it = sessionMap.begin(); it != sessionMap.end(); ++it)
	{
		// 타 플레이어
		Session* otherSession = it->second;
		if (session->sessionId == otherSession->sessionId)
		{
			continue;
		}
		// 현재 플레이어 action에 따른 처리
		uint32_t action = session->action;
		if (action == dfPACKET_CS_ATTACK1)
		{
			isInAttackRange(session, otherSession, dfATTACK1_RANGE_X, dfATTACK1_RANGE_Y, dfATTACK1_DAMAGE);
		}
		else if (action == dfPACKET_CS_ATTACK2)
		{
			isInAttackRange(session, otherSession, dfATTACK2_RANGE_X, dfATTACK2_RANGE_Y, dfATTACK2_DAMAGE);
		}
		else if (action == dfPACKET_CS_ATTACK3)
		{
			isInAttackRange(session, otherSession, dfATTACK3_RANGE_X, dfATTACK3_RANGE_Y, dfATTACK3_DAMAGE);
		}
	}
	// 공격 이후 상태 변경
	session->state = dfCHAR_STATE_IDEL;
}

/// 공격 범위에 있다면 데미지 입힘, 패킷생성
void isInAttackRange(Session* session, Session* otherSession, uint8_t xRange, uint8_t yRange, uint8_t damage)
{
	// 공격한 플레이어의 방향
	uint8_t dir = session->direction;
	// 공격한 플레이어의 x 좌표
	int16_t otherXPosition = session->xPos;
	// 공격한 플레이어의 y 좌표
	int16_t otherYPosition = session->yPos;
	// 타 플레이어의 x 좌표
	int16_t xPosition = otherSession->xPos;
	// 타 플레이어의 y 좌표
	int16_t yPosition = otherSession->yPos;

	// 좌측공격
	if (dir == dfPACKET_MOVE_DIR_LL)
	{
		// 공격범위 안에 있다면 데미지 처리
		if (xPosition >= otherXPosition - xRange && xPosition <= otherXPosition
			&& yPosition >= otherYPosition - yRange && yPosition <= otherYPosition + yRange)
		{
			otherSession->hp -= damage;
			damagePacket(session, otherSession);
		}
	}
	// 우측공격
	else if (dir == dfPACKET_MOVE_DIR_RR)
	{
		// 공격범위 안에 있다면 데미지 처리
		if (xPosition >= otherXPosition && xPosition <= otherXPosition + xRange
			&& yPosition >= otherYPosition - yRange && yPosition <= otherYPosition + yRange)
		{
			otherSession->hp -= damage;
			damagePacket(session, otherSession);
		}
	}
}

/// 세션(플레이어) 삭제
void deleteSession(Session* session, unordered_map<uint16_t, Session*>::iterator* it)
{
	SCDeleteCharacter deleteChar;
	makeMsgSCDeleteCharacter(deleteChar, session->sessionId);
	wcout << L"# PACKET_DELETE_CHARACTER #: SessionID: " << session->sessionId << endl;
	sendBroadCast(dfPACKET_SC_DELETE_CHARACTER, sizeof(deleteChar), (char*)&deleteChar, session);

	if (it)
	{
		*it = sessionMap.erase(*it);
	}
	else
	{
		sessionMap.erase(session->sessionId);
	}
	delete session;
}
