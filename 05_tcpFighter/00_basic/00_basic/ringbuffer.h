#pragma once

#define DEFAULT_SIZE	4096

/// ��Ʈ��ũ ��-���ſ� ����� ������
class RingBuffer
{
public:
	/// ������
	RingBuffer();
	RingBuffer(int size);

	/// �Ҹ���
	~RingBuffer();

	/// ���� ũ�� ��ȯ
	int getBufferSize();

	/// ���� ������� �뷮 ���
	int getUseSize();

	/// ���� ���ۿ� ���� �뷮 ���
	int getFreeSize();

	/// ������ �ֱ�
	/// ���� ũ�⸸ŭ ����
	int enqueue(const char* buffer, int size);

	/// ������ ��������
	/// ������ ũ�⸸ŭ ����
	int dequeue(char* buffer, int size);

	/// ������ �о����
	/// ������ ũ�⸸ŭ ����
	int peek(char* buffer, int size);

	/// ������ �ʰ� �ִ�� ���� �� �ִ� ����
	int directEnqueueSize();

	/// ������ �ʰ� �ִ�� ������ �� �ִ� ����
	int directDequeueSize();

	/// ������ ���̸�ŭ rear ��ġ �̵�
	/// �̵��� ũ�� ����
	int moveRear(int size);

	/// ������ ���̸�ŭ front ��ġ �̵�
	/// �̵��� ũ�� ����
	int moveFront(int size);

	/// rear�� �ش��ϴ� ������ ����
	char* getRearBufferPtr();

	/// front�� �ش��ϴ� ������ ����
	char* getFrontBufferPtr();

	/// ���ۿ� �ִ� ��� ������ ����
	void clearBuffer();

private:
	char* _buffer;
	int _bufferSize;
	int _front;
	int _rear;
};
