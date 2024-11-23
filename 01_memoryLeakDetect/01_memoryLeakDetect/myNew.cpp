#include <iostream>

#include "myNew.h"
#include "memoryManager.h"

#undef new

MemoryManager manager;

void* operator new(size_t size, const char* file, int line)
{
	printf("operater new\n");
	// �����Ҵ��� ���� ����
	void* ptr = malloc(size);
	manager.push(ptr, size, file, line, false);
	return ptr;
}

void* operator new[](size_t size, const char* file, int line)
{
	printf("operater new[]\n");
	// �����Ҵ��� ���� ����
	void* ptr = malloc(size);
	manager.push(ptr, size, file, line, true);
	return ptr;
}

void operator delete(void* p)
{
	// �����Ҵ�� ������ ����
	manager.findMemory(p, false);
	free(p);
	printf("operater delete\n");
}

void operator delete[](void* p)
{
	// �����Ҵ�� ������ ����
	manager.findMemory(p, true);
	free(p);
	printf("operater delete[]\n");
}
// ��� X
void operator delete(void* p, char* file, int Line)
{
}
// ��� X
void operator delete[](void* p, char* file, int Line)
{
}
