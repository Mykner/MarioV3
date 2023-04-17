#pragma once

#if defined(USE_NA_LEAK_DETECTOR)

#include <map>
#include <Windows.h>

#ifdef _DEBUG
#define na_new new(__FILE__, __LINE__)
#else
#define na_new new
#endif

void* operator new(size_t sz, char* filename, int line);
void operator delete(void *p);
void* operator new[](size_t size, const char *filename, int line);
void operator delete[](void *p);

typedef struct tagMemoryAllocationInfo
{
	char   filename[256];
	int    line;
	time_t allocate_time; 
	void*  addr;
} MemoryAllocationInfo;

typedef std::map<void *, MemoryAllocationInfo> MemoryAllocationInfoMap;

extern MemoryAllocationInfoMap g_mapMemoryAllocationInfo;

void CheckCurrentAllocatedMemory();

class NaCriticalSection
{
public:
	NaCriticalSection()
	{
		InitializeCriticalSection(&m_cs);
	};
	~NaCriticalSection()
	{
		DeleteCriticalSection(&m_cs);
	};
	void Enter()
	{
		EnterCriticalSection(&m_cs);
	}
	void Leave()
	{
		LeaveCriticalSection(&m_cs);
	};

	CRITICAL_SECTION m_cs;
};

#endif