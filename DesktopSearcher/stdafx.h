// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once
#define  _CRT_SECURE_NO_WARNINGS
#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include "resource.h"

#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
using namespace SOUI;





#include "Helper.h"
#include "NameSort.h"
#include "MemoryMgr.h"
#include "MemoryPool.h"
#include "ExtArray.h"
#include "queue.h"

#include "Record.h"
#include "IndexNodeBlock.h"
#include "Index.h"
#include "DirBasicInfoMap.h"

#include "OutVector.h"
#include "StrMatch.h"


typedef PVOID   *PPVOID;
typedef PDWORD  *PPDWORD;
typedef PBYTE   *PPBYTE;
typedef PPBYTE  *PPPBYTE;

extern BYTE g_NoCaseTable[]; //搜索时已经初始化

extern CMemoryMgr g_MemoryMgr;
extern CMemoryPool g_MemFile, g_MemDir;//文件和目录的数据都保存在此处
extern CIndex g_vDirIndex, g_vFileIndex;
extern CDirBasicInfoMap g_DirMap;
extern CExtArray g_ExtMgr;
extern COutVector g_vDirOutPtr, g_vFileOutPtr;




inline void DebugStringA(char *pszFormat,...)
{
	//#ifdef _DEBUG
	va_list argList;
	va_start(argList, pszFormat);
	char strInfo[4096];
	vsprintf(strInfo, pszFormat, argList);
	va_end(argList);
	MessageBoxA(NULL, strInfo, "错误", MB_ICONHAND);
	//#endif
}

inline void DebugStringW(WCHAR *pszFormat, ...)
{
	//#ifdef _DEBUG
	va_list argList;
	va_start(argList, pszFormat);
	WCHAR strInfo[4096];
	vswprintf(strInfo, pszFormat, argList);
	va_end(argList);
	MessageBoxW(NULL, strInfo, L"错误", MB_ICONHAND);
	//#endif
}
#ifdef UNICODE
#define DebugString  DebugStringW
#else
#define DebugString  DebugStringA
#endif

class CDebugTrace
{
public:
	CDebugTrace()
	{
		m_pFile = NULL;
	}
	virtual ~CDebugTrace()
	{
		if (m_pFile) {
			fclose(m_pFile);
			m_pFile = NULL;
		}
	}
	void operator=(FILE *pFile)
	{
		m_pFile = pFile;
	}
	operator FILE*(){ return m_pFile; }
private:
	FILE *m_pFile;
};

extern CDebugTrace g_dbgTrace;

inline void DebugTrace0(char *DebugInfo, ...)//inline无法内联但防止重定义
{
	static char szInfoBuf[1024];
	if (NULL == g_dbgTrace)
	{
		g_dbgTrace = fopen("C:\\NtfsTitleFinder.log", "w");
		if (NULL == g_dbgTrace) return;
		g_dbgTrace = g_dbgTrace;
	}
	va_list argList;
	va_start(argList, DebugInfo);
	vsprintf(szInfoBuf, DebugInfo, argList);
	va_end(argList);
	fprintf(g_dbgTrace, "%s", szInfoBuf);
	fflush(g_dbgTrace);
}

#ifdef _DEBUG
#define DebugTrace(...)	DebugTrace0(__VA_ARGS__)
#else
#define DebugTrace(...)
#endif
/**
* Copyright (C) 2014-2050
* All rights reserved.
*
* @file       SSingleton.h
* @brief
* @version    v1.0
* @author     SOUI group
* @date       2014/08/02
*
* Describe    SOUI系统中使用的单件模块
*/

#pragma once
#include <assert.h>

namespace SOUI
{

	/**
	* @class      SSingleton
	* @brief      单件模板
	*
	* Describe
	*/
	template <typename T>
	class CSingleton
	{
	protected:
		static    T* ms_Singleton;

	public:
		CSingleton(void)
		{
			assert(!ms_Singleton);
			ms_Singleton = static_cast<T*>(this);
		}
		virtual ~CSingleton(void)
		{
			assert(ms_Singleton);
			ms_Singleton = 0;
		}
		static T& getSingleton(void)
		{
			assert(ms_Singleton);
			return (*ms_Singleton);
		}
		static T* getSingletonPtr(void)
		{
			return (ms_Singleton);
		}

	private:
		CSingleton& operator=(const CSingleton&)
		{
			return *this;
		}
		CSingleton(const CSingleton&) {}
	};

}//namespace SOUI
#define DATABASEDIR L"C:\\ntfsds.db" //数据库文件存在此目录下