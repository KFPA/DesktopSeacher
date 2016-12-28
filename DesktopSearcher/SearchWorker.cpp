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
	PBYTE DB_Buffer = g_pMemoryMgr->GetMemory(dwFileSize);
 	DWORD dwRead;
 	BOOL bReadOK = ReadFile(hDB, DB_Buffer, dwFileSize, &dwRead, NULL);
 	if (!bReadOK || dwRead != dwFileSize)
 	{
 		DebugString(L"Failed to read file:%d", GetLastError());	
		g_pMemoryMgr->FreeMemory(DB_Buffer);
		bRet = FALSE;
 	}
	//4b �ļ���С MAPƫ�� DIRƫ�� FILEƫ��   =16B
	//4b tag_'����' tag_'����' tag_version dir_size file_size =20B
	//2b time_year time_month time_day time_hour  =8B
	WORD wYear, wMonth, wDay, wHour;

	DWORD *pTag = (DWORD *)DB_Buffer;
	DWORD nDirCount = pTag[7];
	DWORD nFileCount = pTag[8];
	if (0 == nDirCount || dwFileSize != *pTag || pTag[1] < (16 + 20 + 8 + 26) || pTag[2] - pTag[1] != (nDirCount << 2) || pTag[4] != '����' || pTag[5] != '����' || pTag[6] != 0x01000001)
	{
		g_pMemoryMgr->FreeMemory(DB_Buffer);
		bRet = FALSE;
	}
	PBYTE pByte = PBYTE(pTag + 9);
	wYear = *(WORD*)pByte;
	pByte += 2;
	wMonth = *(WORD*)pByte;
	pByte += 2;
	wDay = *(WORD*)pByte;
	pByte += 2;
	wHour = *(WORD*)pByte; 
	pByte += 2;

	return bRet;
}
