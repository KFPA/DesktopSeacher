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
	*	Function:
	*      �������ݿ�,�����������ݽṹ���������غ͸���
	*	Parameter(s):
	*
	*	Return:
	*      ���ɹ����أ�����TRUE;�����ʾ��Ҫ����ɨ�����ݿ�
	*	Commons:
	**/
	BOOL LoadDatabase(HWND hMainWnd);
	DWORD InitSeachProc(PVOID pParam);
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
	SArray<HANDLE> m_arrHandle; //����A~Z�Ķ�Ӧ�ľ��� -'A'��ȡ
	SArray<DWORD>  m_arrBytesPerCluster; //����ÿ�����̶�Ӧ��ÿ���ֽ���
	SArray<DWORD>  m_arrFileRecSize; //����ÿ�����̶�Ӧ��MFT�ļ���¼�Ĵ�С
	SArray<PBYTE>  m_arrOutBuffer; //ÿ�����̵��ļ����¼��ַ����ʼʱ���䣬����ʱȡ��
	SArray<DWORDLONG> m_arrJournalID; //����ÿ�����̵�journalID
	SArray<USN>    m_arrFirstUSN; //����ÿ�����̵ĵ�һ��USN
	SArray<USN>    m_arrNextUSN; //
};

