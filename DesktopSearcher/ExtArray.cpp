#include "stdafx.h"
#include "ExtArray.h"




//��ʼ����չ���б�
//���б��е���չ������Ӧ��ͼ�겻Ψһ
//����洢����ʵʱ����

PWCHAR g_pOmitExt[]=
{
    L"cur"
    ,L"exe"
    ,L"ico"
    ,L"lnk"
};

// PWCHAR g_pInitExt[]=
// {
//     L"avi"
//     ,L"bin"
//     ,L"bmp"
//     ,L"chm"
//     ,L"cpp"
//     ,L"cur"
//     ,L"dat"
//     ,L"dll"
//     ,L"exe"
//     ,L"gif"
//     ,L"htm"
//     ,L"html"
//     ,L"ico"
//     ,L"ime"
//     ,L"ini"
//     ,L"jar"
//     ,L"java"
//     ,L"jpeg"
//     ,L"jpg"
//     ,L"js"
//     ,L"jsp"
//     ,L"kdh"     //CAJViewr ͬ��֪��
//     ,L"key"     //ע���
//     ,L"krc"     //����ļ�
//     ,L"lib"
//     ,L"lnk"     //��ݷ�ʽ
//     ,L"log"
//     ,L"m"
//     ,L"mdb"
//     ,L"mp3"
//     ,L"ncb"
//     ,L"nh"
//     ,L"o"
//     ,L"obj"
//     ,L"ocx"
//     ,L"opt"
//     ,L"p"       //matlab
//     ,L"pdf"
//     ,L"pdg"
//     ,L"png"
//     ,L"ppt"
//     ,L"qpyd"
//     ,L"rar"     //ѹ���ļ�
//     ,L"rc"
//     ,L"rc2"
//     ,L"reg"
//     ,L"resx"
//     ,L"skn"
//     ,L"sln"
//     ,L"suo"
//     ,L"swf"
//     ,L"sys"
//     ,L"tmp"
//     ,L"txt"
//     ,L"url"
//     ,L"vb"
//     ,L"vbs"
//     ,L"vcproj"
//     ,L"wav"
//     ,L"wmf"
//     ,L"xls"
//     ,L"xml"
//     ,L"xsl"
//     ,L"zip"
// };

//ʵʱ����ͼ�����
DWORD CExtArray::s_dwOmitExt=sizeof(g_pOmitExt)/sizeof(*g_pOmitExt);

CExtArray::CExtArray()
{
    m_size=0;
    m_dwMax=512;
    m_piIcon=(int*)g_pMemoryMgr->malloc(sizeof(int)*m_dwMax);

	m_vpExtName = (PWCHAR*)g_pMemoryMgr->malloc(sizeof(PWCHAR)*m_dwMax);
    
	m_vpIndexExtName = (PINDEXNODE)g_pMemoryMgr->malloc(sizeof(INDEXNODE)*m_dwMax);

    m_bInitReal=FALSE;
}

CExtArray::~CExtArray()
{
    if(!m_bInitReal)
    {//��̬�����
        for(size_t i=0;i<s_dwOmitExt;++i){
			g_pMemoryMgr->free(m_vpExtName[i]);
        }
    }
	for (size_t i = s_dwOmitExt; i<m_size; ++i){
		g_pMemoryMgr->free(m_vpExtName[i]);
    }
	g_pMemoryMgr->free(m_vpExtName);
	g_pMemoryMgr->free(m_piIcon);
	g_pMemoryMgr->free(m_vpIndexExtName);
}

//��ʼ��ʵʱICON��չ��
void CExtArray::InitRealTimeCallExt()
{   
    m_bInitReal=TRUE;
	for (size_t id = 0; id<s_dwOmitExt; ++id)
    {
        m_vpExtName[id]=g_pOmitExt[id];
        m_piIcon[id]=ICON_INDEX_REALTIMECALL;
        m_vpIndexExtName[id].pExtName=g_pOmitExt[id];//
        m_vpIndexExtName[id].idExtName=id;
    }
    m_size+=s_dwOmitExt;
}

