#pragma once
class CMemoryMgr : public CSingleton<CMemoryMgr>
{
public:
	CMemoryMgr();
	~CMemoryMgr();
private:
	HANDLE hHeap;
public:
	static CMemoryMgr* GetMemoryMgr();
	PBYTE GetMemory(DWORD dwByte, BOOL bIniZero = FALSE);
	void FreeMemory(PBYTE ptr);
};

