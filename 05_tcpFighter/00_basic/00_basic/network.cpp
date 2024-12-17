#pragma comment(lib, "ws2_32")
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>
#include "network.h"
#include "packetDefine.h"
#include "makeProtocol.h"

using namespace std;

uint32_t acceptNum = 0;
bool isShutdown = false;

SOCKET listenSock;
SOCKET clientSock;
SOCKADDR_IN clientaddr;

/// ���� �ʱ�ȭ �� listen ����
void openServer()
{
	int nonBlockRes;
	int bindRes;
	int listenRes;

	// winsock �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return;
	}
	wcout << L"WSAStartup #" << endl;
	// socket()
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		wcout << L"socket() error: " << errCode << endl;
		return;
	}
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	bindRes = bind(listenSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (bindRes == SOCKET_ERROR)
	{
		bindRes = WSAGetLastError();
		wcout << L"bind() error: " << bindRes << endl;
		return;
	}
	wcout << L"Bind OK # Port: " << SERVER_PORT << endl;
	// listen()
	listenRes = listen(listenSock, SOMAXCONN);
	if (listenRes == SOCKET_ERROR)
	{
		listenRes = WSAGetLastError();
		wcout << L"listen() error: " << listenRes << endl;
		return;
	}
	wcout << L"Listen OK #" << endl;
	// ����ŷ ����
	u_long on = 1;
	nonBlockRes = ioctlsocket(listenSock, FIONBIO, &on);
	if (nonBlockRes == SOCKET_ERROR)
	{
		nonBlockRes = WSAGetLastError();
		wcout << L"ioctlsocket() error: " << nonBlockRes << endl;
		return;
	}
}

/// ��Ʈ��ũ �ۼ��� ó��
void netIOProcess()
{
	int selectRes;
	Session* session;
	// ���� �� �ʱ�ȭ
	FD_SET rset;
	FD_SET wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	// listen ���� �ֱ�
	FD_SET(listenSock, &rset);
	// �������� ��� Ŭ���̾�Ʈ�� ���� ���� üũ
	for (auto sIt = sessionMap.begin(); sIt != sessionMap.end(); ++sIt)
	{
		session = sIt->second;
		// rset���
		FD_SET(session->socket, &rset);
		// sendQ�� �����Ͱ� �ִٸ� wset���
		if (session->sendQ.getUseSize() > 0)
		{
			FD_SET(session->socket, &wset);
		}
	}
	// select() ��
	TIMEVAL timeval;
	timeval.tv_sec = 0;
	timeval.tv_usec = 0;
	selectRes = select(0, &rset, &wset, NULL, &timeval);
	if (selectRes == SOCKET_ERROR)
	{
		selectRes = WSAGetLastError();
		wcout << L"select() error: " << selectRes << endl;
		isShutdown = true;
		return;
	}
	else if (selectRes > 0)
	{
		// client accept
		if (FD_ISSET(listenSock, &rset))
		{
			acceptProc();
		}
		// ���Ǹ��� ���� Ȯ��
		for (auto sIt = sessionMap.begin(); sIt != sessionMap.end(); ++sIt)
		{
			session = sIt->second;
			if (FD_ISSET(session->socket, &rset))
			{
				recvProc(session);
			}
			if (FD_ISSET(session->socket, &wset))
			{
				sendProc(session);
			}
		}
	}
}

/// Ŭ���̾�Ʈ ���� accept, ���ǻ���
void acceptProc()
{
	int addrlen;
	char clientIp[IP_SIZE];

	addrlen = sizeof(clientaddr);
	ZeroMemory(&clientaddr, sizeof(addrlen));
	// Ŭ���̾�Ʈ accept()
	clientSock = accept(listenSock, (SOCKADDR*)&clientaddr, &addrlen);
	if (clientSock == INVALID_SOCKET)
	{
		int errCode = WSAGetLastError();
		wcout << L"accept() error: " << errCode << endl;
		return;
	}
	++acceptNum;
	inet_ntop(AF_INET, &clientaddr.sin_addr, clientIp, IP_SIZE);
	wcout << L"Connect # IP: " << clientIp << ", SessionID: " << acceptNum << endl;
	// ���� ����
	createSession(clientIp, clientSock, ntohs(clientaddr.sin_port));
	// ��� ������ ���ǿ��� �̹� ���ӵǾ� �ִ� ĳ������ ���� ����
	getOtherSession(acceptNum);
	// �̹� ���ӵǾ� �ִ� ĳ���͵鿡�� ��� ������ ���� ���� ����
	createOtherSession(acceptNum);
}