//�����ļ���ʱ��Ҫ������չ��
//�����ظ���չ������Ӧ��ID
int CExtArray::insert(PWCHAR pszExtName,int ExtNameLen)
{
    int low,high,mid;//���ֶ����������
    int iInsert;//Ҫ�����λ��
    int i,j;//��ʱ����
    for(i=0;i<ExtNameLen;++i)//ת���ļ���ΪСд
    {
        if(pszExtName[i]>=L'A' && pszExtName[i]<=L'Z') pszExtName[i]+=32;
    }

    //��������,�����Ƿ����
    WCHAR wchDest,wch,*pName;
    low=0;high=m_size-1;
    for(i=0;i<ExtNameLen;++i)
    {
        wchDest=pszExtName[i];        
        while(low<=high)
        {
            mid=low+((high-low)>>1);
            wch=m_vpIndexExtName[mid].pExtName[i];
            if(wch>wchDest) high=mid-1;
            else if(wch<wchDest) low=mid+1;
            else{
                for(j=mid-1;j>=low && m_vpIndexExtName[j].pExtName[i]==wchDest;--j);
                if(j>=low) low=j+1;
                for(j=mid+1;j<=high && m_vpIndexExtName[j].pExtName[i]==wchDest;++j);
                if(j<=high) high=j-1;
                break;
            }
        }
        if(low>high)
        {//��i��û���ҵ�ƥ�䣬���ʺ�׺�������
            iInsert=low;//����֤
            break;
        }
    }
 
    //�п��ܴ˴�low<high �����ڶ��ǰ׺ƥ�� low����С
    //ƥ��ɹ�
    if(i==ExtNameLen){
        if(L'\0'==m_vpIndexExtName[low].pExtName[ExtNameLen])
            return m_vpIndexExtName[low].idExtName;
        iInsert=low;
    }
        
    if(m_size==m_dwMax) //��ǰ�ռ�����
    {
        m_dwMax+=DELT;
		m_vpExtName = (PWCHAR*)g_pMemoryMgr->realloc(m_vpExtName, sizeof(PWCHAR)*m_dwMax);
		m_piIcon = (int*)g_pMemoryMgr->realloc(m_piIcon, sizeof(int)*m_dwMax);

		PINDEXNODE pTemp = (PINDEXNODE)g_pMemoryMgr->malloc(sizeof(INDEXNODE)*m_dwMax);
        CopyMemory(pTemp,m_vpIndexExtName,iInsert*sizeof(INDEXNODE));
        CopyMemory(pTemp+iInsert+1,m_vpIndexExtName+iInsert,(m_size-iInsert)*sizeof(INDEXNODE));
		g_pMemoryMgr->free(m_vpIndexExtName);
        m_vpIndexExtName=pTemp;
    }
    else
    {
        for(j=m_size;j>iInsert;--j)//��������
        {
            m_vpIndexExtName[j]=m_vpIndexExtName[j-1];
        }
    }
	pName = (PWCHAR)g_pMemoryMgr->malloc((sizeof(WCHAR)*(ExtNameLen + 1)));
    pName[ExtNameLen]=L'\0';
    wcsncpy(pName,pszExtName,ExtNameLen);
    m_vpExtName[m_size]=pName;
    m_vpIndexExtName[iInsert].pExtName=pName;
    m_vpIndexExtName[iInsert].idExtName=m_size;             
    m_piIcon[m_size]=ICON_INDEX_UNINITIALIZED;
    ++m_size;  
    return m_size-1;   
}



//�����ļ���ʱ��Ҫ������չ��
//�����ظ���չ������Ӧ��ID
int CExtArray::find(PWCHAR pszExtName,int ExtNameLen)const
{
    int low,high,mid;//���ֶ����������
    int i,j;//��ʱ����
    for(i=0;i<ExtNameLen;++i)//ת���ļ���ΪСд
    {
        if(pszExtName[i]>=L'A' && pszExtName[i]<=L'Z') pszExtName[i]+=32;
    }

    //��������,�����Ƿ����
    WCHAR wchDest,wch;
    low=0;high=m_size-1;
    for(i=0;i<ExtNameLen;++i)
    {
        wchDest=pszExtName[i];        
        while(low<=high)
        {
            mid=low+((high-low)>>1);
            wch=m_vpIndexExtName[mid].pExtName[i];
            if(wch>wchDest) high=mid-1;
            else if(wch<wchDest) low=mid+1;
            else{
                for(j=mid-1;j>=low && m_vpIndexExtName[j].pExtName[i]==wchDest;--j);
                if(j>=low) low=j+1;
                for(j=mid+1;j<=high && m_vpIndexExtName[j].pExtName[i]==wchDest;++j);
                if(j<=high) high=j-1;
                break;
            }
        }
        if(low>high)
        {//��i��û���ҵ�ƥ�䣬���ʺ�׺�������
            break;
        }
    }

    //�п��ܴ˴�low<high �����ڶ��ǰ׺ƥ�� low����С
    //ƥ��ɹ�
    if(i==ExtNameLen){
        if(L'\0'==m_vpIndexExtName[low].pExtName[ExtNameLen]){
            return m_vpIndexExtName[low].idExtName;
        }
    }
    return -1;
}


