#include <iostream>

#include "memoryManager.h"

MemoryManager::MemoryManager()
{
	printf("MemoryManager 생성자\n");
}

/// 메모리 할당 후 해제가 안된 메모리 로그 저장
MemoryManager::~MemoryManager()
{
	printf("MemoryManager 소멸자\n");

	char buffer[LOG_CONTENTS_LENGTH] = { '\0' };
	char contentsBuffer[LOG_CONTENTS_LENGTH] = { '\0' };
	int bufferPointer = 0;

	int iCnt;
	for (iCnt = 0; iCnt < _infoIndex; ++iCnt)
	{
		// 해제되지 않은 메모리
		if (!_managerInfo[iCnt].isDelete)
		{
			// 버퍼를 한번에 모아서 파일 쓰기는 1번만 하도록
			makeLogContents(buffer, LOG_LEAK, _managerInfo[iCnt]);
			strcat_s(contentsBuffer, buffer);
		}
	}
	// 버퍼에 내용이 있을경우에만 로그파일 생성
	if (strlen(contentsBuffer) > 0)
	{
		makeLogFile(contentsBuffer);
	}
}

/// mangerInfo 배열에 동적할당 정보 삽입
void MemoryManager::push(void* ptr, const size_t size, const char* file, const int line, const bool isArray)
{
	MemoryManagerInfo info;
	info.size = size;
	info.line = line;
	info.array = isArray;
	info.isDelete = false;
	info.ptr = ptr;
	strcpy_s(info.fileName, FILE_LENGTH, file);

	_managerInfo[_infoIndex] = info;
	++_infoIndex;
}

/// 메모리 해제시 오류 확인
/// - 실제 할당된 적이 있는 포인터인지, 배열 형태는 맞는지
/// - 오류시 로그 파일 저장
void MemoryManager::findMemory(void* p, bool isArray)
{
	MemoryManagerInfo info;
	MemoryManagerInfo secondInfo;
	// 할당된 메모리 찾기
	bool isFind = findInfo(p, info);
	bool secondFind;
	char* bytePtr = reinterpret_cast<char*>(p);
	// 할당했던 메모리 주소를 찾을 수 없다면
	if (!isFind)
	{
		// -4 지점의 포인터로 재검색
		// new[]로 메모리를 할당했는데, delete로 해제하는 경우 : -4
		// new로 메모리를 할당했는데, delete[]로 해제하는 경우 : +4
		bytePtr = isArray ? bytePtr + 4 : bytePtr - 4;
		void* findPtr = reinterpret_cast<void*>(bytePtr);
		secondFind = findInfo(findPtr, secondInfo);
		if (!secondFind)
		{
			// 할당한적 없는 메모리이다. (NOALLOC)
			// 로그로 남김
			makeLogFile(LOG_NOALLOC, secondInfo);
		}
		else
		{
			// 잘못 해제함
			if (isArray)
			{
				printf("new 할당, delete[]로 잘못해제\n");
			}
			else
			{
				printf("new[] 할당, delete로 잘못해제\n");
			}
			makeLogFile(LOG_ARRAY, secondInfo);
		}
	}
}

/// _managerInfo에서 할당된 메모리 찾기
bool MemoryManager::findInfo(void* p, MemoryManagerInfo& info)
{
	int iCnt;
	for (iCnt = 0; iCnt < _infoIndex; ++iCnt)
	{
		if (_managerInfo[iCnt].isDelete)
		{
			continue;
		}
		// 해제하는 포인터가 할당한 포인터와 일치할때
		if (_managerInfo[iCnt].ptr == p)
		{
			// 메모리가 올바르게 해제되기 때문에 할당내역 삭제의 의미로 true
			_managerInfo[iCnt].isDelete = true;
			info = _managerInfo[iCnt];
			return true;
		}
	}
	return false;
}

/// 로그 파일 내용 생성
void MemoryManager::makeLogContents(char* buffer, const char* errName, const MemoryManagerInfo& info)
{
	if (strcmp(errName, LOG_NOALLOC) == 0)
	{
		sprintf_s(buffer, LOG_CONTENTS_LENGTH, "%s [0x%p]\n", errName, info.ptr);
	}
	else
	{
		sprintf_s(buffer, LOG_CONTENTS_LENGTH, "%s [0x%p] [%d] [%s] : [%d]\n",
			errName, info.ptr, info.size, info.fileName, info.line);
	}
}

/// 로그 파일 생성
void MemoryManager::makeLogFile(const char* buffer)
{
	FILE* logFile;
	errno_t err;
	char logFileName[LOG_FILE_LENGTH];
	// 파일명 지정
	setFileName(logFileName);
	err = fopen_s(&logFile, logFileName, "w");
	// warning
	if (err != 0)
	{
		return;
	}
	// 쓰기
	fwrite(buffer, strlen(buffer), 1, logFile);
	// 닫기
	fclose(logFile);
}

/// 로그 파일 생성
void MemoryManager::makeLogFile(const char* errName, const MemoryManagerInfo& info)
{
	FILE* logFile;
	errno_t err;
	char logFileName[LOG_FILE_LENGTH] = { '\0' };
	char buffer[LOG_CONTENTS_LENGTH] = { '\0' };
	// 파일명 지정
	setFileName(logFileName);
	err = fopen_s(&logFile, logFileName, "w");
	// warning
	if (err != 0)
	{
		return;
	}
	// 파일 내용 buffer로 받기
	makeLogContents(buffer, errName, info);
	// 쓰기
	fwrite(buffer, strlen(buffer), 1, logFile);
	// 닫기
	fclose(logFile);
}

/// 로그 파일명 생성
/// Alloc_YYYYMMDD_HHMMSS.txt
void MemoryManager::setFileName(char* fileName)
{
	struct tm newTime;
	__time32_t longTime;
	errno_t err;

	_time32(&longTime);
	err = _localtime32_s(&newTime, &longTime);

	sprintf_s(fileName, LOG_FILE_LENGTH, "Alloc_%04d%02d%02d_%02d%02d%02d.txt",
		newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
}