/// �޽��� recv
void recvProc(Session* session)
{
	int recvRes;
	int freeSize = session->recvQ.getFreeSize();
	char recvBuffer[DEFAULT_SIZE];
	char dequeBuffer[DEFAULT_SIZE];
	// recv()
	recvRes = recv(session->socket, recvBuffer, freeSize, 0);
	if (recvRes == SOCKET_ERROR)
	{
		recvRes = WSAGetLastError();
		if (recvRes != WSAEWOULDBLOCK)
		{
			wcout << L"sessionId: " << session->sessionId << L", recvRes() error: " << recvRes << endl;
			// ���� ���� ����
			wcout << L"recv() - disconnect Session: " << session->sessionId << endl;
			disconnectSession(session);
			return;
		}
	}
	// �������
	else if (recvRes == 0)
	{
		wcout << L"client closed, disconnect Session: " << session->sessionId << endl;
		disconnectSession(session);
	}
	// recvQ�� ����
	else if (recvRes > 0)
	{
		session->recvQ.enqueue(recvBuffer, recvRes);
	}
	// ��Ŷó��
	while (1)
	{
		int peekRes;
		int useSize = session->recvQ.getUseSize();
		int headerSize = sizeof(MsgHeader);
		// �ּ����� ����� �ִ��� Ȯ��
		if (headerSize > useSize)
		{
			return;
		}
		peekRes = session->recvQ.peek(recvBuffer, headerSize);
		if (peekRes != headerSize)
		{
			return;
		}
		MsgHeader* header = (MsgHeader*)&recvBuffer;
		if (header->byCode != PACKET_CODE)
		{
			// ���� ���� ����
			wcout << L"recv() header unmarshalling - disconnect Session: " << session->sessionId << endl;
			disconnectSession(session);
			return;
		}
		if (useSize >= (header->bySize + headerSize))
		{
			// peek�ߴ� ��� ������ ��ŭ front �̵�
			session->recvQ.moveFront(headerSize);
			// ���۷� ��Ŷ�� ������.
			session->recvQ.dequeue(dequeBuffer, header->bySize);
			// ��� Ÿ�Ժ��� �б�ó��
			packetProc(session, dequeBuffer, header->byType);
		}

	}
}

/// �޽��� send
void sendProc(Session* session)
{
	int sendRes;
	int peekRes;
	int useSize = session->sendQ.getUseSize();
	char buffer[DEFAULT_SIZE];

	if (useSize <= 0)
	{
		return;
	}
	peekRes = session->sendQ.peek(buffer, useSize);
	if (useSize != peekRes)
	{
		return;
	}
	// send()
	sendRes = send(session->socket, buffer, peekRes, 0);
	if (sendRes == SOCKET_ERROR)
	{
		sendRes = WSAGetLastError();
		if (sendRes != WSAEWOULDBLOCK)
		{
			wcout << L"socketNumber: " << session->socket << L", sendRes() error: " << sendRes << endl;
			// ���� ���� ����
			wcout << L"send() " << session->sessionId << endl;
			disconnectSession(session);
			return;
		}
	}
	else if (sendRes > 0)
	{
		session->sendQ.moveFront(sendRes);
	}
}

