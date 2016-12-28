#include "stdafx.h"
#include "MemoryMgr.h"

template<> CMemoryMgr* CSingleton<CMemoryMgr>::ms_Singleton = NULL;
CMemoryMgr::CMemoryMgr()
{
	hHeap = ::HeapCreate(0, 0, 0);
	if (hHeap == NULL)
	{
		DebugString(L"�����Ѿ��ʧ�ܣ�%d", GetLastError());
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
		DebugString(L"�����ڴ�ʧ��:%d", GetLastError());
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

PVOID CMemoryMgr::malloc(DWORD dwSize)
{
	return HeapAlloc(hHeap, 0, dwSize);
}
//���dwSize��ԭ���ĸ�С����ֻ����ԭ��ǰ����ǲ���
PVOID CMemoryMgr::realloc(PVOID pSrc, DWORD dwSize)
{
	DWORD dwSrcSize = HeapSize(hHeap, 0, pSrc);
	PVOID pDest = HeapAlloc(hHeap, 0, dwSize);
	if (dwSrcSize > dwSize) dwSrcSize = dwSize;
	CopyMemory(pDest, pSrc, dwSrcSize);
	HeapFree(hHeap, 0, pSrc);
	return pDest;
}

void CMemoryMgr::free(PVOID pSrc)
{
	HeapFree(hHeap, 0, pSrc);
}
