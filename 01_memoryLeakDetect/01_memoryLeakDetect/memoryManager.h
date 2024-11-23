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
	/// �޸� �Ҵ� �� ������ �ȵ� �޸� �α� ����
	~MemoryManager();

	/// mangerInfo �迭�� �����Ҵ� ���� ����
	void push(void* ptr, const size_t size, const char* file, const int line, const bool isArray);
	/// �޸� ������ ���� Ȯ��
	/// - ���� �Ҵ�� ���� �ִ� ����������, �迭 ���´� �´���
	/// - ������ �α� ���� ����
	void findMemory(void* p, bool isArray);

private:
	/// _managerInfo���� �Ҵ�� �޸� ã��
	bool findInfo(void* p, MemoryManagerInfo& info);
	/// �α� ���� ���� ����
	void makeLogContents(char* buffer, const char* errName, const MemoryManagerInfo& info);
	/// �α� ���� ����
	void makeLogFile(const char* buffer);
	void makeLogFile(const char* errName, const MemoryManagerInfo& info);
	/// �α� ���ϸ� ����
	/// Alloc_YYYYMMDD_HHMMSS.txt
	void setFileName(char* fileName);

private:
	int _infoIndex = 0;
	MemoryManagerInfo _managerInfo[INFO_LENGTH];
};
