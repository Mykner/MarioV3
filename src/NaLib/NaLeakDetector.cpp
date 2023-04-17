#include "NaLeakDetector.h"

#include "NaCommon.h"
#include "NaString.h"

#if defined(USE_NA_LEAK_DETECTOR)

MemoryAllocationInfoMap g_mapMemoryAllocationInfo;
NaCriticalSection g_cs;

void* operator new(size_t sz, char* filename, int line) throw(std::bad_alloc)
{
	void *pMem = malloc(sz);

	/*
	g_cs.Enter();
	MemoryAllocationInfo info;
	info.addr = pMem;
	info.line = line;
	info.allocate_time = 0;
	//time(&info.allocate_time);

	strncpy_s(info.filename, filename, strlen(filename));
	
	g_mapMemoryAllocationInfo.insert(
		std::pair<void*, MemoryAllocationInfo>(pMem, info)
	);
	g_cs.Leave();
	*/

	return pMem;
}

void operator delete(void *p) throw()
{
	if (p == nullptr)
		return;

	/*
	g_cs.Enter();
	MemoryAllocationInfoMap::iterator it = g_mapMemoryAllocationInfo.find(p);
	if (g_mapMemoryAllocationInfo.end() == it)
	{
		// Not created by na_new
		free(p);
		return;
	}

	g_mapMemoryAllocationInfo.erase(it);
	g_cs.Leave();
	*/

	free(p);
}

void *operator new[](size_t size, const char *filename, int line)
{
	return operator new(size, const_cast<char*>(filename), line);
}

void operator delete[](void *p)
{
	operator delete(p);
}

void CheckCurrentAllocatedMemory()
{
	MemoryAllocationInfoMap::iterator it = g_mapMemoryAllocationInfo.begin();
	while (g_mapMemoryAllocationInfo.end() != it)
	{
		NaDebugOut(L"%s(%d): 0x%08x\n",
			NaString(it->second.filename).wstr(),
			it->second.line,
			it->second.addr
			);

		++it;
	}
}

#endif