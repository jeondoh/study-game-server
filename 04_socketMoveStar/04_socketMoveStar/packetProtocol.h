#pragma once

#define SERVER_PORT		3000
#define BUFFER_SIZE		512
#define PACKET_SIZE		16
#define IP_SIZE			16

/// 패킷 프로토콜별 구조체
/// 구조체 멤버변수는 다 같지만 용도별로 분리해서 관리

enum PACKET_TYPE
{
	TYPE_ID,
	TYPE_CREATE,
	TYPE_DELETE,
	TYPE_MOVE
};
#pragma pack(1)
// ID 할당
struct PPAssignStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;

};
// 별 생성
struct PPCreateStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;
};
// 별 삭제
struct PPDeleteStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;
};
// 별 이동
struct PPMoveStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;
};
#pragma pack()