/// �޽��� ���Ÿ�Ժ��� �б�ó��
void packetProc(Session* session, const char* buffer, uint8_t byType)
{
	switch (byType)
	{
		// ĳ���� �̵�����
	case dfPACKET_CS_MOVE_START:
		moveStartPacket(session, buffer);
		return;
		// ĳ���� �̵�����
	case dfPACKET_CS_MOVE_STOP:
		moveStopPacket(session, buffer);
		return;
	case dfPACKET_CS_ATTACK1:
		attack01Packet(session, buffer);
		return;
	case dfPACKET_CS_ATTACK2:
		attack02Packet(session, buffer);
		return;
	case dfPACKET_CS_ATTACK3:
		attack03Packet(session, buffer);
		return;
	}
}

/// ���� ����
void createSession(const char* ip, uint32_t sockFd, uint16_t port)
{
	Session* session = new Session;
	SCCreateMyCharacter scCreate;
	// ĳ���� ��ǥ ����
	uint16_t xPosMin = dfRANGE_MOVE_LEFT + dfPACKET_MOVE_FRAME_UNIT_X;
	uint16_t xPosMax = dfRANGE_MOVE_RIGHT - dfPACKET_MOVE_FRAME_UNIT_X;
	uint16_t yPosMin = dfRANGE_MOVE_TOP + dfPACKET_MOVE_FRAME_UNIT_Y;
	uint16_t yPosMax = dfRANGE_MOVE_BOTTOM - dfPACKET_MOVE_FRAME_UNIT_Y;

	uint16_t xPos = xPosMin + rand() % (xPosMax - xPosMin);
	uint16_t yPos = yPosMin + rand() % (yPosMax - yPosMin);

	uint8_t dir = rand() % 2 == 0 ? dfPACKET_MOVE_DIR_LL : dfPACKET_MOVE_DIR_RR;
	// ���� ����
	session->socket = sockFd;
	session->sessionId = acceptNum;
	session->state = dfCHAR_STATE_IDEL;
	session->direction = dir;
	session->xPos = xPos;
	session->yPos = yPos;
	session->hp = dfHP;
	sessionMap.insert(make_pair(session->sessionId, session));
	// ĳ���� �Ҵ� �޽��� ����
	makeMsgSCCreateCharacter(scCreate, session->sessionId, dir, session->xPos, session->yPos, session->hp);
	// �۽� �����ۿ� ����
	wcout << L"# PACKET_CONNECT #: SessionID: " << session->sessionId << endl;
	sendUnicast(dfPACKET_SC_CREATE_MY_CHARACTER, sizeof(scCreate), (char*)&scCreate, session);
	wcout << L"Create Character # SessionID: " << session->sessionId << "	X: " << xPos << "	Y: " << yPos << endl;
}

/// ���� ���ӽ� �̹� ���ӵǾ� �ִ� �ٸ� ĳ������ ���� ��������
void getOtherSession(uint16_t sessionId)
{
	// �ٸ� ���� ĳ���� ��������
	for (auto it = sessionMap.begin(); it != sessionMap.end(); ++it)
	{
		Session* session = it->second;
		// ���� ����
		if (sessionId == session->sessionId)
		{
			continue;
		}
		// ��� �������(������) ĳ���� ����
		Session* findSession = sessionMap.find(sessionId)->second;
		if (findSession == nullptr)
		{
			return;
		}
		SCCreateOtherCharacter scOther;
		// �޽��� ����
		makeMsgSCCreateOtherCharacter(scOther, session->sessionId, session->direction, session->xPos, session->yPos, session->hp);
		// �۽� �����ۿ� ����
		sendUnicast(dfPACKET_SC_CREATE_OTHER_CHARACTER, sizeof(scOther), (char*)&scOther, findSession);
	}
}

/// �̹� ���ӵǾ� �ִ� ĳ���͵鿡�� ��� ������ ���� ���� ����
void createOtherSession(uint16_t sessionId)
{
	SCCreateOtherCharacter scOther;
	// ��� �������(������) ĳ���� ����
	Session* findSession = sessionMap.find(sessionId)->second;
	if (findSession == nullptr)
	{
		return;
	}
	// �޽��� ����
	makeMsgSCCreateOtherCharacter(scOther, findSession->sessionId, findSession->direction, findSession->xPos, findSession->yPos, findSession->hp);
	// ���� ���� ���� ��� �������� ��Ŷ ����
	sendBroadCast(dfPACKET_SC_CREATE_OTHER_CHARACTER, sizeof(scOther), (char*)&scOther, findSession);
}

