#pragma once

#define INFO_LENGTH				100
#define FILE_LENGTH				128
#define LOG_FILE_LENGTH			30
#define LOG_CONTENTS_LENGTH		2048
#define LOG_NOALLOC				"NOALLOC"
#define LOG_ARRAY				"ARRAY"
#define LOG_LEAK				"LEAK"

struct MemoryManagerInfo
{
	int size;
	int line;
	bool array;
	bool isDelete;
	void* ptr;
	char fileName[FILE_LENGTH];
};

class MemoryManager
{
public:
	MemoryManager();
	/// 메모리 할당 후 해제가 안된 메모리 로그 저장
	~MemoryManager();

	/// mangerInfo 배열에 동적할당 정보 삽입
	void push(void* ptr, const size_t size, const char* file, const int line, const bool isArray);
	/// 메모리 해제시 오류 확인
	/// - 실제 할당된 적이 있는 포인터인지, 배열 형태는 맞는지
	/// - 오류시 로그 파일 저장
	void findMemory(void* p, bool isArray);

private:
	/// _managerInfo에서 할당된 메모리 찾기
	bool findInfo(void* p, MemoryManagerInfo& info);
	/// 로그 파일 내용 생성
	void makeLogContents(char* buffer, const char* errName, const MemoryManagerInfo& info);
	/// 로그 파일 생성
	void makeLogFile(const char* buffer);
	void makeLogFile(const char* errName, const MemoryManagerInfo& info);
	/// 로그 파일명 생성
	/// Alloc_YYYYMMDD_HHMMSS.txt
	void setFileName(char* fileName);

private:
	int _infoIndex = 0;
	MemoryManagerInfo _managerInfo[INFO_LENGTH];
};
