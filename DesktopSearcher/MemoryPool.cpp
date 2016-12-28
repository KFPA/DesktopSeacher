#include "stdafx.h"
#include "MemoryPool.h"



void CMemoryPool::AddLastBlock()
{
    m_pLastBlock=m_ppBlock[m_dwBlockCount]=(PBLOCK)g_pMemoryMgr->GetMemory(sizeof(BLOCK),TRUE);
    m_iLastPos=0;
    ++m_dwBlockCount;
    if(m_dwBlockCount==m_dwMaxCount){//����BUF����������֮
        m_dwMaxCount+=INDEX_COUNT_DELT;
		m_ppBlock = (PBLOCK*)g_pMemoryMgr->realloc(m_ppBlock, m_dwMaxCount*sizeof(PBLOCK));
    }
}

CMemoryPool::CMemoryPool()
{
    m_dwMaxCount=128;
	m_ppBlock = (PBLOCK*)g_pMemoryMgr->malloc(m_dwMaxCount*sizeof(PBLOCK));
	*m_ppBlock = m_pLastBlock = (PBLOCK)g_pMemoryMgr->GetMemory(sizeof(BLOCK), TRUE);
    m_dwBlockCount=1;
    m_iLastPos=0;
}
/*virtual */CMemoryPool::~CMemoryPool()
{
    for(size_t i=0;i<m_dwBlockCount;++i){
		g_pMemoryMgr->FreeMemory((PBYTE)m_ppBlock[i]);
    }
	g_pMemoryMgr->free(m_ppBlock);
}

/**
*	Function:
*      ��ʼ��ʱʹ��
*	Parameter(s):
*
*	Return:	
*
*	Commons:
**/
PVOID CMemoryPool::PushBack(DWORD dwRecordLen)
{
    if(m_iLastPos+dwRecordLen>BLOCK_SIZE)
    {
        AddLastBlock();
    }
    PBYTE pAlloc=(PBYTE)m_pLastBlock+m_iLastPos;
    m_iLastPos+=dwRecordLen;
    return pAlloc;
}

/**
*	Function:
*      ����ά��ʱ�����ڴ�
*	Parameter(s):
*
*	Return:	
*
*	Commons:
*
**/
PVOID CMemoryPool::Alloc(DWORD dwRecordLen)
{
    return PushBack(dwRecordLen);
}

/**
*	Function:
*       ����ά��ʱ�ͷ��ڴ�
*	Parameter(s):
*
*	Return:	
*
*	Commons:
*
**/
void  CMemoryPool::Free(PVOID pRecord,DWORD dwRecordLen)
{

}





