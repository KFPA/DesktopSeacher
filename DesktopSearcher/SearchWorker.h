#pragma once
class CSearchWorker : public CSingleton<CSearchWorker>
{
public:
	CSearchWorker();
	~CSearchWorker();
public:
	static CSearchWorker* GetDispatchCenter();
public:
	/**
	*	Function:
	*      打开各NTFS卷，并获取相关信息
	*	Parameter(s):
	*
	*	Return:
	*
	*	Commons:
	**/
	BOOL OpenNtfsVolume();
	/**
	*   Function:
	*     判断磁盘是否为NTFS卷
	*     Parameter(s):
	*	  WCHAR cDriName  磁盘名称
	*/
	BOOL IsNTFS(char cDriName);
	/**
	*   Function:
	*     初始化本地磁盘
	*/
	void InitDrives();
public:
	char vol[26];
	SMap<DWORD,HANDLE> mapHandle; //保存A~Z的对应的卷句柄 -'A'获取
	SMap<DWORD, DWORD>  mapBytesPerCluster; //保存每个磁盘对应的每簇字节数
	SMap<DWORD, DWORD>  mapFileRecSize; //保存每个磁盘对应的MFT文件记录的大小
	SMap<DWORD, PBYTE>  mapOutBuffer; //每个磁盘的文件块记录地址，初始时分配，结束时取消
	SMap<DWORD, DWORDLONG> mapJournalID; //保存每个磁盘的journalID
	SMap<DWORD, USN>    mapFirstUSN; //保存每个磁盘的第一个USN
	SMap<DWORD, USN>    mapNextUSN; //
};

