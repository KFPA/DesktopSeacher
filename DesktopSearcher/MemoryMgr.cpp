#include "stdafx.h"
#include "MemoryMgr.h"

template<> CMemoryMgr* CSingleton<CMemoryMgr>::ms_Singleton = NULL;
CMemoryMgr::CMemoryMgr()
{
	hHeap = ::HeapCreate(0, 0, 0);
	if (hHeap == NULL)
	{
		DebugString(L"¥¥Ω®∂—æ‰±˙ ß∞‹£∫%d", GetLastError());
	}
}


CMemoryMgr::~CMemoryMgr()
{
}

PBYTE CMemoryMgr::GetMemory(DWORD dwByte, BOOL bIniZero /*= FALSE*/)
{
	PVOID pRet;
	if (bIniZero)
		pRet = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwByte);
	else
		pRet = HeapAlloc(hHeap, 0, dwByte);
	if (NULL == pRet)
	{
		DebugString(L"…Í«Îƒ⁄¥Ê ß∞‹:%d", GetLastError());
	}
	return (PBYTE)pRet;
}

void CMemoryMgr::FreeMemory(PBYTE ptr)
{
	HeapFree(hHeap, 0, ptr);
}

CMemoryMgr* CMemoryMgr::GetMemoryMgr()
{
	if (ms_Singleton != nullptr)
	{
		return ms_Singleton;
	}
	ms_Singleton = new CMemoryMgr;
	return ms_Singleton;
}
