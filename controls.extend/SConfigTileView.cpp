#include "stdafx.h"
#include "SConfigTileView.h"

namespace SOUI
{
	SConfigTileView::SConfigTileView()
	{

	}


	SConfigTileView::~SConfigTileView()
	{

	}


	void SConfigTileView::OnPaint(IRenderTarget *pRT)
	{
		if (m_bDatasetInvalidated)
		{
			UpdateVisibleItems();
			m_bDatasetInvalidated = FALSE;
		}
		SPainter duiDC;
		BeforePaint(pRT, duiDC);


		int iFirst = m_iFirstVisible;
		if (iFirst != -1)
		{
			SOUI::CRect rcClient;
			GetClientRect(&rcClient);
			pRT->PushClipRect(&rcClient, RGN_AND);

			SOUI::CRect rcClip, rcInter;
			pRT->GetClipBox(&rcClip);

			int nOffset = m_tvItemLocator->Item2Position(iFirst) - m_siVer.nPos;
			int nLastBottom = rcClient.top + m_tvItemLocator->GetMarginSize() + nOffset;

			SPOSITION pos = m_lstItems.GetHeadPosition();
			int i = 0;
			for (; pos; i++)
			{
				ItemInfo ii = m_lstItems.GetNext(pos);
				SOUI::CRect rcItem = m_tvItemLocator->GetItemRect(iFirst + i);
				rcItem.OffsetRect(rcClient.left, 0);
				rcItem.MoveToY(nLastBottom);
				if (m_tvItemLocator->IsLastInRow(iFirst + i))
				{
					nLastBottom = rcItem.bottom + m_tvItemLocator->GetMarginSize();
				}

				rcInter.IntersectRect(&rcClip, &rcItem);
				if (!rcInter.IsRectEmpty())
				{
					ii.pItem->Draw(pRT, rcItem);
				}
			}

			pRT->PopClip();
		}
		AfterPaint(pRT, duiDC);
	}

	void SConfigTileView::OnDestroy()
	{
		//destroy all itempanel
		SPOSITION pos = m_lstItems.GetHeadPosition();
		while (pos)
		{
			ItemInfo ii = m_lstItems.GetNext(pos);
			ii.pItem->Release();
		}
		m_lstItems.RemoveAll();

		for (int i = 0; i < (int)m_itemRecycle.GetCount(); i++)
		{
			SList<SItemPanel *> *pLstTypeItems = m_itemRecycle[i];
			SPOSITION pos = pLstTypeItems->GetHeadPosition();
			while (pos)
			{
				SItemPanel *pItem = pLstTypeItems->GetNext(pos);
				pItem->Release();
			}
			delete pLstTypeItems;
		}
		m_itemRecycle.RemoveAll();
		__super::OnDestroy();
	}

	void SConfigTileView::OnMouseLeave()
	{
		if (m_pHoverItem)
		{
			m_pHoverItem->DoFrameEvent(WM_MOUSELEAVE, 0, 0);
			m_pHoverItem = NULL;
		}
	}

	LRESULT SConfigTileView::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

			SItemPanel *pHover = HitTest(pt);
			if (pHover != m_pHoverItem)
			{
				SItemPanel *nOldHover = m_pHoverItem;
				m_pHoverItem = pHover;
				if (nOldHover)
				{
					nOldHover->DoFrameEvent(WM_MOUSELEAVE, 0, 0);
					RedrawItem(nOldHover);
					///////隐藏右上角按钮///////
					HideConfigBtn(nOldHover);
				}
				if (m_pHoverItem)
				{
					m_pHoverItem->DoFrameEvent(WM_MOUSEHOVER, wParam, MAKELPARAM(pt.x, pt.y));
					RedrawItem(m_pHoverItem);
					///////显示右上角按钮///////
					ShowConfigBtn(m_pHoverItem);
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
		SetMsgHandled(TRUE);

		return 0;
	}

	void SConfigTileView::ShowConfigBtn(SItemPanel *pItem)
	{
		pItem->InvalidateRect(NULL);
		SButton *pConfigBtn = pItem->FindChildByName2<SButton>(L"btn_setting");
		if (pConfigBtn)
		{
			pConfigBtn->SetVisible(TRUE);
		}
	}

	void SConfigTileView::HideConfigBtn(SItemPanel *pItem)
	{
		pItem->InvalidateRect(NULL);
		SButton *pConfigBtn = pItem->FindChildByName2<SButton>(L"btn_setting");
		if (pConfigBtn)
		{
			pConfigBtn->SetVisible(FALSE);
		}
	}

}


