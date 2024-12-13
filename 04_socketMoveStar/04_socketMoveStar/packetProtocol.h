#pragma once

#define SERVER_PORT		3000
#define BUFFER_SIZE		512
#define PACKET_SIZE		16
#define IP_SIZE			16

/// ��Ŷ �������ݺ� ����ü
/// ����ü ��������� �� ������ �뵵���� �и��ؼ� ����

enum PACKET_TYPE
{
	TYPE_ID,
	TYPE_CREATE,
	TYPE_DELETE,
	TYPE_MOVE
};
#pragma pack(1)
// ID �Ҵ�
struct PPAssignStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;

};
// �� ����
struct PPCreateStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;
};
// �� ����
struct PPDeleteStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;
};
// �� �̵�
struct PPMoveStar
{
	PACKET_TYPE type;
	__int32 id;
	__int32 x;
	__int32 y;
};
#pragma pack()
