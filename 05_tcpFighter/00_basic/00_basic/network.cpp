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

/// 서버 초기화 및 listen 상태
void openServer()
{
	int nonBlockRes;
	int bindRes;
	int listenRes;

	// winsock 초기화
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
	// 논블로킹 설정
	u_long on = 1;
	nonBlockRes = ioctlsocket(listenSock, FIONBIO, &on);
	if (nonBlockRes == SOCKET_ERROR)
	{
		nonBlockRes = WSAGetLastError();
		wcout << L"ioctlsocket() error: " << nonBlockRes << endl;
		return;
	}
}

/// 네트워크 송수신 처리
void netIOProcess()
{
	int selectRes;
	Session* session;
	// 소켓 셋 초기화
	FD_SET rset;
	FD_SET wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	// listen 소켓 넣기
	FD_SET(listenSock, &rset);
	// 접속중인 모든 클라이언트에 대해 소켓 체크
	for (auto sIt = sessionMap.begin(); sIt != sessionMap.end(); ++sIt)
	{
		session = sIt->second;
		// rset등록
		FD_SET(session->socket, &rset);
		// sendQ에 데이터가 있다면 wset등록
		if (session->sendQ.getUseSize() > 0)
		{
			FD_SET(session->socket, &wset);
		}
	}
	// select() 모델
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
		// 세션마다 반응 확인
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

/// 클라이언트 접속 accept, 세션생성
void acceptProc()
{
	int addrlen;
	char clientIp[IP_SIZE];

	addrlen = sizeof(clientaddr);
	ZeroMemory(&clientaddr, sizeof(addrlen));
	// 클라이언트 accept()
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
	// 세션 생성
	createSession(clientIp, clientSock, ntohs(clientaddr.sin_port));
	// 방금 생성된 세션에게 이미 접속되어 있는 캐릭터의 정보 전송
	getOtherSession(acceptNum);
	// 이미 접속되어 있는 캐릭터들에게 방금 생성된 세션 정보 전송
	createOtherSession(acceptNum);
}

/// 메시지 recv
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
			// 소켓 연결 해제
			wcout << L"recv() - disconnect Session: " << session->sessionId << endl;
			disconnectSession(session);
			return;
		}
	}
	// 연결끊김
	else if (recvRes == 0)
	{
		wcout << L"client closed, disconnect Session: " << session->sessionId << endl;
		disconnectSession(session);
	}
	// recvQ에 저장
	else if (recvRes > 0)
	{
		session->recvQ.enqueue(recvBuffer, recvRes);
	}
	// 패킷처리
	while (1)
	{
		int peekRes;
		int useSize = session->recvQ.getUseSize();
		int headerSize = sizeof(MsgHeader);
		// 최소한의 사이즈가 있는지 확인
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
			// 소켓 연결 해제
			wcout << L"recv() header unmarshalling - disconnect Session: " << session->sessionId << endl;
			disconnectSession(session);
			return;
		}
		if (useSize >= (header->bySize + headerSize))
		{
			// peek했던 헤더 사이즈 만큼 front 이동
			session->recvQ.moveFront(headerSize);
			// 버퍼로 패킷을 빼낸다.
			session->recvQ.dequeue(dequeBuffer, header->bySize);
			// 헤더 타입별로 분기처리
			packetProc(session, dequeBuffer, header->byType);
		}

	}
}

/// 메시지 send
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
			// 소켓 연결 해제
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

