#pragma once
namespace SOUI
{
	class SConfigTileView :
		public STileView
	{
		SOUI_CLASS_NAME(SConfigTileView, L"configtileview")
	public:
		SConfigTileView();
		virtual ~SConfigTileView();

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_DESTROY(OnDestroy)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseEvent)
			SOUI_MSG_MAP_END()

			SOUI_ATTRS_BEGIN()
			SOUI_ATTRS_END()

	protected:
		void OnMouseLeave();
		void OnPaint(IRenderTarget *pRT);
		void OnSize(UINT nType, SOUI::CSize size);
		void OnDestroy();

		LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		void ShowConfigBtn(SItemPanel *pItem);
		void HideConfigBtn(SItemPanel *pItem);
	};


}
