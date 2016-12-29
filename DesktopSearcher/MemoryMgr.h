#pragma once
class CMemoryMgr 
{
public:
	CMemoryMgr();
	~CMemoryMgr();
private:
	HANDLE hHeap;
public:
	PBYTE GetMemory(DWORD dwByte, BOOL bIniZero = FALSE);
	void FreeMemory(PBYTE ptr);

	PVOID malloc(DWORD dwSize);
	PVOID realloc(PVOID pSrc, DWORD dwSize);
	void free(PVOID pSrc);
};

