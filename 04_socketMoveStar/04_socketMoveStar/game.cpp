#include "game.h"
#include "network.h"
#include "console.h"

/// 서버 listen 상태로
void startServer()
{
	openServer();
}
/// 클라와 소켓통신
void network()
{
	// 클라 수용 및 세그먼트 보내기 / 받기
	acceptClient();
	// 죽은 플레이어 연결 끊기
	disConnect();
}

/// 랜더링
void render()
{
	bufferClear();
	// 플레이어 랜더링
	renderPlayer();
	bufferFlip();
}

/// 게임 종료
void closeGame()
{
	// 플레이어 삭제
	deleteAllPlayer();
	// 소켓 종료
	closeSocket();
}

