#include <iostream>

#include "myNew.h"
#include "memoryManager.h"

#undef new

MemoryManager manager;

void* operator new(size_t size, const char* file, int line)
{
	printf("operater new\n");
	// 동적할당한 정보 전달
	void* ptr = malloc(size);
	manager.push(ptr, size, file, line, false);
	return ptr;
}

void* operator new[](size_t size, const char* file, int line)
{
	printf("operater new[]\n");
	// 동적할당한 정보 전달
	void* ptr = malloc(size);
	manager.push(ptr, size, file, line, true);
	return ptr;
}

void operator delete(void* p)
{
	// 동적할당된 포인터 전달
	manager.findMemory(p, false);
	free(p);
	printf("operater delete\n");
}

void operator delete[](void* p)
{
	// 동적할당된 포인터 전달
	manager.findMemory(p, true);
	free(p);
	printf("operater delete[]\n");
}
// 사용 X
void operator delete(void* p, char* file, int Line)
{
}
// 사용 X
void operator delete[](void* p, char* file, int Line)
{
}
