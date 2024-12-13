#pragma once
#include "player.h"

/// 서버소켓(non-blocking)을 listen 상태로 설정
void openServer();
/// 클라 수용 및 세그먼트 보내기 / 받기
void acceptClient();
/// 클라 접속 수용
void acceptProc();
/// ID 할당 send()
void starIdAssignSend(Player* player);
/// 별 생성 send()
void starCreateSend(const Player* player);
/// 별 생성 send()
void starCreateUnicastSend(Player* player, const Player* otherPlayer);
/// 한 유저에게 보내기 위함, 링버퍼에 send()할 내용 저장
void sendUnicast(const char* msg, Player* player);
/// 전체 유저에게 send()
void sendBroadcast(const char* msg, const Player* player = nullptr);
/// 별 삭제 send()
void starDeleteSend(Player* player);
/// 메시지 받기
void recvProc(Player* player);
/// 메시지 보내기
void sendProc(Player* player);
/// 별 이동 send()
void starMoveSend(Player* player, char* buffer);
/// 죽은 player 연결 끊기
void disConnect();
/// 소켓 종료
void closeSocket();