//��ü�����չ����ICON����
//������>-2 ��ֱ��ʹ��
//==-2 ��Ҫ�û�ʵʱ��ȡ
//==-3 ������û�г�ʼ����id ,�û���Ҫ����SetIconIndex����ʼ����չ������
int CExtArray::GetIconIndex(int idExtName)const
{
	size_t idExtNameSize_t = idExtName;
	assert(idExtNameSize_t >= 0 && idExtNameSize_t<m_size &&"GetIconIndex");
	return m_piIcon[idExtNameSize_t];
}

int CExtArray::Compare(int idExt1,int idExt2)const
{
	size_t idExt1size_t = idExt1;
	size_t idExt2size_t = idExt2;
	assert(idExt1size_t >= 0 && idExt1size_t<m_size && idExt2size_t >= 0 && idExt2size_t<m_size);
	PWCHAR str1 = m_vpExtName[idExt1size_t], str2 = m_vpExtName[idExt2size_t];
    int ret=0 ;
    while(!(ret=(int)(*str1-*str2))&&*str2)
        ++str1,++str2;
    if(ret<0)
        ret=-1 ;
    else if(ret>0)
        ret=1 ;
    return(ret);
}

PWCHAR CExtArray::GetExtName(int idExtName)const
{
	size_t idExtNamesize_t = idExtName;
	assert(idExtNamesize_t >= 0 && idExtNamesize_t<m_size &&"GetExtName");
	return m_vpExtName[idExtNamesize_t];
}

//
int CExtArray::GetExtIndexSequence(int idExtName)const//��ȡ��չ���洢IDλ������ID�ĵڼ���
{
	size_t idExtNamesize_t = idExtName;
	if (idExtNamesize_t<0 || idExtNamesize_t >= m_size) return -1;
	return this->find(m_vpExtName[idExtNamesize_t]);
}


//idExtNameΪ��չ��ID
//pFilePathΪ��б��Ŀ¼��
//filePathLenΪ��б��Ŀ¼������
//pFileName�ļ���
//fileLen�ļ�������
int CExtArray::SetIconIndex(int idExtName,PWCHAR pFilePath,int filePathLen,PWCHAR pFileName,int fileLen)
{
	size_t idExtNamesize_t = idExtName;
	assert(idExtNamesize_t >= s_dwOmitExt && "�Ѿ���ʼ��ʵʱ���õĲ����ٴ��ݽ���");

    SHFILEINFOW sfi;

    //�ֽ�filePath��չΪ�ļ���
    int j=filePathLen;
    for(int i=0;i<fileLen;++i)
    {
        pFilePath[j++]=pFileName[i];
    }
    pFilePath[j]=L'\0';

    SHGetFileInfoW(pFilePath,  
        FILE_ATTRIBUTE_NORMAL,  
        &sfi,  
        sizeof(sfi),  
        SHGFI_SMALLICON
        | SHGFI_SYSICONINDEX 
        | SHGFI_USEFILEATTRIBUTES
        ) ;
    pFilePath[filePathLen]=L'\0';//��ԭfilePath
	m_piIcon[idExtNamesize_t] = sfi.iIcon;
    return sfi.iIcon;
}


