// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <wtypes.h>
#include <crtdbg.h>
//////////////////////////////////////////////////////////////////////////火柴盒修改，防止检测内存泄露不准问题
struct _DEBUG_STATE
{
	_DEBUG_STATE() {}
	~_DEBUG_STATE() { _CrtDumpMemoryLeaks(); }
};

_DEBUG_STATE g_ds;
//////////////////////////////////////////////////////////////////////////
// TODO: reference additional headers your program requires here
