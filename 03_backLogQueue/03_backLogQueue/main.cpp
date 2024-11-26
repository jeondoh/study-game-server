#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32")

#define SERVERPORT	9999
#define BUFSIZE		512
#define IPSIZE		32

using namespace std;

int wmain()
{
	wcout.imbue(locale("kor"));
	::wcout << L"서버시작" << endl;
	int retval;
	WSADATA wsaData;
	WSADATA wsa;

	// winsock 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}
	// socket(): 프로토콜 결정
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
	{
		int errCode = WSAGetLastError();
		::wcout << L"socket() 에러번호: " << errCode << endl;
		return 1;
	}
	// bind(): 지역 IP주소, 지역 포트번호 결정
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listenSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		::wcout << L"bind() 에러번호: " << errCode << endl;
		return 1;
	}

	// listen(): TCP를 LISTENING 상태로 변경, 클라가 연결 가능한 상태
	/*
		SOMMAXCONN으로 listen시 200개 연결 허용(WSAECONNREFUSED - 10061)
		연결이 거부되었습니다.
		대상 컴퓨터가 적극적으로 거부했기 때문에 연결할 수 없습니다.
		이는 일반적으로 서버 애플리케이션이 실행되지 않는 외부 호스트에서 비활성 상태인 서비스에 연결하려고 시도하기 때문에 발생합니다.

		SOMAXCONN_HINT(SOMAXCONN)으로 listen시 16,353개 연결 허용(WSAENOBUFS - 10055)
		사용할 수 있는 버퍼 공간이 없습니다.
		시스템에 충분한 버퍼 공간이 부족하거나 큐가 가득 차서 소켓에서 작업을 수행할 수 없습니다.

		포트번호 49,152 ~ 65,535
	*/

	//retval = listen(listenSock, SOMAXCONN);
	retval = listen(listenSock, SOMAXCONN_HINT(SOMAXCONN));

	while (1)
	{
		if (retval == SOCKET_ERROR)
		{
			retval = WSAGetLastError();
			::wcout << L"listen() 에러번호: " << retval << endl;
			return 1;
		}
	}

	closesocket(listenSock);
	WSACleanup();
}