/// 메시지 헤더타입별로 분기처리
void packetProc(Session* session, const char* buffer, uint8_t byType)
{
	switch (byType)
	{
		// 캐릭터 이동시작
	case dfPACKET_CS_MOVE_START:
		moveStartPacket(session, buffer);
		return;
		// 캐릭터 이동중지
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

/// 세션 생성
void createSession(const char* ip, uint32_t sockFd, uint16_t port)
{
	Session* session = new Session;
	SCCreateMyCharacter scCreate;
	// 캐릭터 좌표 지정
	uint16_t xPosMin = dfRANGE_MOVE_LEFT + dfPACKET_MOVE_FRAME_UNIT_X;
	uint16_t xPosMax = dfRANGE_MOVE_RIGHT - dfPACKET_MOVE_FRAME_UNIT_X;
	uint16_t yPosMin = dfRANGE_MOVE_TOP + dfPACKET_MOVE_FRAME_UNIT_Y;
	uint16_t yPosMax = dfRANGE_MOVE_BOTTOM - dfPACKET_MOVE_FRAME_UNIT_Y;

	uint16_t xPos = xPosMin + rand() % (xPosMax - xPosMin);
	uint16_t yPos = yPosMin + rand() % (yPosMax - yPosMin);

	uint8_t dir = rand() % 2 == 0 ? dfPACKET_MOVE_DIR_LL : dfPACKET_MOVE_DIR_RR;
	// 세션 생성
	session->socket = sockFd;
	session->sessionId = acceptNum;
	session->state = dfCHAR_STATE_IDEL;
	session->direction = dir;
	session->xPos = xPos;
	session->yPos = yPos;
	session->hp = dfHP;
	sessionMap.insert(make_pair(session->sessionId, session));
	// 캐릭터 할당 메시지 생성
	makeMsgSCCreateCharacter(scCreate, session->sessionId, dir, session->xPos, session->yPos, session->hp);
	// 송신 링버퍼에 저장
	wcout << L"# PACKET_CONNECT #: SessionID: " << session->sessionId << endl;
	sendUnicast(dfPACKET_SC_CREATE_MY_CHARACTER, sizeof(scCreate), (char*)&scCreate, session);
	wcout << L"Create Character # SessionID: " << session->sessionId << "	X: " << xPos << "	Y: " << yPos << endl;
}

/// 서버 접속시 이미 접속되어 있는 다른 캐릭터의 정보 가져오기
void getOtherSession(uint16_t sessionId)
{
	// 다른 접속 캐릭터 가져오기
	for (auto it = sessionMap.begin(); it != sessionMap.end(); ++it)
	{
		Session* session = it->second;
		// 본인 제외
		if (sessionId == session->sessionId)
		{
			continue;
		}
		// 방금 만들어진(접속한) 캐릭터 세션
		Session* findSession = sessionMap.find(sessionId)->second;
		if (findSession == nullptr)
		{
			return;
		}
		SCCreateOtherCharacter scOther;
		// 메시지 생성
		makeMsgSCCreateOtherCharacter(scOther, session->sessionId, session->direction, session->xPos, session->yPos, session->hp);
		// 송신 링버퍼에 저장
		sendUnicast(dfPACKET_SC_CREATE_OTHER_CHARACTER, sizeof(scOther), (char*)&scOther, findSession);
	}
}

/// 이미 접속되어 있는 캐릭터들에게 방금 생성된 세션 정보 전송
void createOtherSession(uint16_t sessionId)
{
	SCCreateOtherCharacter scOther;
	// 방금 만들어진(접속한) 캐릭터 세션
	Session* findSession = sessionMap.find(sessionId)->second;
	if (findSession == nullptr)
	{
		return;
	}
	// 메시지 생성
	makeMsgSCCreateOtherCharacter(scOther, findSession->sessionId, findSession->direction, findSession->xPos, findSession->yPos, findSession->hp);
	// 전달 세션 제외 모든 유저에게 패킷 전달
	sendBroadCast(dfPACKET_SC_CREATE_OTHER_CHARACTER, sizeof(scOther), (char*)&scOther, findSession);
}

/// 캐릭터 이동 시작 처리 로직
/// 현재는 클라에서 먼저 이동, 서버에서 후 반영
void moveStartPacket(Session* session, const char* buffer)
{
	CSMoveStart* csMoveStart = (CSMoveStart*)buffer;
	// 이동 범위를 벗어났을 경우 연결 끊기
	if (abs(session->xPos - csMoveStart->X) > dfERROR_RANGE || abs(session->yPos - csMoveStart->Y) > dfERROR_RANGE)
	{
		wcout << L"dfERROR_RANGE! moveStart - disconnect Session: " << session->sessionId << endl;
		disconnectSession(session);
		return;
	}
	session->state = dfCHAR_STATE_MOVE_START;
	session->action = csMoveStart->Direction;
	// 방향 변경
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
	// 좌표설정
	session->xPos = csMoveStart->X;
	session->yPos = csMoveStart->Y;
	// 현재 접속중인 세션들에게 전달받은 플레이어가 움직인다는 것을 전부 뿌림(전달받은 session 당사자 제외)
	SCMoveStart scMoveStart;
	makeMsgSCMoveStart(scMoveStart, session->sessionId, csMoveStart->Direction, session->xPos, session->yPos);
	wcout << L"# PACKET_MOVE_START #: SessionID: " << session->sessionId <<
		L" / Direction: " << session->direction << L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_MOVE_START, sizeof(scMoveStart), (char*)&scMoveStart, session);
}

/// 캐릭터 이동 중지 처리 로직
/// 현재는 클라에서 먼저 처리, 서버에서 후 반영
void moveStopPacket(Session* session, const char* buffer)
{
	CSMoveStop* csMoveStop = (CSMoveStop*)buffer;
	// 이동 범위를 벗어났을 경우 연결 끊기
	if (abs(session->xPos - csMoveStop->X) > dfERROR_RANGE || abs(session->yPos - csMoveStop->Y) > dfERROR_RANGE)
	{
		wcout << L"dfERROR_RANGE! moveStop - disconnect Session: " << session->sessionId << endl;
		disconnectSession(session);
		return;
	}
	session->state = dfCHAR_STATE_MOVE_STOP;
	session->action = csMoveStop->Direction;
	// 방향 변경
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
	// 좌표설정
	session->xPos = csMoveStop->X;
	session->yPos = csMoveStop->Y;
	// 현재 접속중인 세션들에게 전달받은 플레이어가 움직임을 중지한 것을 전부 뿌림(전달받은 session 당사자 제외)
	SCMoveStop scMoveStop;
	makeMsgSCMoveStop(scMoveStop, session->sessionId, csMoveStop->Direction, session->xPos, session->yPos);
	wcout << L"# PACKET_MOVE_STOP #: SessionID: " << session->sessionId <<
		L" / Direction: " << session->direction << L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_MOVE_STOP, sizeof(scMoveStop), (char*)&scMoveStop, session);
}

