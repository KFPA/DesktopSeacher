
#pragma once
//���ڴ洢Ŀ¼Basic��Ϣ
//map<BasicInfo,DirPtr> ����Ҫ��õ�i��Ԫ�� �����ۻ�����
//��������BasicInfo��Ӧ��Ŀ¼ʱ DirPtr=NULL
//DWORDLONG:  BasicInfo,DirPtr ��DWORD��BasicInfo
//�洢�ṹ����4�ֽ�����
//����Ŀ¼ʱ��Ҫ����
//��ƴ����������ڽ���std::map���ڴ濪��

typedef IndexBlockNode<DWORDLONG,1024> DirFrnIndexBlockNode,*PDirFrnIndexBlockNode;

class CDirBasicInfoMap
{   
public:
    CDirBasicInfoMap(int _N=32);
    ~CDirBasicInfoMap();

    //���ڳ�ʼ��ʱʹ�ã��Ա����Ч��
    void push_back(DWORD BasicInfo,IndexElemType DirPtr);
    //���Բ��ṩɾ���ڵ㹦��
    //û��ɾ������ϢҲ������Ҫ������
    //�����������õ���ǰ��FRN����
    //��Ϊ��������������ظ�ֵ���DirOffset�������޸�   
    IndexElemType erase(DWORD BasicInfo);//ɾ���ڵ�
    
    void insert(DWORD BasicInfo,IndexElemType DirPtr);

    DWORDLONG* find(DWORD BasicInfo);

    //����˳����������û���������ٶ�
    PDirFrnIndexBlockNode *GetBlockIndex()const
    {
        return m_pIndex;
    }
    int GetBlockCount()const
    {
        return m_dwBlockCount;
    }

protected:
    void AddLastBlock();
    void InsertBlock(int i);//�ڵ�i�������¿�

private:
    PDirFrnIndexBlockNode *m_pIndex;
    DWORD   m_dwMaxCount;//����ָ���������Ŀռ�

    PDirFrnIndexBlockNode m_pLastNode;//DWORD   m_iCurBlock;//���һ���ڵ��
    DWORD   m_dwBlockCount;//�ѷ����˶��ٸ��飨�ѷ���Ŀ�����λ��m_pIndex��ǰm_dwBlockCount�

    DWORD m_size;

};