/// ĳ���� �̵� ���� ó�� ����
/// ����� Ŭ�󿡼� ���� �̵�, �������� �� �ݿ�
void moveStartPacket(Session* session, const char* buffer)
{
	CSMoveStart* csMoveStart = (CSMoveStart*)buffer;
	// �̵� ������ ����� ��� ���� ����
	if (abs(session->xPos - csMoveStart->X) > dfERROR_RANGE || abs(session->yPos - csMoveStart->Y) > dfERROR_RANGE)
	{
		wcout << L"dfERROR_RANGE! moveStart - disconnect Session: " << session->sessionId << endl;
		disconnectSession(session);
		return;
	}
	session->state = dfCHAR_STATE_MOVE_START;
	session->action = csMoveStart->Direction;
	// ���� ����
	switch (csMoveStart->Direction)
	{
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		session->direction = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LD:
		session->direction = dfPACKET_MOVE_DIR_LL;
		break;
	}
	// ��ǥ����
	session->xPos = csMoveStart->X;
	session->yPos = csMoveStart->Y;
	// ���� �������� ���ǵ鿡�� ���޹��� �÷��̾ �����δٴ� ���� ���� �Ѹ�(���޹��� session ����� ����)
	SCMoveStart scMoveStart;
	makeMsgSCMoveStart(scMoveStart, session->sessionId, csMoveStart->Direction, session->xPos, session->yPos);
	wcout << L"# PACKET_MOVE_START #: SessionID: " << session->sessionId <<
		L" / Direction: " << session->direction << L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_MOVE_START, sizeof(scMoveStart), (char*)&scMoveStart, session);
}

/// ĳ���� �̵� ���� ó�� ����
/// ����� Ŭ�󿡼� ���� ó��, �������� �� �ݿ�
void moveStopPacket(Session* session, const char* buffer)
{
	CSMoveStop* csMoveStop = (CSMoveStop*)buffer;
	// �̵� ������ ����� ��� ���� ����
	if (abs(session->xPos - csMoveStop->X) > dfERROR_RANGE || abs(session->yPos - csMoveStop->Y) > dfERROR_RANGE)
	{
		wcout << L"dfERROR_RANGE! moveStop - disconnect Session: " << session->sessionId << endl;
		disconnectSession(session);
		return;
	}
	session->state = dfCHAR_STATE_MOVE_STOP;
	session->action = csMoveStop->Direction;
	// ���� ����
	switch (csMoveStop->Direction)
	{
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		session->direction = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LD:
		session->direction = dfPACKET_MOVE_DIR_LL;
		break;
	}
	// ��ǥ����
	session->xPos = csMoveStop->X;
	session->yPos = csMoveStop->Y;
	// ���� �������� ���ǵ鿡�� ���޹��� �÷��̾ �������� ������ ���� ���� �Ѹ�(���޹��� session ����� ����)
	SCMoveStop scMoveStop;
	makeMsgSCMoveStop(scMoveStop, session->sessionId, csMoveStop->Direction, session->xPos, session->yPos);
	wcout << L"# PACKET_MOVE_STOP #: SessionID: " << session->sessionId <<
		L" / Direction: " << session->direction << L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_MOVE_STOP, sizeof(scMoveStop), (char*)&scMoveStop, session);
}

