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

				m_arrHandle.SetAt(dwDri, hVolume);  //������

				//////////����USN��־/////////////
				NTFS_VOLUME_DATA_BUFFER ntfsVolData;
				Usntools::CreateUsnJournal(hVolume, ntfsVolData);

				m_arrBytesPerCluster.SetAt(dwDri, ntfsVolData.BytesPerCluster);
				m_arrFileRecSize.SetAt(dwDri, sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) - 1 + ntfsVolData.BytesPerFileRecordSegment);
			
				//////////��ѯUSN��־/////////////
				USN_JOURNAL_DATA usnJournalData;
				Usntools::QueryUsnJournal(hVolume, &usnJournalData);
				m_arrJournalID.SetAt(dwDri, usnJournalData.UsnJournalID);
				m_arrFirstUSN.SetAt(dwDri, usnJournalData.FirstUsn);
				m_arrNextUSN.SetAt(dwDri, usnJournalData.NextUsn);
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

	//���Journal ID
	{
		SASSERT(bRet);
		USN usnLast[26] = { 0 };
		{
			for (DWORD dwDri = 0; dwDri < 26; dwDri++)
			{
				if (*pByte++)
				{
					if (*(DWORDLONG*)pByte != m_arrJournalID.GetAt(dwDri))
					{
						//Journal ID�Ѿ��ı�
						bRet = FALSE;
						break;
					}
					pByte += 8;
					usnLast[dwDri] = *(USN*)pByte; //�޸�Ҫ����USN���
					if (usnLast[dwDri] < m_arrFirstUSN.GetAt(dwDri))
					{
						bRet = FALSE;
						break;
					}
					pByte += 8;
				}
				else
				{
					if (m_arrJournalID.GetAt(dwDri))
					{
						bRet = FALSE;
						break;
					}
				}
			}
		}
		if (!bRet)
		{
			DebugString(L"Your database needs to update! time:%d,%d,%d,%d",wYear, wMonth, wDay, wHour);
		}
	}

	return bRet;
}

