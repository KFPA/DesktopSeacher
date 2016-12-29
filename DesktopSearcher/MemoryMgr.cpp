#include "stdafx.h"
#include "MemoryMgr.h"

CMemoryMgr::CMemoryMgr()
{
	hHeap = ::HeapCreate(0, 0, 0);
	if (hHeap == NULL)
	{
		DebugStringA("�����Ѿ��ʧ�ܣ�%d", GetLastError());
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
		DebugStringA("�����ڴ�ʧ��:%d", GetLastError());
	}
	return (PBYTE)pRet;
}

void CMemoryMgr::FreeMemory(PBYTE ptr)
{
	HeapFree(hHeap, 0, ptr);
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
