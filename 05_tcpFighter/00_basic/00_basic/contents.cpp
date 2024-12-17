#include <Windows.h>
#include <iostream>
#include "network.h"
#include "contents.h"
#include "makeProtocol.h"
#include "packetDefine.h"

unordered_map<uint16_t, Session*> sessionMap;
static DWORD oldTime = timeGetTime();

/// ���� ������Ʈ Ÿ�̹�(������) ����
bool setGameFrame()
{
	if ((timeGetTime() - oldTime) < (1000 / CONTENTS_FPS))
	{
		return true;
	}
	oldTime += (1000 / CONTENTS_FPS);
	return false;
}

/// ���� ���� ������Ʈ
void gameUpdate()
{
	// CONTENTS_FPS�� ���� ������Ʈ Ÿ�̹� ����
	if (setGameFrame())
	{
		return;
	}
	// ������ ���� ó��
	for (auto it = sessionMap.begin(); it != sessionMap.end();)
	{
		Session* session = it->second;
		if (session->hp <= 0)
		{
			// ���ó��
			disconnectSession(session, &it);
		}
		else
		{
			// �÷��̾� �̵� ����
			playerMove(session);
			// �÷��̾� ���� ����
			playerAttack(session);
			// iterator ++
			++it;
		}
	}
}

/// �÷��̾� �̵� ����
void playerMove(Session* session)
{
	if (session->state != dfCHAR_STATE_MOVE_START)
	{
		return;
	}
	// ���� �÷��̾� action�� ���� ó��
	switch (session->action)
	{
		// ���� �̵�
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
	// ������ �̵�
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
	// �� �̵�
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
	// ������ �� �̵�
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
	// ������ �̵�
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
	// ������ �Ʒ� �̵�
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
	// �Ʒ� �̵�
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
	// ���� �Ʒ� �̵�
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

/// �÷��̾� ���� ����
void playerAttack(Session* session)
{
	if (session->state != dfCHAR_STATE_ATTACK)
	{
		return;
	}
	for (auto it = sessionMap.begin(); it != sessionMap.end(); ++it)
	{
		// Ÿ �÷��̾�
		Session* otherSession = it->second;
		if (session->sessionId == otherSession->sessionId)
		{
			continue;
		}
		// ���� �÷��̾� action�� ���� ó��
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
	// ���� ���� ���� ����
	session->state = dfCHAR_STATE_IDEL;
}

/// ���� ������ �ִٸ� ������ ����, ��Ŷ����
void isInAttackRange(Session* session, Session* otherSession, uint8_t xRange, uint8_t yRange, uint8_t damage)
{
	// ������ �÷��̾��� ����
	uint8_t dir = session->direction;
	// ������ �÷��̾��� x ��ǥ
	int16_t otherXPosition = session->xPos;
	// ������ �÷��̾��� y ��ǥ
	int16_t otherYPosition = session->yPos;
	// Ÿ �÷��̾��� x ��ǥ
	int16_t xPosition = otherSession->xPos;
	// Ÿ �÷��̾��� y ��ǥ
	int16_t yPosition = otherSession->yPos;

	// ��������
	if (dir == dfPACKET_MOVE_DIR_LL)
	{
		// ���ݹ��� �ȿ� �ִٸ� ������ ó��
		if (xPosition >= otherXPosition - xRange && xPosition <= otherXPosition
			&& yPosition >= otherYPosition - yRange && yPosition <= otherYPosition + yRange)
		{
			otherSession->hp -= damage;
			damagePacket(session, otherSession);
		}
	}
	// ��������
	else if (dir == dfPACKET_MOVE_DIR_RR)
	{
		// ���ݹ��� �ȿ� �ִٸ� ������ ó��
		if (xPosition >= otherXPosition && xPosition <= otherXPosition + xRange
			&& yPosition >= otherYPosition - yRange && yPosition <= otherYPosition + yRange)
		{
			otherSession->hp -= damage;
			damagePacket(session, otherSession);
		}
	}
}

/// ����(�÷��̾�) ����
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
