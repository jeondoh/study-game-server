#pragma once

#define DEFAULT_SIZE	4096

/// 네트워크 송-수신에 사용할 링버퍼
class RingBuffer
{
public:
	/// 생성자
	RingBuffer();
	RingBuffer(int size);

	/// 소멸자
	~RingBuffer();

	/// 버퍼 크기 반환
	int getBufferSize();

	/// 현재 사용중인 용량 얻기
	int getUseSize();

	/// 현재 버퍼에 남은 용량 얻기
	int getFreeSize();

	/// 데이터 넣기
	/// 넣은 크기만큼 리턴
	int enqueue(const char* buffer, int size);

	/// 데이터 가져오기
	/// 가져온 크기만큼 리턴
	int dequeue(char* buffer, int size);

	/// 데이터 읽어오기
	/// 가져온 크기만큼 리턴
	int peek(char* buffer, int size);

	/// 끊기지 않고 최대로 넣을 수 있는 길이
	int directEnqueueSize();

	/// 끊기지 않고 최대로 가져올 수 있는 길이
	int directDequeueSize();

	/// 전달한 길이만큼 rear 위치 이동
	/// 이동한 크기 리턴
	int moveRear(int size);

	/// 전달한 길이만큼 front 위치 이동
	/// 이동한 크기 리턴
	int moveFront(int size);

	/// rear에 해당하는 포인터 리턴
	char* getRearBufferPtr();

	/// front에 해당하는 포인터 리턴
	char* getFrontBufferPtr();

	/// 버퍼에 있는 모든 데이터 삭제
	void clearBuffer();

private:
	char* _buffer;
	int _bufferSize;
	int _front;
	int _rear;
};
