#pragma once

#define dfSCREEN_WIDTH		82		// �ܼ� ���� 81ĭ + NULL
#define dfSCREEN_HEIGHT		24		// �ܼ� ���� 24ĭ

/// �ܼ� ���� �غ��۾�
void csInitial();

/// ������ ������ ȭ������ ����ִ� �Լ�
void bufferFlip();

/// ȭ�� ���۸� �����ִ� �Լ�(������ �ܻ� ����)
/// �� ������ �׸��� �׸��� ������ ���۸� ���� �ش�.
void bufferClear();

/// ������ Ư�� ��ġ�� ���ϴ� ���ڿ��� ���.
/// �Է� ���� X,Y ��ǥ�� �ƽ�Ű�ڵ� �ϳ��� ��� (���ۿ� �׸�) 
void spriteStringDraw(int iX, int iY, const wchar_t* string);
