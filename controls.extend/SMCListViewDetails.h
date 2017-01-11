#pragma once
namespace SOUI
{
	class SMCListViewDetails :
		public SMCListView
	{
		SOUI_CLASS_NAME(SMCListViewDetails, L"mclistviewdetails")
	public:

		SMCListViewDetails();
		virtual ~SMCListViewDetails();
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			MSG_WM_SETFOCUS_EX(OnSetFocus)
			MSG_WM_KILLFOCUS_EX(OnKillFocus)
			MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseEvent)
			
			SOUI_MSG_MAP_END()

			SOUI_ATTRS_BEGIN()
			SOUI_ATTRS_END()
	public:
		BOOL m_bHasIgnore;
		BOOL m_bHasProperities;
	protected:
		LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void OnPaint(IRenderTarget *pRT);  
		void OnMouseLeave();
		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	public:
		void EnsureItemFirstVisible(int iItem);
	private:
		void ShowBtn(SItemPanel* pPanel);
		void HideBtn(SItemPanel* pPanel);
		
	};
}


