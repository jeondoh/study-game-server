#pragma once

#include <cstdint>
#include "contents.h"

extern uint32_t acceptNum;
extern bool isShutdown;

/// 서버 초기화 및 listen 상태
void openServer();
/// 네트워크 송수신 처리
void netIOProcess();
/// 클라이언트 접속 accept, 세션생성
void acceptProc();
/// 메시지 헤더타입별로 분기처리
void packetProc(Session* session, const char* buffer, uint8_t byType);
/// 메시지 recv
void recvProc(Session* session);
/// 메시지 send
void sendProc(Session* session);

/// 세션(플레이어) 생성
void createSession(const char* ip, uint32_t sockFd, uint16_t port);
/// 서버 접속시 이미 접속되어 있는 다른 캐릭터의 정보 가져오기
void getOtherSession(uint16_t sessionId);
/// 이미 접속되어 있는 캐릭터들에게 방금 생성된 세션 정보 전송
void createOtherSession(uint16_t sessionId);
/// 캐릭터 이동 시작 처리 로직
/// 현재는 클라에서 먼저 이동, 서버에서 후 반영
void moveStartPacket(Session* session, const char* buffer);
/// 캐릭터 이동 중지 처리 로직
/// 현재는 클라에서 먼저 처리, 서버에서 후 반영
void moveStopPacket(Session* session, const char* buffer);
/// 캐릭터 공격01 처리 로직
void attack01Packet(Session* session, const char* buffer);
/// 캐릭터 공격02 처리 로직
void attack02Packet(Session* session, const char* buffer);
/// 캐릭터 공격03 처리 로직
void attack03Packet(Session* session, const char* buffer);
/// 데미지 처리
void damagePacket(Session* session, Session* otherSession);
/// 전달받은 msg 링버퍼에 저장
void sendUnicast(uint8_t byType, int32_t size, const char* msg, Session* pSession);
/// 세션 전체에게 msg 전달
/// session이 nullptr이 아닐시 해당 세션 제외후 전체 전달
void sendBroadCast(uint8_t byType, int32_t size, const char* msg, const Session* pSession = nullptr);
/// 세션 연결 해제
void disconnectSession(Session* session, unordered_map<uint16_t, Session*>::iterator* it = nullptr);
