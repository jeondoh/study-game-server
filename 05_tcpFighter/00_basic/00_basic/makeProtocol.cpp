#include "packetDefine.h"
#include "makeProtocol.h"

/// 캐릭터 할당 메시지 생성
void makeMsgSCCreateCharacter(SCCreateMyCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp)
{
	// 메시지 세팅
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
	msg.HP = hp;
}

/// 서버 접속시 이미 접속되어 있는 다른 캐릭터의 정보
void makeMsgSCCreateOtherCharacter(SCCreateOtherCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp)
{
	// 메시지 세팅
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
	msg.HP = hp;
}

/// 캐릭터 이동시작
void makeMsgSCMoveStart(SCMoveStart& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// 메시지 세팅
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// 캐릭터 이동중지
void makeMsgSCMoveStop(SCMoveStop& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// 메시지 세팅
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// 캐릭터 공격01
void makeMsgSCAttack01(SCAttack01& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// 메시지 세팅
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// 캐릭터 공격02
void makeMsgSCAttack02(SCAttack02& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// 메시지 세팅
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// 캐릭터 공격03
void makeMsgSCAttack03(SCAttack03& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// 메시지 세팅
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// 데미지
void makeMsgSCDamage(SCDamage& msg, uint32_t attackId, uint32_t damageId, int8_t hp)
{
	// 메시지 세팅
	msg.AttackID = attackId;
	msg.DamageID = damageId;
	msg.DamageHP = hp;
}

/// 유저 삭제
void makeMsgSCDeleteCharacter(SCDeleteCharacter& msg, uint32_t id)
{
	// 메시지 세팅
	msg.ID = id;
}
