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
	*	Function:
	*      加载数据库,建立基本数据结构，包括加载和更新
	*	Parameter(s):
	*
	*	Return:
	*      若成功加载，返回TRUE;否则表示需要重新扫面数据库
	*	Commons:
	**/
	BOOL LoadDatabase(HWND hMainWnd);
	DWORD InitSeachProc(PVOID pParam);
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
	SArray<HANDLE> m_arrHandle; //保存A~Z的对应的卷句柄 -'A'获取
	SArray<DWORD>  m_arrBytesPerCluster; //保存每个磁盘对应的每簇字节数
	SArray<DWORD>  m_arrFileRecSize; //保存每个磁盘对应的MFT文件记录的大小
	SArray<PBYTE>  m_arrOutBuffer; //每个磁盘的文件块记录地址，初始时分配，结束时取消
	SArray<DWORDLONG> m_arrJournalID; //保存每个磁盘的journalID
	SArray<USN>    m_arrFirstUSN; //保存每个磁盘的第一个USN
	SArray<USN>    m_arrNextUSN; //
};

