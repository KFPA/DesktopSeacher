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
	*      �򿪸�NTFS������ȡ�����Ϣ
	*	Parameter(s):
	*
	*	Return:
	*
	*	Commons:
	**/
	BOOL OpenNtfsVolume();
	/**
	*   Function:
	*     �жϴ����Ƿ�ΪNTFS��
	*     Parameter(s):
	*	  WCHAR cDriName  ��������
	*/
	BOOL IsNTFS(char cDriName);
	/**
	*   Function:
	*     ��ʼ�����ش���
	*/
	void InitDrives();
public:
	char vol[26];
	SMap<DWORD,HANDLE> mapHandle; //����A~Z�Ķ�Ӧ�ľ��� -'A'��ȡ
	SMap<DWORD, DWORD>  mapBytesPerCluster; //����ÿ�����̶�Ӧ��ÿ���ֽ���
	SMap<DWORD, DWORD>  mapFileRecSize; //����ÿ�����̶�Ӧ��MFT�ļ���¼�Ĵ�С
	SMap<DWORD, PBYTE>  mapOutBuffer; //ÿ�����̵��ļ����¼��ַ����ʼʱ���䣬����ʱȡ��
	SMap<DWORD, DWORDLONG> mapJournalID; //����ÿ�����̵�journalID
	SMap<DWORD, USN>    mapFirstUSN; //����ÿ�����̵ĵ�һ��USN
	SMap<DWORD, USN>    mapNextUSN; //
};

