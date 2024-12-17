#pragma once
#include <cstdint>

//---------------------------------------------------------------
// 서버정보
//---------------------------------------------------------------
#define CONTENTS_FPS						50
#define SERVER_PORT							5555
#define IP_SIZE								16

//---------------------------------------------------------------
// 캐릭터 정보
//---------------------------------------------------------------
#define dfHP								100;

//---------------------------------------------------------------
// 캐릭터 상태
//---------------------------------------------------------------
#define dfCHAR_STATE_IDEL					100
#define dfCHAR_STATE_MOVE_START				101
#define dfCHAR_STATE_MOVE_STOP				102
#define dfCHAR_STATE_ATTACK					103

//---------------------------------------------------------------
// 캐릭터 이동 반경
// 
// 위 좌표에 가지 못하게 해야하며
// 해당 좌표에 닿는 경우 움직임을 멈추어야 한다.
//---------------------------------------------------------------
#define dfRANGE_MOVE_TOP					50
#define dfRANGE_MOVE_LEFT					10
#define dfRANGE_MOVE_RIGHT					630
#define dfRANGE_MOVE_BOTTOM					470

//---------------------------------------------------------------
// 프레임당 이동 단위
// 클라이언트: 50FPS, 한 프레임당 X:3, Y:2 만큼 이동
//---------------------------------------------------------------
#define dfPACKET_MOVE_FRAME_UNIT_X			3
#define dfPACKET_MOVE_FRAME_UNIT_Y			2

//---------------------------------------------------------------
// 이동 오류 체크 범위
// 클라이언트가 이동중 해당 범위를 넘어섰을경우
// 해당 클라이언트 연결을 끊어버림
//---------------------------------------------------------------
#define dfERROR_RANGE						50

//---------------------------------------------------------------
// 공격범위.
//---------------------------------------------------------------
#define dfATTACK1_RANGE_X					80
#define dfATTACK2_RANGE_X					90
#define dfATTACK3_RANGE_X					100
#define dfATTACK1_RANGE_Y					10
#define dfATTACK2_RANGE_Y					10
#define dfATTACK3_RANGE_Y					20

//---------------------------------------------------------------
// 공격 데미지
//---------------------------------------------------------------
#define dfATTACK1_DAMAGE					12
#define dfATTACK2_DAMAGE					16
#define dfATTACK3_DAMAGE					20

//---------------------------------------------------------------
#define PACKET_CODE							0x89

/// 메시지 프로토콜 구조체
#pragma pack(push, 1)
//---------------------------------------------------------------
// 헤더
//---------------------------------------------------------------
struct MsgHeader
{
	uint8_t	byCode;
	uint8_t bySize;
	uint8_t	byType;
};
//---------------------------------------------------------------
// 캐릭터 할당
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
// 접속 이후 다른 클라의 캐릭터 정보
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
// 캐릭터 이동 시작
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
// 캐릭터 이동 중지
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
// 캐릭터 공격
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
// 캐릭터 데미지 패킷
//---------------------------------------------------------------
struct SCDamage
{
	uint32_t AttackID;
	uint32_t DamageID;
	int8_t DamageHP;
};

//---------------------------------------------------------------
// 캐릭터 삭제
//---------------------------------------------------------------
struct SCDeleteCharacter
{
	uint32_t ID;
};

#pragma pack(pop)
//---------------------------------------------------------------
/// 메시지 프로토콜 생성 함수

//---------------------------------------------------------------
// 캐릭터 할당
//---------------------------------------------------------------
void makeMsgSCCreateCharacter(SCCreateMyCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp);

//---------------------------------------------------------------
// 서버 접속시 이미 접속되어 있는 다른 캐릭터의 정보
//---------------------------------------------------------------
void makeMsgSCCreateOtherCharacter(SCCreateOtherCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp);

//---------------------------------------------------------------
// 캐릭터 이동시작
//---------------------------------------------------------------
void makeMsgSCMoveStart(SCMoveStart& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);

//---------------------------------------------------------------
// 캐릭터 이동중지
//---------------------------------------------------------------
void makeMsgSCMoveStop(SCMoveStop& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);

//---------------------------------------------------------------
// 캐릭터 공격
//---------------------------------------------------------------
void makeMsgSCAttack01(SCAttack01& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);
void makeMsgSCAttack02(SCAttack02& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);
void makeMsgSCAttack03(SCAttack03& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos);

//---------------------------------------------------------------
// 데미지
//---------------------------------------------------------------
void makeMsgSCDamage(SCDamage& msg, uint32_t attackId, uint32_t damageId, int8_t hp);

//---------------------------------------------------------------
// 유저 삭제
//---------------------------------------------------------------
void makeMsgSCDeleteCharacter(SCDeleteCharacter& msg, uint32_t id);
