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
namespace tools
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