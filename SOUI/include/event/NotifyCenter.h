#pragma once

#include <core/SSingleton.h>

namespace SOUI
{
	template<class T>
	class TAutoEventMapReg
	{
		typedef TAutoEventMapReg<T> _thisClass;
	public:
		TAutoEventMapReg()
		{
			SNotifyCenter::getSingleton().RegisterEventMap(Subscriber(&_thisClass::OnEvent,this));
		}

		~TAutoEventMapReg()
		{
			SNotifyCenter::getSingleton().UnregisterEventMap(Subscriber(&_thisClass::OnEvent,this));
		}

	protected:
		bool OnEvent(EventArgs *e){
			T * pThis = static_cast<T*>(this);
			return !!pThis->_HandleEvent(e);
		}
	};

	class SOUI_EXP SNotifyCenter : public SSingleton<SNotifyCenter>
						, public SEventSet
	{
	public:
		SNotifyCenter(void);
		~SNotifyCenter(void);

        /**
        * FireEventSync
        * @brief    ����һ��ͬ��֪ͨ�¼�
        * @param    EventArgs *e -- �¼�����
        * @return    
        *
        * Describe  ֻ����UI�߳��е���
        */
		void FireEventSync(EventArgs *e);

        /**
        * FireEventAsync
        * @brief    ����һ���첽֪ͨ�¼�
        * @param    EventArgs *e -- �¼�����
        * @return    
        *
        * Describe  �����ڷ�UI�߳��е��ã�EventArgs *e�����ǴӶ��Ϸ�����ڴ棬���ú�ʹ��Release�ͷ����ü���
        */
		void FireEventAsync(EventArgs *e);


        /**
        * RegisterEventMap
        * @brief    ע��һ������֪ͨ�Ķ���
        * @param    const ISlotFunctor &slot -- �¼��������
        * @return    
        *
        * Describe 
        */
		bool RegisterEventMap(const ISlotFunctor &slot);

        /**
        * RegisterEventMap
        * @brief    ע��һ������֪ͨ�Ķ���
        * @param    const ISlotFunctor &slot -- �¼��������
        * @return    
        *
        * Describe 
        */
		bool UnregisterEventMap(const ISlotFunctor & slot);
	protected:
		void OnFireEvent(EventArgs *e);

		void ExecutePendingEvents();

		static VOID CALLBACK OnTimer( HWND hwnd,
			UINT uMsg,
			UINT_PTR idEvent,
			DWORD dwTime
			);

		SCriticalSection	m_cs;			//�߳�ͬ������
		SList<EventArgs*>	*m_evtPending;//����ĵȴ�ִ�е��¼�
		DWORD				m_dwMainTrdID;//���߳�ID
		
		UINT_PTR			m_timerID;	//��ʱ��ID������ִ���첽�¼�

		SList<ISlotFunctor*>	m_evtHandlerMap;
	};
}
