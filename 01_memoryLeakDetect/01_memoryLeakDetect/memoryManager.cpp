#include <iostream>

#include "memoryManager.h"

MemoryManager::MemoryManager()
{
	printf("MemoryManager ������\n");
}

/// �޸� �Ҵ� �� ������ �ȵ� �޸� �α� ����
MemoryManager::~MemoryManager()
{
	printf("MemoryManager �Ҹ���\n");

	char buffer[LOG_CONTENTS_LENGTH] = { '\0' };
	char contentsBuffer[LOG_CONTENTS_LENGTH] = { '\0' };
	int bufferPointer = 0;

	int iCnt;
	for (iCnt = 0; iCnt < _infoIndex; ++iCnt)
	{
		// �������� ���� �޸�
		if (!_managerInfo[iCnt].isDelete)
		{
			// ���۸� �ѹ��� ��Ƽ� ���� ����� 1���� �ϵ���
			makeLogContents(buffer, LOG_LEAK, _managerInfo[iCnt]);
			strcat_s(contentsBuffer, buffer);
		}
	}
	// ���ۿ� ������ ������쿡�� �α����� ����
	if (strlen(contentsBuffer) > 0)
	{
		makeLogFile(contentsBuffer);
	}
}

/// mangerInfo �迭�� �����Ҵ� ���� ����
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

/// �޸� ������ ���� Ȯ��
/// - ���� �Ҵ�� ���� �ִ� ����������, �迭 ���´� �´���
/// - ������ �α� ���� ����
void MemoryManager::findMemory(void* p, bool isArray)
{
	MemoryManagerInfo info;
	MemoryManagerInfo secondInfo;
	// �Ҵ�� �޸� ã��
	bool isFind = findInfo(p, info);
	bool secondFind;
	char* bytePtr = reinterpret_cast<char*>(p);
	// �Ҵ��ߴ� �޸� �ּҸ� ã�� �� ���ٸ�
	if (!isFind)
	{
		// -4 ������ �����ͷ� ��˻�
		// new[]�� �޸𸮸� �Ҵ��ߴµ�, delete�� �����ϴ� ��� : -4
		// new�� �޸𸮸� �Ҵ��ߴµ�, delete[]�� �����ϴ� ��� : +4
		bytePtr = isArray ? bytePtr + 4 : bytePtr - 4;
		void* findPtr = reinterpret_cast<void*>(bytePtr);
		secondFind = findInfo(findPtr, secondInfo);
		if (!secondFind)
		{
			// �Ҵ����� ���� �޸��̴�. (NOALLOC)
			// �α׷� ����
			makeLogFile(LOG_NOALLOC, secondInfo);
		}
		else
		{
			// �߸� ������
			if (isArray)
			{
				printf("new �Ҵ�, delete[]�� �߸�����\n");
			}
			else
			{
				printf("new[] �Ҵ�, delete�� �߸�����\n");
			}
			makeLogFile(LOG_ARRAY, secondInfo);
		}
	}
}

/// _managerInfo���� �Ҵ�� �޸� ã��
bool MemoryManager::findInfo(void* p, MemoryManagerInfo& info)
{
	int iCnt;
	for (iCnt = 0; iCnt < _infoIndex; ++iCnt)
	{
		if (_managerInfo[iCnt].isDelete)
		{
			continue;
		}
		// �����ϴ� �����Ͱ� �Ҵ��� �����Ϳ� ��ġ�Ҷ�
		if (_managerInfo[iCnt].ptr == p)
		{
			// �޸𸮰� �ùٸ��� �����Ǳ� ������ �Ҵ系�� ������ �ǹ̷� true
			_managerInfo[iCnt].isDelete = true;
			info = _managerInfo[iCnt];
			return true;
		}
	}
	return false;
}

/// �α� ���� ���� ����
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

/// �α� ���� ����
void MemoryManager::makeLogFile(const char* buffer)
{
	FILE* logFile;
	errno_t err;
	char logFileName[LOG_FILE_LENGTH];
	// ���ϸ� ����
	setFileName(logFileName);
	err = fopen_s(&logFile, logFileName, "w");
	// warning
	if (err != 0)
	{
		return;
	}
	// ����
	fwrite(buffer, strlen(buffer), 1, logFile);
	// �ݱ�
	fclose(logFile);
}

/// �α� ���� ����
void MemoryManager::makeLogFile(const char* errName, const MemoryManagerInfo& info)
{
	FILE* logFile;
	errno_t err;
	char logFileName[LOG_FILE_LENGTH] = { '\0' };
	char buffer[LOG_CONTENTS_LENGTH] = { '\0' };
	// ���ϸ� ����
	setFileName(logFileName);
	err = fopen_s(&logFile, logFileName, "w");
	// warning
	if (err != 0)
	{
		return;
	}
	// ���� ���� buffer�� �ޱ�
	makeLogContents(buffer, errName, info);
	// ����
	fwrite(buffer, strlen(buffer), 1, logFile);
	// �ݱ�
	fclose(logFile);
}

/// �α� ���ϸ� ����
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
