#pragma once
#include <cstdint>

//---------------------------------------------------------------
// ��������
//---------------------------------------------------------------
#define CONTENTS_FPS						50
#define SERVER_PORT							5555
#define IP_SIZE								16

//---------------------------------------------------------------
// ĳ���� ����
//---------------------------------------------------------------
#define dfHP								100;

//---------------------------------------------------------------
// ĳ���� ����
//---------------------------------------------------------------
#define dfCHAR_STATE_IDEL					100
#define dfCHAR_STATE_MOVE_START				101
#define dfCHAR_STATE_MOVE_STOP				102
#define dfCHAR_STATE_ATTACK					103

//---------------------------------------------------------------
// ĳ���� �̵� �ݰ�
// 
// �� ��ǥ�� ���� ���ϰ� �ؾ��ϸ�
// �ش� ��ǥ�� ��� ��� �������� ���߾�� �Ѵ�.
//---------------------------------------------------------------
#define dfRANGE_MOVE_TOP					50
#define dfRANGE_MOVE_LEFT					10
#define dfRANGE_MOVE_RIGHT					630
#define dfRANGE_MOVE_BOTTOM					470

//---------------------------------------------------------------
// �����Ӵ� �̵� ����
// Ŭ���̾�Ʈ: 50FPS, �� �����Ӵ� X:3, Y:2 ��ŭ �̵�
//---------------------------------------------------------------
#define dfPACKET_MOVE_FRAME_UNIT_X			3
#define dfPACKET_MOVE_FRAME_UNIT_Y			2

//---------------------------------------------------------------
// �̵� ���� üũ ����
// Ŭ���̾�Ʈ�� �̵��� �ش� ������ �Ѿ�����
// �ش� Ŭ���̾�Ʈ ������ �������
//---------------------------------------------------------------
#define dfERROR_RANGE						50

//---------------------------------------------------------------
// ���ݹ���.
//---------------------------------------------------------------
#define dfATTACK1_RANGE_X					80
#define dfATTACK2_RANGE_X					90
#define dfATTACK3_RANGE_X					100
#define dfATTACK1_RANGE_Y					10
#define dfATTACK2_RANGE_Y					10
#define dfATTACK3_RANGE_Y					20

//---------------------------------------------------------------
// ���� ������
//---------------------------------------------------------------
#define dfATTACK1_DAMAGE					12
#define dfATTACK2_DAMAGE					16
#define dfATTACK3_DAMAGE					20

//---------------------------------------------------------------
#define PACKET_CODE							0x89

/// �޽��� �������� ����ü
#pragma pack(push, 1)
//---------------------------------------------------------------
// ���
//---------------------------------------------------------------
struct MsgHeader
{
	uint8_t	byCode;
	uint8_t bySize;
	uint8_t	byType;
};
//---------------------------------------------------------------
// ĳ���� �Ҵ�
//---------------------------------------------------------------
struct SCCreateMyCharacter
{
	uint32_t ID;
	uint8_t Direction;
	int16_t X;
	int16_t Y;
	int8_t HP;
};
//---------------------------------------------------------------
// ���� ���� �ٸ� Ŭ���� ĳ���� ����
//---------------------------------------------------------------
struct SCCreateOtherCharacter
{
	uint32_t ID;
	uint8_t Direction;
	int16_t X;
	int16_t Y;
	int8_t HP;
};
//---------------------------------------------------------------
// ĳ���� �̵� ����
//---------------------------------------------------------------
struct CSMoveStart
{
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

struct SCMoveStart
{
	uint32_t ID;
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

//---------------------------------------------------------------
// ĳ���� �̵� ����
//---------------------------------------------------------------
struct CSMoveStop
{
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

struct SCMoveStop
{
	uint32_t ID;
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

//---------------------------------------------------------------
// ĳ���� ����
//---------------------------------------------------------------
struct CSAttack01
{
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

struct SCAttack01
{
	uint32_t ID;
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

struct CSAttack02
{
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

struct SCAttack02
{
	uint32_t ID;
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

struct CSAttack03
{
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

struct SCAttack03
{
	uint32_t ID;
	uint8_t Direction;
	int16_t X;
	int16_t Y;
};

//---------------------------------------------------------------
// ĳ���� ������ ��Ŷ
//---------------------------------------------------------------
struct SCDamage
{
	uint32_t AttackID;
	uint32_t DamageID;
	int8_t DamageHP;
};

//---------------------------------------------------------------
// ĳ���� ����
//---------------------------------------------------------------
struct SCDeleteCharacter
{
	uint32_t ID;
};

#pragma pack(pop)
//---------------------------------------------------------------
/// �޽��� �������� ���� �Լ�

//---------------------------------------------------------------
// ĳ���� �Ҵ�
//---------------------------------------------------------------
void makeMsgSCCreateCharacter(SCCreateMyCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp);

//---------------------------------------------------------------
// ���� ���ӽ� �̹� ���ӵǾ� �ִ� �ٸ� ĳ������ ����
//---------------------------------------------------------------
void makeMsgSCCreateOtherCharacter(SCCreateOtherCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp);

//---------------------------------------------------------------
// ĳ���� �̵�����
//---------------------------------------------------------------
void makeMsgSCMoveStart(SCMoveStart& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);

//---------------------------------------------------------------
// ĳ���� �̵�����
//---------------------------------------------------------------
void makeMsgSCMoveStop(SCMoveStop& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);

//---------------------------------------------------------------
// ĳ���� ����
//---------------------------------------------------------------
void makeMsgSCAttack01(SCAttack01& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);
void makeMsgSCAttack02(SCAttack02& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);
void makeMsgSCAttack03(SCAttack03& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);

//---------------------------------------------------------------
// ������
//---------------------------------------------------------------
void makeMsgSCDamage(SCDamage& msg, uint32_t attackId, uint32_t damageId, int8_t hp);

//---------------------------------------------------------------
// ���� ����
//---------------------------------------------------------------
void makeMsgSCDeleteCharacter(SCDeleteCharacter& msg, uint32_t id);
