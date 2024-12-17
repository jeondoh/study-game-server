#include "packetDefine.h"
#include "makeProtocol.h"

/// ĳ���� �Ҵ� �޽��� ����
void makeMsgSCCreateCharacter(SCCreateMyCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp)
{
	// �޽��� ����
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
	msg.HP = hp;
}

/// ���� ���ӽ� �̹� ���ӵǾ� �ִ� �ٸ� ĳ������ ����
void makeMsgSCCreateOtherCharacter(SCCreateOtherCharacter& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos, int8_t hp)
{
	// �޽��� ����
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
	msg.HP = hp;
}

/// ĳ���� �̵�����
void makeMsgSCMoveStart(SCMoveStart& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// �޽��� ����
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// ĳ���� �̵�����
void makeMsgSCMoveStop(SCMoveStop& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// �޽��� ����
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// ĳ���� ����01
void makeMsgSCAttack01(SCAttack01& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// �޽��� ����
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// ĳ���� ����02
void makeMsgSCAttack02(SCAttack02& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// �޽��� ����
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// ĳ���� ����03
void makeMsgSCAttack03(SCAttack03& msg, uint32_t id, uint8_t direction, int16_t xPos, int16_t yPos)
{
	// �޽��� ����
	msg.ID = id;
	msg.Direction = direction;
	msg.X = xPos;
	msg.Y = yPos;
}

/// ������
void makeMsgSCDamage(SCDamage& msg, uint32_t attackId, uint32_t damageId, int8_t hp)
{
	// �޽��� ����
	msg.AttackID = attackId;
	msg.DamageID = damageId;
	msg.DamageHP = hp;
}

/// ���� ����
void makeMsgSCDeleteCharacter(SCDeleteCharacter& msg, uint32_t id)
{
	// �޽��� ����
	msg.ID = id;
}