/// 캐릭터 공격01 처리 로직
void attack01Packet(Session* session, const char* buffer)
{
	CSAttack01* attack = (CSAttack01*)buffer;

	session->state = dfCHAR_STATE_ATTACK;
	session->action = dfPACKET_CS_ATTACK1;
	session->direction = attack->Direction;
	// 좌표설정
	session->xPos = attack->X;
	session->yPos = attack->Y;
	// 현재 접속중인 세션들에게 전달받은 플레이어가 공격한다는것을 전부 뿌림(전달받은 session 당사자 제외)
	SCAttack01 scAttack01;
	makeMsgSCAttack01(scAttack01, session->sessionId, session->direction, session->xPos, session->yPos);
	wcout << L"# PACKET_ATTACK1 # SessionID: " << session->sessionId << L" / Direction: " << session->direction
		<< L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_ATTACK1, sizeof(scAttack01), (char*)&scAttack01, session);
}

/// 캐릭터 공격02 처리 로직
void attack02Packet(Session* session, const char* buffer)
{
	CSAttack02* attack = (CSAttack02*)buffer;

	session->state = dfCHAR_STATE_ATTACK;
	session->action = dfPACKET_CS_ATTACK2;
	session->direction = attack->Direction;
	// 좌표설정
	session->xPos = attack->X;
	session->yPos = attack->Y;
	// 현재 접속중인 세션들에게 전달받은 플레이어가 공격한다는것을 전부 뿌림(전달받은 session 당사자 제외)
	SCAttack02 scAttack02;
	makeMsgSCAttack02(scAttack02, session->sessionId, session->direction, session->xPos, session->yPos);
	wcout << L"# PACKET_ATTACK2 # SessionID: " << session->sessionId << L" / Direction: " << session->direction
		<< L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_ATTACK2, sizeof(scAttack02), (char*)&scAttack02, session);
}

/// 캐릭터 공격03 처리 로직
void attack03Packet(Session* session, const char* buffer)
{
	CSAttack03* attack = (CSAttack03*)buffer;

	session->state = dfCHAR_STATE_ATTACK;
	session->action = dfPACKET_CS_ATTACK3;
	session->direction = attack->Direction;
	// 좌표설정
	session->xPos = attack->X;
	session->yPos = attack->Y;
	// 현재 접속중인 세션들에게 전달받은 플레이어가 공격한다는것을 전부 뿌림(전달받은 session 당사자 제외)
	SCAttack03 scAttack03;
	makeMsgSCAttack03(scAttack03, session->sessionId, session->direction, session->xPos, session->yPos);
	wcout << L"# PACKET_ATTACK3 # SessionID: " << session->sessionId << L" / Direction: " << session->direction
		<< L" / X: " << session->xPos << L" / Y: " << session->yPos << endl;
	sendBroadCast(dfPACKET_SC_ATTACK3, sizeof(scAttack03), (char*)&scAttack03, session);
}

/// 데미지 처리
void damagePacket(Session* session, Session* otherSession)
{
	SCDamage damage;
	makeMsgSCDamage(damage, session->sessionId, otherSession->sessionId, otherSession->hp);
	sendBroadCast(dfPACKET_SC_DAMAGE, sizeof(damage), (char*)&damage);
}

/// 전달받은 msg 링버퍼에 저장
void sendUnicast(uint8_t byType, int32_t size, const char* msg, Session* pSession)
{
	// 헤더 세팅
	MsgHeader header;
	header.byCode = PACKET_CODE;
	header.bySize = size;
	header.byType = byType;

	pSession->sendQ.enqueue((char*)&header, sizeof(header));
	pSession->sendQ.enqueue(msg, size);
}

/// 세션 전체에게 msg 전달
/// session이 nullptr이 아닐시 해당 세션 제외후 전체 전달
void sendBroadCast(uint8_t byType, int32_t size, const char* msg, const Session* pSession)
{
	for (auto it = sessionMap.begin(); it != sessionMap.end(); ++it)
	{
		Session* session = it->second;
		// 제외
		if (pSession != nullptr && (pSession->sessionId == session->sessionId))
		{
			continue;
		}
		// 전달받은 msg 링버퍼에 저장
		sendUnicast(byType, size, msg, session);
	}
}

/// 세션 연결 해제
void disconnectSession(Session* session, unordered_map<uint16_t, Session*>::iterator* it)
{
	closesocket(session->socket);
	deleteSession(session, it);
}