//��չ�����ݿ���������ȼ���
//������FALSE ���ļ������ݿ��е���չ��������Ч
//��ʱ��Ҫ���������ļ������ݿ�
BOOL CExtArray::LoadFromFile(PWCHAR pwszExtDatabase)
{
    HANDLE hFile=CreateFileW(pwszExtDatabase
        ,GENERIC_READ
        ,FILE_SHARE_READ
        ,NULL
        ,OPEN_EXISTING
        ,FILE_ATTRIBUTE_NORMAL
        ,NULL);
    if(INVALID_HANDLE_VALUE==hFile){
        DebugString(L"����չ���ļ�ʧ��:%d",GetLastError());
        return FALSE;
    }
    DWORD dwFileSize=GetFileSize(hFile,NULL);
	PBYTE pByte = g_pMemoryMgr->GetMemory(dwFileSize), pEnd = pByte + dwFileSize;
    DWORD dwRead;
    ReadFile(hFile,pByte,dwFileSize,&dwRead,NULL);
    assert(dwFileSize==dwRead);
    m_size=*(DWORD*)(pByte);
    if(m_size>=m_dwMax)
    {
        m_dwMax=m_size+DELT;
		m_vpExtName = (PWCHAR*)g_pMemoryMgr->realloc(m_vpExtName, sizeof(PWCHAR)*m_dwMax);
		m_piIcon = (int*)g_pMemoryMgr->realloc(m_piIcon, sizeof(int)*m_dwMax);
		m_vpIndexExtName = (PINDEXNODE)g_pMemoryMgr->realloc(m_vpIndexExtName, sizeof(INDEXNODE)*m_dwMax);
    }
    size_t extLen;
	size_t i = 0, j;
    int idExt;
    PWCHAR pName;
    for(pByte+=4;pByte<pEnd;)
    {
         idExt=*(int*)pByte;
         m_vpIndexExtName[i].idExtName=idExt;
         pByte+=4;
         m_piIcon[idExt]=*(int*)pByte;
         pName=PWCHAR(pByte+4);
         extLen=wcslen(pName);
		 m_vpExtName[idExt] = (PWCHAR)g_pMemoryMgr->malloc((sizeof(WCHAR)*(extLen + 1)));
         m_vpIndexExtName[i].pExtName=m_vpExtName[idExt];
         for(j=0;j<extLen;++j){
            m_vpExtName[idExt][j]=*pName++;
         }
         m_vpExtName[idExt][j]=L'\0';
         pByte=PBYTE(pName+1);   
         i++;
    }
    assert(i==m_size);
    CloseHandle(hFile);
    return TRUE;
}

//��д�ļ������ݿ�֮��д����չ�����ݿ�
//��ʽ��IDICON��L'\0'
BOOL CExtArray::WriteToFile(PWCHAR pwszExtDatabase)
{
    HANDLE hFile=CreateFileW(pwszExtDatabase
        ,GENERIC_READ|GENERIC_WRITE
        ,FILE_SHARE_READ|FILE_SHARE_WRITE
        ,NULL
        ,CREATE_ALWAYS
        ,FILE_ATTRIBUTE_NORMAL
        ,NULL);
    if(INVALID_HANDLE_VALUE==hFile){
        DebugString(L"д����չ���ļ�ʧ��:%d",GetLastError());
        return FALSE;
    }
    //����չ����ĸ˳��д��
    DWORD dwWrited;
    char buf[1024];
    int len;
    int *pId=(int*)buf;
    int *pIcon=(int*)(buf+4);
    PWCHAR pExtName=(PWCHAR)(buf+8),pSrc;

    WriteFile(hFile,&m_size,sizeof(m_size),&dwWrited,NULL);//д����չ������
    for(size_t i=0;i<m_size;++i)
    {      
        *pId=m_vpIndexExtName[i].idExtName;
        *pIcon=ICON_INDEX_UNINITIALIZED;
        pSrc=m_vpExtName[*pId];
        for(len=0;*pSrc;++pSrc)
        {
            pExtName[len++]=*pSrc;
        }
        pExtName[len++]=L'\0';
        len=(len<<1)+8;
        WriteFile(hFile,buf,len,&dwWrited,NULL);
        assert(len==dwWrited);
    }
    CloseHandle(hFile);
    return FALSE;
}


//����-1��ʾû���ҵ�
//��չ����L'\0'����
int  CExtArray::find(PWCHAR pszExtName)const
{
        int low,high,mid;//���ֶ����������
        int i,j;//��ʱ����

        //��������,�����Ƿ����
        WCHAR wchDest,wch;
        low=0;high=m_size-1;

        i=-1;
        for(;*pszExtName!=L'\0';++pszExtName)
        {
            ++i;
            wchDest=*pszExtName;        
            while(low<=high)
            {
                mid=low+((high-low)>>1);
                wch=m_vpIndexExtName[mid].pExtName[i];
                if(wch>wchDest) high=mid-1;
                else if(wch<wchDest) low=mid+1;
                else{
                    for(j=mid-1;j>=low && m_vpIndexExtName[j].pExtName[i]==wchDest;--j);
                    if(j>=low) low=j+1;
                    for(j=mid+1;j<=high && m_vpIndexExtName[j].pExtName[i]==wchDest;++j);
                    if(j<=high) high=j-1;
                    break;
                }
            }
            if(low>high)
            {//��i��û���ҵ�ƥ�䣬���ʺ�׺�������
                return -1;
            }
        }

        //�п��ܴ˴�low<high �����ڶ��ǰ׺ƥ�� low����С
        //ƥ��ɹ�
        if(*pszExtName==L'\0'){
            if(L'\0'==m_vpIndexExtName[low].pExtName[i])
                return low;
        }
        return -1;
}
    