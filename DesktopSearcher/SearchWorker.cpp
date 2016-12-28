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
	
	DWORD dwDri; //卷号 0~25
	
	int iFilterRoot = 0;

	for (WCHAR* pDri = tDrivers; *pDri != '\0';pDri += 4)
	{
		if (*pDri >= 'a')
		{
			*pDri -= 32; //转换为大写
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
				//////////获取驱动盘句柄/////////////
				HANDLE hVolume = Usntools::GetDriversHandle(pDri);
				if (INVALID_HANDLE_VALUE == hVolume)
				{
					SStringT strContent;
					strContent.Format(GETSTRING(L"@string/volerror"), *pDri, GetLastError());
					DebugString(strContent);
				}

				m_arrHandle.SetAt(dwDri, hVolume);  //保存句柄

				//////////创建USN日志/////////////
				NTFS_VOLUME_DATA_BUFFER ntfsVolData;
				Usntools::CreateUsnJournal(hVolume, ntfsVolData);

				m_arrBytesPerCluster.SetAt(dwDri, ntfsVolData.BytesPerCluster);
				m_arrFileRecSize.SetAt(dwDri, sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) - 1 + ntfsVolData.BytesPerFileRecordSegment);
			
				//////////查询USN日志/////////////
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
	//4b 文件大小 MAP偏移 DIR偏移 FILE偏移   =16B
	//4b tag_'吴盼' tag_'盼吴' tag_version dir_size file_size =20B
	//2b time_year time_month time_day time_hour  =8B
	WORD wYear, wMonth, wDay, wHour;

	DWORD *pTag = (DWORD *)DB_Buffer;
	DWORD nDirCount = pTag[7];
	DWORD nFileCount = pTag[8];
	if (0 == nDirCount || dwFileSize != *pTag || pTag[1] < (16 + 20 + 8 + 26) || pTag[2] - pTag[1] != (nDirCount << 2) || pTag[4] != '吴盼' || pTag[5] != '盼吴' || pTag[6] != 0x01000001)
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

	//检查Journal ID
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
						//Journal ID已经改变
						bRet = FALSE;
						break;
					}
					pByte += 8;
					usnLast[dwDri] = *(USN*)pByte; //修改要读的USN起点
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
		if (m_arrHandle.GetAt(dwDri))//是NTFS且信息已获取
		{
			// 枚举USN
			MFT_ENUM_DATA med;
			med.StartFileReferenceNumber = 0;
			med.LowUsn = 0;
			med.HighUsn = m_arrNextUSN.GetAt(dwDri);

			DWORD dwMemRecord;//记录所占空间
			
			PNORMALFILE_RECORD pFile;
			PDIRECTORY_RECORD pDir;
			
			{//先把根加进去
				//C:  D: ... ROOT=0x000000000005 Parent
				//计算所需空间
				dwMemRecord = GetDirectoryRecordLength(2);
				//分配空间
				pDir = (PDIRECTORY_RECORD)g_MemDir.PushBack(dwMemRecord);
				//填充数据
				pDir->BasicInfo = GetBasicInfo(dwDri, 5);
				pDir->ParentBasicInfo = 0;
				pDir->NameLength = 2;
				pDir->Name[0] = (BYTE)(dwDri + 'A');
				pDir->Name[1] = ':';
				//设置索引
				g_vDirIndex.push_back((IndexElemType)pDir);
				g_DirMap.push_back(GetBasicInfo(dwDri, 5), (IndexElemType)pDir);
			}

			DWORD dwLastBasic = 0;
			DWORD cbRet;
			PUSN_RECORD pRecord, pEnd;
			PWCHAR pFileName;
			HANDLE hVolume = m_arrHandle.GetAt(dwDri);
			int codeNameLen;    //文件名占内存大小
			BYTE szFileNameCode[768];//最多需要255*3个字节<768

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
						int idExt = 0;//扩展名ID
						DWORD dwExtIdLen;//扩展名所占用字节数  
						DWORD dwIconLen = 0;
						for (i = iLen - 1; i >= 0 && pFileName[i] != L' '&&pFileName[i] != L'.'; --i);
						if (i <= 0 || pFileName[i] == L' '){
							dwExtIdLen = 0;//无扩展名 注意，如果文件名以.开头 后接非. 空格字符，不认为是扩展名 此时i==0
						}
						else{//
							idExt = g_ExtMgr.insert(pFileName + i + 1, iLen - 1 - i);
							if (idExt<CExtArray::s_dwOmitExt) dwIconLen = 4;
							dwExtIdLen = GetExtIdLength(idExt);
							iLen = i;
						}

						//iLen为UCS2文件名长 去.扩展名
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
	//利用g_DirMap 或者 g_vDirIndex皆可
	//程序中统一使用g_DirMap增强代码可读性
	{
		PDIRECTORY_RECORD pDir;
		PNORMALFILE_RECORD pFile;

		int i;
		PIndexElemType  pData, pDataEnd;
		PINDEX_BLOCK_NODE   *pIndex;
		PINDEX_BLOCK_NODE   pNode;
		int cBlock;

		//先构建目录部分
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
					else//其父未被索引，例如VISTA WIN7下 \$Extend\$RmMetadata
					{
						g_vDirIndex.DecreaseSize();
						//在索引中删除该项
						--pDataEnd;
						for (PIndexElemType p = pData; p < pDataEnd; ++p)
						{
							*p = *(p + 1);
						}
						pNode->CurrentEnd -= 1;
						--pData;
					}
				}
				//为0时为根目录，不处理
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
				else//其父未被索引
				{
					g_vFileIndex.DecreaseSize();
					//在索引中删除该项
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
		//拷贝索引到pTempByte
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
		//对pTempHead[0,...,nDirSize-1]进行排序
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
