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
* Describe    SOUIϵͳ��ʹ�õĵ���ģ��
*/

#pragma once
#include <assert.h>

namespace SOUI
{

	/**
	* @class      SSingleton
	* @brief      ����ģ��
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
namespace Usntools
{
	static HANDLE GetDriversHandle(WCHAR* pDri)
	{
		WCHAR szVolumePath[8];
		swprintf(szVolumePath, L"\\\\.\\%c:", *pDri);
		HANDLE hVolume = CreateFileW(szVolumePath,
			GENERIC_READ | GENERIC_WRITE,  // ����Ϊ0
			FILE_SHARE_READ | FILE_SHARE_WRITE,// ���������FILE_SHARE_WRITE
			NULL,
			OPEN_EXISTING,// �������OPEN_EXISTING, CREATE_ALWAYS���ܻᵼ�´���
			FILE_ATTRIBUTE_READONLY,// FILE_ATTRIBUTE_NORMAL���ܻᵼ�´���
			NULL);
		return hVolume;
	}
	static BOOL CreateUsnJournal(HANDLE hVolume,NTFS_VOLUME_DATA_BUFFER &ntfsVolData)
	{
		DWORD dwWritten;
		return DeviceIoControl(hVolume, // handle to volume
			FSCTL_GET_NTFS_VOLUME_DATA,// dwIoControlCode
			NULL, 0,// input buffer�� size of input buffer
			&ntfsVolData, sizeof(ntfsVolData),  // lpOutBuffer// nOutBufferSize
			&dwWritten, NULL); // number of bytes returned
	}
	static BOOL QueryUsnJournal(HANDLE hVolume, PUSN_JOURNAL_DATA pUsnJournalData)
	{
		DWORD dwCB;
		return DeviceIoControl(hVolume
			, FSCTL_QUERY_USN_JOURNAL
			, NULL, 0,
			pUsnJournalData, sizeof(USN_JOURNAL_DATA), &dwCB
			, NULL);
	}
}
const int ALPHABETA_SIZE = 0x80;
namespace tools
{
	//����ucs2Len
	//ucs2Name ��L'\0'��β
	static int CodeToUcs2Case(OUT PWCHAR ucs2Name, IN PBYTE pCode, IN int codeLen)
	{
		PBYTE pCodeEnd = pCode + codeLen;
		PWCHAR pUcs2 = ucs2Name;
		for (; pCode < pCodeEnd; ++pUcs2)
		{
			if (*pCode < 0x80){
				*pUcs2 = *pCode++;
			}
			else{
				++pCode;
				*pUcs2 = *(PWCHAR)pCode;
				pCode += 2;
			}
		}
		*pUcs2 = L'\0';
		return pUcs2 - ucs2Name;
	}

	//����ucs2Len
	//ucs2Name ��L'\0'��β
	//ucs2Name�е���ĸΪСд
	static INT CodeToUcs2NoCase(OUT PWCHAR ucs2Name, IN PBYTE pCode, IN int codeLen)
	{
		extern BYTE g_NoCaseTable[ALPHABETA_SIZE];
		PBYTE pCodeEnd = pCode + codeLen;
		PWCHAR pUcs2 = ucs2Name;
		BYTE code;
		for (; pCode < pCodeEnd; ++pUcs2)
		{
			code = *pCode;
			if (code < 0x80){
				*pUcs2 = g_NoCaseTable[code];
				++pCode;
			}
			else{
				++pCode;//������ʶ�ַ�
				*pUcs2 = *(PWCHAR)pCode;
				pCode += 2;
			}
		}
		*pUcs2 = L'\0';
		return pUcs2 - ucs2Name;
	}
	static void InitNoCaseTable()
	{
		extern BYTE g_NoCaseTable[ALPHABETA_SIZE];
		for (int i = 0; i < ALPHABETA_SIZE; ++i){
			if (i >= 'A' && i <= 'Z') g_NoCaseTable[i] = i + 32;
			else g_NoCaseTable[i] = i;
		}
	}
	static int Ucs2ToCode(OUT PBYTE pCode, IN PWCHAR pUcs2, IN int ucs2Len)
	{
		const PWCHAR pUcs2End = pUcs2 + ucs2Len;
		const PBYTE pCodeBeg = pCode;
		WCHAR wch;
		for (; pUcs2 < pUcs2End; ++pUcs2)
		{
			wch = *pUcs2;
			if (wch < 0x80){//������ĸ����
				*pCode++ = wch;
			}
			else{
				*pCode++ = 0x81;
				*(PWCHAR)pCode = wch;
				pCode += 2;
			}
		}
		return pCode - pCodeBeg;
	}
	static int Ucs2ToCodeCase(OUT PBYTE pCode, IN PWCHAR pUcs2, IN int ucs2Len)
	{
		const PWCHAR pUcs2End = pUcs2 + ucs2Len;
		const PBYTE pCodeBeg = pCode;
		WCHAR wch;
		for (; pUcs2 < pUcs2End; ++pUcs2)
		{
			wch = *pUcs2;
			if (wch < 0x80){//������ĸ����
				*pCode++ = wch;
			}
			else{
				*pCode++ = 0x81;
				*(PWCHAR)pCode = wch;
				pCode += 2;
			}
		}
		return pCode - pCodeBeg;
	}
}

#define DATABASEDIR L"C:\\ntfsds.db" //���ݿ��ļ����ڴ�Ŀ¼��

inline void DebugString(SOUI::SStringW strContent, ...)
{
#ifdef _DEBUG
	MessageBox(NULL, strContent, SOUI::GETSTRING(L"@string/title"), MB_OK);
#endif
}