/// ĳ���� ����01 ó�� ����
void attack01Packet(Session* session, const char* buffer)
{
	CSAttack01* attack = (CSAttack01*)buffer;

	session->state = dfCHAR_STATE_ATTACK;
	session->action = dfPACKET_CS_ATTACK1;
	session->direction = attack->Direction;
	// ��ǥ����
	session->xPos = attack->X;
	session->yPos = attack->Y;
	// ���� �������� ���ǵ鿡�� ���޹��� �÷��̾ �����Ѵٴ°��� ���� �Ѹ�(���޹��� session ����� ����)
	SCAttack01 scAttack01;
	makeMsgSCAttack01(scAttack01, session->sessionId, session->direction, session->xPos, session->yPos);
	wcout << L"# PACKET_ATTACK1 # SessionID: " << session->sessionId << L" / Direction: " << session->direction
		<< L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_ATTACK1, sizeof(scAttack01), (char*)&scAttack01, session);
}

/// ĳ���� ����02 ó�� ����
void attack02Packet(Session* session, const char* buffer)
{
	CSAttack02* attack = (CSAttack02*)buffer;

	session->state = dfCHAR_STATE_ATTACK;
	session->action = dfPACKET_CS_ATTACK2;
	session->direction = attack->Direction;
	// ��ǥ����
	session->xPos = attack->X;
	session->yPos = attack->Y;
	// ���� �������� ���ǵ鿡�� ���޹��� �÷��̾ �����Ѵٴ°��� ���� �Ѹ�(���޹��� session ����� ����)
	SCAttack02 scAttack02;
	makeMsgSCAttack02(scAttack02, session->sessionId, session->direction, session->xPos, session->yPos);
	wcout << L"# PACKET_ATTACK2 # SessionID: " << session->sessionId << L" / Direction: " << session->direction
		<< L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_ATTACK2, sizeof(scAttack02), (char*)&scAttack02, session);
}

/// ĳ���� ����03 ó�� ����
void attack03Packet(Session* session, const char* buffer)
{
	CSAttack03* attack = (CSAttack03*)buffer;

	session->state = dfCHAR_STATE_ATTACK;
	session->action = dfPACKET_CS_ATTACK3;
	session->direction = attack->Direction;
	// ��ǥ����
	session->xPos = attack->X;
	session->yPos = attack->Y;
	// ���� �������� ���ǵ鿡�� ���޹��� �÷��̾ �����Ѵٴ°��� ���� �Ѹ�(���޹��� session ����� ����)
	SCAttack03 scAttack03;
	makeMsgSCAttack03(scAttack03, session->sessionId, session->direction, session->xPos, session->yPos);
	wcout << L"# PACKET_ATTACK3 # SessionID: " << session->sessionId << L" / Direction: " << session->direction
		<< L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_ATTACK3, sizeof(scAttack03), (char*)&scAttack03, session);
}

/// ������ ó��
void damagePacket(Session* session, Session* otherSession)
{
	SCDamage damage;
	makeMsgSCDamage(damage, session->sessionId, otherSession->sessionId, otherSession->hp);
	sendBroadCast(dfPACKET_SC_DAMAGE, sizeof(damage), (char*)&damage);
}

/// ���޹��� msg �����ۿ� ����
void sendUnicast(uint8_t byType, int32_t size, const char* msg, Session* pSession)
{
	// ��� ����
	MsgHeader header;
	header.byCode = PACKET_CODE;
	header.bySize = size;
	header.byType = byType;

	pSession->sendQ.enqueue((char*)&header, sizeof(header));
	pSession->sendQ.enqueue(msg, size);
}

/// ���� ��ü���� msg ����
/// session�� nullptr�� �ƴҽ� �ش� ���� ������ ��ü ����
void sendBroadCast(uint8_t byType, int32_t size, const char* msg, const Session* pSession)
{
	for (auto it = sessionMap.begin(); it != sessionMap.end(); ++it)
	{
		Session* session = it->second;
		// ����
		if (pSession != nullptr && (pSession->sessionId == session->sessionId))
		{
			continue;
		}
		// ���޹��� msg �����ۿ� ����
		sendUnicast(byType, size, msg, session);
	}
}

/// ���� ���� ����
void disconnectSession(Session* session, unordered_map<uint16_t, Session*>::iterator* it)
{
	closesocket(session->socket);
	deleteSession(session, it);
}
