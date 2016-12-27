#include "stdafx.h"
#include "SearchWorker.h"

template<> CSearchWorker* CSingleton<CSearchWorker>::ms_Singleton = NULL;
CSearchWorker::CSearchWorker()
{
}


CSearchWorker::~CSearchWorker()
{
}

CSearchWorker* CSearchWorker::GetDispatchCenter()
{
	if (ms_Singleton != nullptr)
	{
		return ms_Singleton;
	}
	ms_Singleton = new CSearchWorker;
	return ms_Singleton;
}

BOOL CSearchWorker::OpenNtfsVolume()
{
	BOOL bRet = FALSE;
	WCHAR tDrivers[26 * 4 + 1];
	GetLogicalDriveStringsW(26 * 4 + 1, tDrivers);
	
	DWORD dwDri; //��� 0~25
	
	int iFilterRoot = 0;

	for (WCHAR* pDri = tDrivers; *pDri != '\0';pDri += 4)
	{
		if (*pDri >= 'a')
		{
			*pDri -= 32; //ת��Ϊ��д
		}
		dwDri = *pDri - L'A';
		if (DRIVE_FIXED == GetDriveTypeW(pDri))
		{
			DWORD dwMaxComLen, dwFileSysFlag;
			
			WCHAR szVolumeName[32];
			WCHAR fileSysBuf[8];
			GetVolumeInformationW(pDri, szVolumeName, 32, NULL, &dwMaxComLen, &dwFileSysFlag, fileSysBuf, 8);
			if (fileSysBuf[0] == L'N' && fileSysBuf[1] == L'T' && fileSysBuf[2] == L'F' && fileSysBuf[3] == L'S')
			{
				//////////��ȡ�����̾��/////////////
				HANDLE hVolume = Usntools::GetDriversHandle(pDri);
				if (INVALID_HANDLE_VALUE == hVolume)
				{
					SStringT strContent;
					strContent.Format(GETSTRING(L"@string/volerror"), *pDri, GetLastError());
					DebugString(strContent);
				}

				mapHandle.SetAt(dwDri, hVolume);  //������

				//////////����USN��־/////////////
				NTFS_VOLUME_DATA_BUFFER ntfsVolData;
				Usntools::CreateUsnJournal(hVolume, ntfsVolData);

				mapBytesPerCluster.SetAt(dwDri, ntfsVolData.BytesPerCluster);
				mapFileRecSize.SetAt(dwDri, sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) - 1 + ntfsVolData.BytesPerFileRecordSegment);
			
				//////////��ѯUSN��־/////////////
				USN_JOURNAL_DATA usnJournalData;
				Usntools::QueryUsnJournal(hVolume, &usnJournalData);
				mapJournalID.SetAt(dwDri, usnJournalData.UsnJournalID);
				mapFirstUSN.SetAt(dwDri, usnJournalData.FirstUsn);
				mapNextUSN.SetAt(dwDri, usnJournalData.NextUsn);
			}
		}
	}

	return bRet;
}

BOOL CSearchWorker::IsNTFS(char cDriName)
{
	char tDrivers[] = "C:\\";
	tDrivers[0] = cDriName;

	DWORD dwMaxComLen, dwFileSysFlag;

	char szVolumeName[32];
	char fileSysBuf[8];
	GetVolumeInformationA(tDrivers, szVolumeName, 32, NULL, &dwMaxComLen, &dwFileSysFlag, fileSysBuf, 8);
	if (!strcmp(fileSysBuf,"NTFS"))
	{
		return TRUE;
	}
	return FALSE;
}

void CSearchWorker::InitDrives()
{
	char cvol;
	for (int i=0; i < 26;i++)
	{
		cvol = 'A' + i;
		if (IsNTFS(cvol))
		{
			vol[i] = cvol;
		}
	}
}

BOOL CSearchWorker::LoadDatabase(HWND hMainWnd)
{
	BOOL bRet = TRUE;
	HANDLE hDB = CreateFileW(DATABASEDIR,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hDB)
	{
		SStringT strContent;
		strContent.Format(GETSTRING(L"@string/volerror"), hDB, GetLastError());
		DebugString(strContent);
		bRet = FALSE;
	}

	DWORD dwFileSize = GetFileSize(hDB, NULL);
	if (INVALID_FILE_SIZE == dwFileSize || dwFileSize <(16+20+8+26))
	{
		SStringT strContent;
		strContent.Format(GETSTRING(L"@string/errorcode"), GetLastError());
		DebugString(strContent);
		bRet = FALSE;
	}
//	PBYTE DB_Buffer = g_MemoryMgr.GetMemory(dwFileSize);
// 	DWORD dwRead;
// 	BOOL bReadOK = ReadFile(hDB, DB_Buffer, dwFileSize, &dwRead, NULL);
// 	if (!bReadOK || dwRead != dwFileSize)
// 	{
// 		DebugString(L"Failed to read file:%d", GetLastError());
// 		bRet = FALSE;
// 	}


	return bRet;
}