DWORD CSearchWorker::InitSeachProc(PVOID pParam)
{
	HWND hMainWnd = (HWND)pParam;
	BYTE RecvBuffer[sizeof(DWORDLONG) + 0x80000];
	for (DWORD dwDri = 0; dwDri < 26;dwDri++)
	{
		if (m_arrHandle.GetAt(dwDri))//��NTFS����Ϣ�ѻ�ȡ
		{
			// ö��USN
			MFT_ENUM_DATA med;
			med.StartFileReferenceNumber = 0;
			med.LowUsn = 0;
			med.HighUsn = m_arrNextUSN.GetAt(dwDri);

			DWORD dwMemRecord;//��¼��ռ�ռ�
			
			PNORMALFILE_RECORD pFile;
			PDIRECTORY_RECORD pDir;
			
			{//�ȰѸ��ӽ�ȥ
				//C:  D: ... ROOT=0x000000000005 Parent
				//��������ռ�
				dwMemRecord = GetDirectoryRecordLength(2);
				//����ռ�
				pDir = (PDIRECTORY_RECORD)g_MemDir.PushBack(dwMemRecord);
				//�������
				pDir->BasicInfo = GetBasicInfo(dwDri, 5);
				pDir->ParentBasicInfo = 0;
				pDir->NameLength = 2;
				pDir->Name[0] = (BYTE)(dwDri + 'A');
				pDir->Name[1] = ':';
				//��������
				g_vDirIndex.push_back((IndexElemType)pDir);
				g_DirMap.push_back(GetBasicInfo(dwDri, 5), (IndexElemType)pDir);
			}

			DWORD dwLastBasic = 0;
			DWORD cbRet;
			PUSN_RECORD pRecord, pEnd;
			PWCHAR pFileName;
			HANDLE hVolume = m_arrHandle.GetAt(dwDri);
			int codeNameLen;    //�ļ���ռ�ڴ��С
			BYTE szFileNameCode[768];//�����Ҫ255*3���ֽ�<768

			while (DeviceIoControl(hVolume, FSCTL_ENUM_USN_DATA,
				&med, sizeof(med),
				RecvBuffer, sizeof(RecvBuffer), &cbRet,
				NULL)
				)
			{
				for (pRecord = (PUSN_RECORD)&RecvBuffer[sizeof(USN)], pEnd = PUSN_RECORD(RecvBuffer + cbRet);
					pRecord < pEnd;
					pRecord = (PUSN_RECORD)((PBYTE)pRecord + pRecord->RecordLength)
					)
				{
					pFileName = pRecord->FileName;
					int i, iLen = pRecord->FileNameLength >> 1;
					if (pRecord->FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
					{
						codeNameLen = tools::Ucs2ToCode(szFileNameCode, pFileName, iLen);
						dwMemRecord = GetDirectoryRecordLength(codeNameLen);
						pDir = (PDIRECTORY_RECORD)g_MemDir.PushBack(dwMemRecord);
						pDir->InitializeData(dwDri
							, pRecord->FileReferenceNumber
							, pRecord->ParentFileReferenceNumber
							, szFileNameCode, codeNameLen
							, codeNameLen>iLen
							);
						g_vDirIndex.push_back((IndexElemType)pDir);
						g_DirMap.push_back(GetBasicInfo(dwDri, pRecord->FileReferenceNumber), (IndexElemType)pDir);
					}
					else
					{
						int idExt = 0;//��չ��ID
						DWORD dwExtIdLen;//��չ����ռ���ֽ���  
						DWORD dwIconLen = 0;
						for (i = iLen - 1; i >= 0 && pFileName[i] != L' '&&pFileName[i] != L'.'; --i);
						if (i <= 0 || pFileName[i] == L' '){
							dwExtIdLen = 0;//����չ�� ע�⣬����ļ�����.��ͷ ��ӷ�. �ո��ַ�������Ϊ����չ�� ��ʱi==0
						}
						else{//
							idExt = g_ExtMgr.insert(pFileName + i + 1, iLen - 1 - i);
							if (idExt<CExtArray::s_dwOmitExt) dwIconLen = 4;
							dwExtIdLen = GetExtIdLength(idExt);
							iLen = i;
						}

						//iLenΪUCS2�ļ����� ȥ.��չ��
						codeNameLen = tools::Ucs2ToCode(szFileNameCode, pFileName, iLen);
						DWORD dwNameLenLength = GetNameLenLength(codeNameLen);
						dwMemRecord = GetNormalFileRecordLength(codeNameLen, dwNameLenLength, dwExtIdLen, dwIconLen);
						pFile = (PNORMALFILE_RECORD)g_MemFile.PushBack(dwMemRecord);
						pFile->InitializeData(codeNameLen>iLen, dwNameLenLength, dwExtIdLen
							, pRecord->FileReferenceNumber
							, dwDri, pRecord->ParentFileReferenceNumber
							, szFileNameCode, codeNameLen
							, idExt
							);
						if (pFile->BasicInfo == 0x18030d74)
						{
							int jjj = 0;
							++jjj;
						}
						g_vFileIndex.push_back((IndexElemType)pFile);
					}
				}
				med.StartFileReferenceNumber = *(DWORDLONG*)RecvBuffer;
			}
		}
	}

	//Building a database
	//����g_DirMap ���� g_vDirIndex�Կ�
	//������ͳһʹ��g_DirMap��ǿ����ɶ���
	{
		PDIRECTORY_RECORD pDir;
		PNORMALFILE_RECORD pFile;

		int i;
		PIndexElemType  pData, pDataEnd;
		PINDEX_BLOCK_NODE   *pIndex;
		PINDEX_BLOCK_NODE   pNode;
		int cBlock;

		//�ȹ���Ŀ¼����
		cBlock = g_vDirIndex.GetBlockCount();
		pIndex = g_vDirIndex.GetBlockIndex();


		for (i = 0; i < cBlock; ++i)
		{
			pNode = pIndex[i];
			pData = pNode->PtrData + pNode->CurrentBegin;
			pDataEnd = pNode->PtrData + pNode->CurrentEnd;
			for (; pData < pDataEnd; ++pData)
			{
				pDir = PDIRECTORY_RECORD(*pData);
				if (pDir->ParentBasicInfo)
				{
					PDIRECTORY_RECORD* ppDir = (PDIRECTORY_RECORD*)g_DirMap.find(pDir->ParentBasicInfo);
					if (ppDir) pDir->ParentPtr = *(ppDir + 1);
					else//�丸δ������������VISTA WIN7�� \$Extend\$RmMetadata
					{
						g_vDirIndex.DecreaseSize();
						//��������ɾ������
						--pDataEnd;
						for (PIndexElemType p = pData; p < pDataEnd; ++p)
						{
							*p = *(p + 1);
						}
						pNode->CurrentEnd -= 1;
						--pData;
					}
				}
				//Ϊ0ʱΪ��Ŀ¼��������
			}
		}

	/*	cBlock = g_vFileIndex.GetBlockCount();
		pIndex = g_vFileIndex.GetBlockIndex();
		for (i = 0; i < cBlock; ++i)
		{
			pNode = pIndex[i];
			pData = pNode->PtrData + pNode->CurrentBegin;
			pDataEnd = pNode->PtrData + pNode->CurrentEnd;
			for (; pData < pDataEnd; ++pData){
				pFile = PNORMALFILE_RECORD(*pData);
				PDIRECTORY_RECORD* ppDir = (PDIRECTORY_RECORD*)g_DirMap.find(pFile->ParentBasicInfo);
				if (ppDir) pFile->ParentPtr = *(ppDir + 1);
				else//�丸δ������
				{
					g_vFileIndex.DecreaseSize();
					//��������ɾ������
					--pDataEnd;
					for (PIndexElemType p = pData; p < pDataEnd; ++p)
					{
						*p = *(p + 1);
					}
					pNode->CurrentEnd -= 1;
					--pData;
				}
			}
		}*/
	}

	DWORD nDirSize = g_vDirIndex.size();
	DWORD nFileSize = g_vFileIndex.size();
	PIndexElemType pTempHead, pTempByte;;
	pTempHead = (PIndexElemType)g_pMemoryMgr->GetMemory(sizeof(IndexElemType)*(nFileSize>nDirSize ? nFileSize : nDirSize));
	
	//Sorting folder
	{
		//����������pTempByte
		pTempByte = pTempHead;
		int i;
		IndexElemType *pData, *pDataEnd;
		PINDEX_BLOCK_NODE   *pIndex;
		PINDEX_BLOCK_NODE   pNode;
		int cBlock;
		cBlock = g_vDirIndex.GetBlockCount();
		pIndex = g_vDirIndex.GetBlockIndex();

		for (i = 0; i < cBlock; ++i)
		{
			pNode = pIndex[i];
			pData = pNode->PtrData + pNode->CurrentBegin;
			pDataEnd = pNode->PtrData + pNode->CurrentEnd;
			for (; pData < pDataEnd; ++pData)
			{
				*pTempByte++ = *pData;
			}
		}
		//��pTempHead[0,...,nDirSize-1]��������
#ifdef MS_QSORT
		qsort(pTempHead, nDirSize, 4, pcomp_dir);
#else
		name_qsort(pTempHead, nDirSize, comp_dir);
#endif
		pTempByte = pTempHead;
		for (i = 0; i < cBlock; ++i)
		{
			pNode = pIndex[i];
			pData = pNode->PtrData + pNode->CurrentBegin;
			pDataEnd = pNode->PtrData + pNode->CurrentEnd;
			for (; pData < pDataEnd; ++pData)
			{
				*pData = *pTempByte++;
			}
		}
	}
	 {
		 pTempByte = pTempHead;
		 int i;
		 PIndexElemType pData, pDataEnd;
		 PINDEX_BLOCK_NODE   *pIndex;
		 PINDEX_BLOCK_NODE   pNode;
		 int cBlock;
		 cBlock = g_vFileIndex.GetBlockCount();
		 pIndex = g_vFileIndex.GetBlockIndex();
		 for (i = 0; i < cBlock; ++i)
		 {
			 pNode = pIndex[i];
			 pData = pNode->PtrData + pNode->CurrentBegin;
			 pDataEnd = pNode->PtrData + pNode->CurrentEnd;
			 for (; pData < pDataEnd; ++pData){
				 *pTempByte++ = *pData;
			 }
		 }
#ifdef MS_QSORT
		 qsort(pTempHead, nFileSize, 4, pcomp_file);
#else
		 name_qsort(pTempHead, nFileSize, comp_file);
#endif
		 pTempByte = pTempHead;
		 for (i = 0; i < cBlock; ++i)
		 {
			 pNode = pIndex[i];
			 pData = pNode->PtrData + pNode->CurrentBegin;
			 pDataEnd = pNode->PtrData + pNode->CurrentEnd;
			 for (; pData < pDataEnd; ++pData)
			 {
				 *pData = *pTempByte++;
			 }
		 }
	 }
	 g_pMemoryMgr->FreeMemory((PBYTE)pTempHead);
	 return 0;
}
