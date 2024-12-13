#include <iostream>
#include "windows.h"
#include "ringBuffer.h"

/// 생성자
RingBuffer::RingBuffer() : _bufferSize(DEFAULT_SIZE), _front(0), _rear(0)
{
	_buffer = new char[_bufferSize];
}

RingBuffer::RingBuffer(int size) : _bufferSize(size), _front(0), _rear(0)
{
	_buffer = new char[_bufferSize];
}

/// 소멸자
RingBuffer::~RingBuffer()
{
	if (_buffer)
	{
		delete[] _buffer;
	}
}

/// 버퍼 크기 반환
int RingBuffer::getBufferSize()
{
	return _bufferSize - 1;
}

/// 현재 사용중인 용량 얻기
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

/// 현재 버퍼에 남은 용량 얻기
int RingBuffer::getFreeSize()
{
	return _bufferSize - getUseSize() - 1;
}

/// 데이터 넣기
/// 넣은 크기만큼 리턴
int RingBuffer::enqueue(const char* buffer, int size)
{
	// 논블로킹 소켓에서 -1이 올 경우를 대비
	if (size <= 0)
	{
		return 0;
	}
	int freeSize = getFreeSize();
	// 가득 찼을 경우 더이상 넣을수 없으므로 return 0
	if (freeSize == 0)
	{
		return 0;
	}
	// 남은크기보다 복사하려는 값이 클때
	else if (size > freeSize)
	{
		return 0;
	}
	// 위 조건에서 넣을 수 있는 공간을 체크한 뒤
	// 링버퍼에 데이터를 잘라서 넣어야 할 경우
	// 즉, 앞뒤로 남는 공간이 있으면 잘라서 채우는것
	// 한번에 넣을수 있는 사이즈
	int firstEnqueSize = directEnqueueSize();
	if (size > firstEnqueSize)
	{
		// 나눠서 2번 넣는다
		// 1번째 삽입
		memcpy_s((_buffer + _rear), firstEnqueSize, buffer, firstEnqueSize);
		// _rear 이동
		moveRear(firstEnqueSize);
		// 2번째 삽입
		memcpy_s(_buffer, size - firstEnqueSize, (buffer + firstEnqueSize), (size - firstEnqueSize));
		// _rear 이동
		moveRear(size - firstEnqueSize);
	}
	else
	{
		// _buffer에 데이터 넣기
		memcpy_s((_buffer + _rear), (_bufferSize - _rear), buffer, size);
		// _rear 이동
		moveRear(size);
	}
	return size;
}

/// 데이터 가져오기
/// 가져온 크기만큼 리턴
int RingBuffer::dequeue(char* buffer, int size)
{
	// 논블로킹 소켓에서 -1이 올 경우를 대비
	if (size <= 0)
	{
		return 0;
	}
	int useSize = getUseSize();
	// 사용중인 크기만큼만 반환하기 위함
	if (useSize == 0)
	{
		return 0;
	}
	// dequeue 할 수 있는 최대 용량만큼 빼줌
	else if (size > useSize)
	{
		size = useSize;
	}
	// 위 조건에서 뺄 수 있는 공간을 체크한 뒤
	// 링버퍼에서 데이터를 나누어 꺼내어야 할 경우
	// 즉, 앞뒤로 사용 공간이 있으면 두번에 거처 빼내어 합쳐서 전달
	// 한번에 뺄 수 있는 사이즈
	int firstDequeSize = directDequeueSize();
	if (size > firstDequeSize)
	{
		// 나눠서 2번 뺀다.
		// 1번째 삽입
		memcpy_s(buffer, firstDequeSize, (_buffer + _front), firstDequeSize);
		// _front 이동
		moveFront(firstDequeSize);
		// 2번째 삽입
		memcpy_s((buffer + firstDequeSize), (size - firstDequeSize), _buffer, (size - firstDequeSize));
		// _front 이동
		moveFront(size - firstDequeSize);
	}
	else
	{
		memcpy_s(buffer, size, (_buffer + _front), size);
		// _front 이동
		moveFront(size);
	}
	return size;
}

/// 데이터 읽어오기
/// 가져온 크기만큼 리턴
int RingBuffer::peek(char* buffer, int size)
{
	// 논블로킹 소켓에서 -1이 올 경우를 대비
	if (size <= 0)
	{
		return 0;
	}
	int useSize = getUseSize();
	// 사용중인 크기만큼만 반환하기 위함
	if (useSize == 0)
	{
		return 0;
	}
	// dequeue 할 수 있는 최대 용량만큼 빼줌
	else if (size > useSize)
	{
		size = useSize;
	}
	// 위 조건에서 뺄 수 있는 공간을 체크한 뒤
	// 링버퍼에서 데이터를 나누어 꺼내어야 할 경우
	// 즉, 앞뒤로 사용 공간이 있으면 두번에 거처 빼내어 합쳐서 전달
	// 한번에 뺄 수 있는 사이즈
	int firstDequeSize = directDequeueSize();
	if (size > firstDequeSize)
	{
		// 나눠서 2번 뺀다.
		// 1번째 삽입
		memcpy_s(buffer, firstDequeSize, (_buffer + _front), firstDequeSize);
		// 2번째 삽입
		memcpy_s((buffer + firstDequeSize), (size - firstDequeSize), _buffer, (size - firstDequeSize));
	}
	else
	{
		memcpy_s(buffer, size, (_buffer + _front), size);
	}
	return size;
}

/// 끊기지 않고 최대로 넣을 수 있는 길이
int RingBuffer::directEnqueueSize()
{
	if (_front > _rear)
	{
		return _front - _rear;
	}
	return _bufferSize - _rear;
}

/// 끊기지 않고 최대로 가져올 수 있는 길이
int RingBuffer::directDequeueSize()
{
	if (_rear >= _front)
	{
		return _rear - _front;
	}
	return _bufferSize - _front;
}

/// 전달한 길이만큼 rear 위치 이동
/// 이동한 크기 리턴
int RingBuffer::moveRear(int size)
{
	_rear = (_rear + size) % _bufferSize;
	return size;
}

/// 전달한 길이만큼 front 위치 이동
/// 이동한 크기 리턴
int RingBuffer::moveFront(int size)
{
	_front = (_front + size) % _bufferSize;
	return size;
}

/// rear에 해당하는 포인터 리턴
char* RingBuffer::getRearBufferPtr()
{
	return _buffer + _rear;
}

/// front에 해당하는 포인터 리턴
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
