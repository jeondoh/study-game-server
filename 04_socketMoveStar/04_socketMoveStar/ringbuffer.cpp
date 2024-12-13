#include <iostream>
#include "windows.h"
#include "ringBuffer.h"

/// ������
RingBuffer::RingBuffer() : _bufferSize(DEFAULT_SIZE), _front(0), _rear(0)
{
	_buffer = new char[_bufferSize];
}

RingBuffer::RingBuffer(int size) : _bufferSize(size), _front(0), _rear(0)
{
	_buffer = new char[_bufferSize];
}

/// �Ҹ���
RingBuffer::~RingBuffer()
{
	if (_buffer)
	{
		delete[] _buffer;
	}
}

/// ���� ũ�� ��ȯ
int RingBuffer::getBufferSize()
{
	return _bufferSize - 1;
}

/// ���� ������� �뷮 ���
int RingBuffer::getUseSize()
{
	if (_rear >= _front)
	{
		return _rear - _front;
	}
	else
	{
		return _bufferSize - _front + _rear;
	}
}

/// ���� ���ۿ� ���� �뷮 ���
int RingBuffer::getFreeSize()
{
	return _bufferSize - getUseSize() - 1;
}

/// ������ �ֱ�
/// ���� ũ�⸸ŭ ����
int RingBuffer::enqueue(const char* buffer, int size)
{
	// ����ŷ ���Ͽ��� -1�� �� ��츦 ���
	if (size <= 0)
	{
		return 0;
	}
	int freeSize = getFreeSize();
	// ���� á�� ��� ���̻� ������ �����Ƿ� return 0
	if (freeSize == 0)
	{
		return 0;
	}
	// ����ũ�⺸�� �����Ϸ��� ���� Ŭ��
	else if (size > freeSize)
	{
		return 0;
	}
	// �� ���ǿ��� ���� �� �ִ� ������ üũ�� ��
	// �����ۿ� �����͸� �߶� �־�� �� ���
	// ��, �յڷ� ���� ������ ������ �߶� ä��°�
	// �ѹ��� ������ �ִ� ������
	int firstEnqueSize = directEnqueueSize();
	if (size > firstEnqueSize)
	{
		// ������ 2�� �ִ´�
		// 1��° ����
		memcpy_s((_buffer + _rear), firstEnqueSize, buffer, firstEnqueSize);
		// _rear �̵�
		moveRear(firstEnqueSize);
		// 2��° ����
		memcpy_s(_buffer, size - firstEnqueSize, (buffer + firstEnqueSize), (size - firstEnqueSize));
		// _rear �̵�
		moveRear(size - firstEnqueSize);
	}
	else
	{
		// _buffer�� ������ �ֱ�
		memcpy_s((_buffer + _rear), (_bufferSize - _rear), buffer, size);
		// _rear �̵�
		moveRear(size);
	}
	return size;
}

/// ������ ��������
/// ������ ũ�⸸ŭ ����
int RingBuffer::dequeue(char* buffer, int size)
{
	// ����ŷ ���Ͽ��� -1�� �� ��츦 ���
	if (size <= 0)
	{
		return 0;
	}
	int useSize = getUseSize();
	// ������� ũ�⸸ŭ�� ��ȯ�ϱ� ����
	if (useSize == 0)
	{
		return 0;
	}
	// dequeue �� �� �ִ� �ִ� �뷮��ŭ ����
	else if (size > useSize)
	{
		size = useSize;
	}
	// �� ���ǿ��� �� �� �ִ� ������ üũ�� ��
	// �����ۿ��� �����͸� ������ ������� �� ���
	// ��, �յڷ� ��� ������ ������ �ι��� ��ó ������ ���ļ� ����
	// �ѹ��� �� �� �ִ� ������
	int firstDequeSize = directDequeueSize();
	if (size > firstDequeSize)
	{
		// ������ 2�� ����.
		// 1��° ����
		memcpy_s(buffer, firstDequeSize, (_buffer + _front), firstDequeSize);
		// _front �̵�
		moveFront(firstDequeSize);
		// 2��° ����
		memcpy_s((buffer + firstDequeSize), (size - firstDequeSize), _buffer, (size - firstDequeSize));
		// _front �̵�
		moveFront(size - firstDequeSize);
	}
	else
	{
		memcpy_s(buffer, size, (_buffer + _front), size);
		// _front �̵�
		moveFront(size);
	}
	return size;
}

/// ������ �о����
/// ������ ũ�⸸ŭ ����
int RingBuffer::peek(char* buffer, int size)
{
	// ����ŷ ���Ͽ��� -1�� �� ��츦 ���
	if (size <= 0)
	{
		return 0;
	}
	int useSize = getUseSize();
	// ������� ũ�⸸ŭ�� ��ȯ�ϱ� ����
	if (useSize == 0)
	{
		return 0;
	}
	// dequeue �� �� �ִ� �ִ� �뷮��ŭ ����
	else if (size > useSize)
	{
		size = useSize;
	}
	// �� ���ǿ��� �� �� �ִ� ������ üũ�� ��
	// �����ۿ��� �����͸� ������ ������� �� ���
	// ��, �յڷ� ��� ������ ������ �ι��� ��ó ������ ���ļ� ����
	// �ѹ��� �� �� �ִ� ������
	int firstDequeSize = directDequeueSize();
	if (size > firstDequeSize)
	{
		// ������ 2�� ����.
		// 1��° ����
		memcpy_s(buffer, firstDequeSize, (_buffer + _front), firstDequeSize);
		// 2��° ����
		memcpy_s((buffer + firstDequeSize), (size - firstDequeSize), _buffer, (size - firstDequeSize));
	}
	else
	{
		memcpy_s(buffer, size, (_buffer + _front), size);
	}
	return size;
}

/// ������ �ʰ� �ִ�� ���� �� �ִ� ����
int RingBuffer::directEnqueueSize()
{
	if (_front > _rear)
	{
		return _front - _rear;
	}
	return _bufferSize - _rear;
}

/// ������ �ʰ� �ִ�� ������ �� �ִ� ����
int RingBuffer::directDequeueSize()
{
	if (_rear >= _front)
	{
		return _rear - _front;
	}
	return _bufferSize - _front;
}

/// ������ ���̸�ŭ rear ��ġ �̵�
/// �̵��� ũ�� ����
int RingBuffer::moveRear(int size)
{
	_rear = (_rear + size) % _bufferSize;
	return size;
}

/// ������ ���̸�ŭ front ��ġ �̵�
/// �̵��� ũ�� ����
int RingBuffer::moveFront(int size)
{
	_front = (_front + size) % _bufferSize;
	return size;
}

/// rear�� �ش��ϴ� ������ ����
char* RingBuffer::getRearBufferPtr()
{
	return _buffer + _rear;
}

/// front�� �ش��ϴ� ������ ����
char* RingBuffer::getFrontBufferPtr()
{
	return _buffer + _front;
}


void RingBuffer::clearBuffer()
{
	_bufferSize = 0;
	_front = 0;
	_rear = 0;
}
