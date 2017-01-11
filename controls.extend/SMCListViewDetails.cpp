#include "stdafx.h"
#include "SMCListViewDetails.h"

namespace SOUI
{
	SMCListViewDetails::SMCListViewDetails()
	{
		m_bHasIgnore = FALSE;
	}


	SMCListViewDetails::~SMCListViewDetails()
	{
	}

	LRESULT SMCListViewDetails::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SetMsgHandled(FALSE);

		if (!m_adapter)
		{
			return 0;
		}

		LRESULT lRet = 0;
		SOUI::CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		if (m_itemCapture)
		{
			SOUI::CRect rcItem = m_itemCapture->GetItemRect();
			pt.Offset(-rcItem.TopLeft());
			lRet = m_itemCapture->DoFrameEvent(uMsg, wParam, MAKELPARAM(pt.x, pt.y));
		}
		else
		{
			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN)
			{//交给panel处理
				__super::ProcessSwndMessage(uMsg, wParam, lParam, lRet);
			}

			SItemPanel * pHover = HitTest(pt);
			if (pHover != m_pHoverItem)
			{
				SItemPanel * nOldHover = m_pHoverItem;
				m_pHoverItem = pHover;
				if (nOldHover)
				{
					nOldHover->DoFrameEvent(WM_MOUSELEAVE, 0, 0);
					RedrawItem(nOldHover);
					HideBtn(nOldHover);
				}
				if (m_pHoverItem)
				{
					m_pHoverItem->DoFrameEvent(WM_MOUSEHOVER, wParam, MAKELPARAM(pt.x, pt.y));
					RedrawItem(m_pHoverItem);
					ShowBtn(m_pHoverItem);
				}
			}
			if (m_pHoverItem)
			{
				m_pHoverItem->DoFrameEvent(uMsg, wParam, MAKELPARAM(pt.x, pt.y));
			}
		}

		if (uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONUP)
		{//交给panel处理
			__super::ProcessSwndMessage(uMsg, wParam, lParam, lRet);
		}
		SetFocus();
		SetMsgHandled(FALSE);
		return 0;
	}

	void SMCListViewDetails::ShowBtn(SItemPanel* pPanel)
	{
		if (!m_bHasIgnore && !m_bHasProperities)
			return;
		pPanel->InvalidateRect(NULL);
		SButton *pIgnBtn = pPanel->FindChildByName2<SButton>(L"btn_ignore");
		if (pIgnBtn && m_bHasIgnore) pIgnBtn->SetVisible(TRUE);
		SButton *pPerBtn = pPanel->FindChildByName2<SButton>(L"btn_properities");
		if (pPerBtn && m_bHasProperities) pPerBtn->SetVisible(TRUE);

	}

	void SMCListViewDetails::HideBtn(SItemPanel* pPanel)
	{
		if (!m_bHasIgnore && !m_bHasProperities)
			return;
		pPanel->InvalidateRect(NULL);
		SButton *pIgnBtn = pPanel->FindChildByName2<SButton>(L"btn_ignore");
		if (pIgnBtn  && m_bHasIgnore) pIgnBtn->SetVisible(FALSE);
		SButton *pPerBtn = pPanel->FindChildByName2<SButton>(L"btn_properities");
		if (pPerBtn && m_bHasProperities) pPerBtn->SetVisible(FALSE);
	}

	void SMCListViewDetails::OnPaint(IRenderTarget *pRT)
	{
		__super::OnPaint(pRT);
	}

	void SMCListViewDetails::OnMouseLeave()
	{
		if (m_pHoverItem)
		{
			m_pHoverItem->DoFrameEvent(WM_MOUSELEAVE, 0, 0);
			HideBtn(m_pHoverItem);
			m_pHoverItem = NULL;
			
		}
		KillFocus();
	}

	BOOL SMCListViewDetails::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		return __super::OnMouseWheel(nFlags, zDelta, pt);
	}

	void SMCListViewDetails::EnsureItemFirstVisible(int iItem)
	{
		if (iItem < 0 || iItem >= m_adapter->getCount()) return;

		int iFirstVisible = m_iFirstVisible;
		int iLastVisible = m_iFirstVisible + m_lstItems.GetCount();

		if (iItem >= iFirstVisible && iItem < iLastVisible)
		{
		/*	if (iItem == iFirstVisible)
			{
				int pos = m_lvItemLocator->Item2Position(iItem);
				OnScroll(TRUE, SB_THUMBPOSITION, pos);
			}
			else if (iItem == iLastVisible - 1)
			{
				if (iItem == m_adapter->getCount() - 1)
					OnScroll(TRUE, SB_BOTTOM, 0);
				else
				{
					int pos = m_lvItemLocator->Item2Position(iItem + 1) - m_siVer.nPage;
					OnScroll(TRUE, SB_THUMBPOSITION, pos);
				}
			}*/
			
			
			int pos = m_lvItemLocator->Item2Position(iItem);
			OnScroll(TRUE, SB_THUMBPOSITION, pos);
			return;
		}

		if (iItem < iFirstVisible)
		{//scroll up
			int pos = m_lvItemLocator->Item2Position(iItem);
			OnScroll(TRUE, SB_THUMBPOSITION, pos);
		}
		else // if(iItem >= iLastVisible)
		{//scroll down
			if (iItem == m_adapter->getCount() - 1)
			{
				OnScroll(TRUE, SB_BOTTOM, 0);
			}
			else
			{
				int pos = m_lvItemLocator->Item2Position(iItem + 1) - m_siVer.nPage;
				OnScroll(TRUE, SB_THUMBPOSITION, pos);
			}
		}
	}



}
