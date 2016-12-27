#include "souistd.h"
#include "event/NotifyCenter.h"

namespace SOUI{

template<> SNotifyCenter * SSingleton<SNotifyCenter>::ms_Singleton = 0;

//////////////////////////////////////////////////////////////////////////
class SNotifyReceiver:public CSimpleWnd
{
public:
	enum{
		UM_NOTIFYEVENT = (WM_USER+1000)
	};

	SNotifyReceiver(INotifyCallback * pCallback) :m_pCallback(pCallback)
	{
		SASSERT(m_pCallback);
	}

	~SNotifyReceiver()
	{

	}

	LRESULT OnNotifyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

	BEGIN_MSG_MAP_EX(SNotifyReceiver)
		MESSAGE_HANDLER_EX(UM_NOTIFYEVENT, OnNotifyEvent)
	END_MSG_MAP()

protected:
	INotifyCallback * m_pCallback;
};


LRESULT SNotifyReceiver::OnNotifyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	EventArgs *e = (EventArgs*)lParam;
	m_pCallback->OnFireEvent(e);
	e->Release();
	return 0;
}


//////////////////////////////////////////////////////////////////////////
SNotifyCenter::SNotifyCenter(void):m_pReceiver(NULL)
{
	m_dwMainTrdID = GetCurrentThreadId();
	m_pReceiver = new SNotifyReceiver(this);
	m_pReceiver->Create(_T("NotifyReceiver"),WS_POPUP,0,0,0,0,0,HWND_MESSAGE,0);
	SASSERT(m_pReceiver->IsWindow());
}

SNotifyCenter::~SNotifyCenter(void)
{
	m_pReceiver->DestroyWindow();
	delete m_pReceiver;
	m_pReceiver = NULL;
}

void SNotifyCenter::FireEventSync( EventArgs *e )
{
	SASSERT(m_dwMainTrdID == GetCurrentThreadId());
	OnFireEvent(e);
}

//���¼��׵��¼����У�������¼��Ƿ�ע�ᣬִ���¼�ʱ�ټ�顣
void SNotifyCenter::FireEventAsync( EventArgs *e )
{
	e->AddRef();
	m_pReceiver->PostMessage(SNotifyReceiver::UM_NOTIFYEVENT,0,(LPARAM)e);
}


void SNotifyCenter::OnFireEvent( EventArgs *e )
{
	if(!GetEventObject(e->GetID())) return;//ȷ���¼����Ѿ�ע������Ѿ��¼���

	FireEvent(*e);
	if(!e->bubbleUp) return ;

	SPOSITION pos = m_evtHandlerMap.GetTailPosition();
	while(pos)
	{
		ISlotFunctor * pSlot = m_evtHandlerMap.GetPrev(pos);
		(*pSlot)(e);
		if(!e->bubbleUp) break;
	}
}

bool SNotifyCenter::RegisterEventMap( const ISlotFunctor &slot )
{
	for(SPOSITION pos = m_evtHandlerMap.GetHeadPosition();pos;)
	{
		ISlotFunctor * pSlot = m_evtHandlerMap.GetNext(pos);
		if(pSlot->Equal(slot)) return false;
	}
	m_evtHandlerMap.AddTail(slot.Clone());
	return true;
}

bool SNotifyCenter::UnregisterEventMap( const ISlotFunctor &slot )
{
	for(SPOSITION pos = m_evtHandlerMap.GetHeadPosition();pos;)
	{
		SPOSITION posPrev = pos;
		ISlotFunctor * pSlot = m_evtHandlerMap.GetNext(pos);
		if(pSlot->Equal(slot))
		{
			m_evtHandlerMap.RemoveAt(posPrev);
			delete pSlot;
			return true;
		}
	}
	return false;
}

}