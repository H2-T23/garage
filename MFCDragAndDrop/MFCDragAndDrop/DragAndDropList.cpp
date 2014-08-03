// DragAndDropList.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCDragAndDrop.h"
#include "DragAndDropList.h"


// CDragAndDropList

IMPLEMENT_DYNAMIC(CDragAndDropList, CFixedCtrlPane)

BEGIN_MESSAGE_MAP(CDragAndDropList, CFixedCtrlPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY (LVN_BEGINDRAG, IDI_LIST, OnBeginDrag)
END_MESSAGE_MAP()

// CDragAndDropList メッセージ ハンドラー

int	CDragAndDropList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFixedCtrlPane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create CImageList
	VERIFY (m_Image.Create (16, 15, TRUE, 0, 1));
	CBitmap bm;
	VERIFY (bm.LoadBitmap (IDR_MAINFRAME));
	VERIFY (-1 != m_Image.Add (&bm, RGB (192, 192, 192)));

	// create CListCtrl
	VERIFY (m_List.Create (WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL, 
			CRect (0, 0, 0, 0), this, IDI_LIST));
	// create a column in the CListCtrl
	VERIFY (-1 != m_List.InsertColumn (0, _T("Button Label"), LVCFMT_LEFT, 100));
	m_List.SetImageList (&m_Image, LVSIL_SMALL);
	TCHAR*	szLabels[] = { _T("New"),
							_T("Open"),
							_T("Save"),
							_T("Cut"),
							_T("Copy"),
							_T("Paste"),
							_T("Print"),
							_T("Help"),
							_T("About") };

	for( int i=0; i<NUM_BUTTONS; i++)
		VERIFY (-1 != m_List.InsertItem (LVIF_IMAGE | LVIF_TEXT, i, szLabels[i], 0, 0, i, 0L));
	
	return 0;
}

void	CDragAndDropList::OnSize(UINT nType, int cx, int cy) 
{
	CFixedCtrlPane::OnSize(nType, cx, cy);
	m_List.MoveWindow (0, 0, cx, cy);
}

// this function is called in response to an LVN_BEGINDRAG
void	CDragAndDropList::OnBeginDrag (NMHDR* pnmhdr, LRESULT* pResult)
{		
	// save the index of the item being dragged in m_nDragIndex
	m_nDragIndex	= ((NM_LISTVIEW*)pnmhdr)->iItem;
	POINT pt;
	pt.x = 8;
	pt.y = 8;

	if( m_pDragImage ){
		delete m_pDragImage;
	}
	
	m_pDragImage	= m_List.CreateDragImage( m_nDragIndex, &pt );
	ASSERT (m_pDragImage);

	VERIFY (m_pDragImage->BeginDrag (0, CPoint (8, 8)));
	VERIFY (m_pDragImage->DragEnter (GetDesktopWindow (), ((NM_LISTVIEW *)pnmhdr)->ptAction));

	m_bDragging		= TRUE;
	m_hDropItem		= NULL;
	m_nDropIndex	= -1;
	m_pDropWnd		= &m_List;

	SetCapture();
}

void	CDragAndDropList::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		VERIFY (m_pDragImage->DragShowNolock (FALSE));

		// get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd);
		// if we drag outside current window
		if (pDropWnd != m_pDropWnd)
		{
			// turn off hilight for drop target in tree control
			if (m_hDropItem)
			{
				((CTreeCtrl*)m_pDropWnd)->SelectDropTarget (NULL);
				m_hDropItem = NULL;
			}
			// turn off hilight for drop target in list control
			if (m_nDropIndex != -1)
			{
				CListCtrl* pList = (CListCtrl*)m_pDropWnd;
				VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
				// redraw item
				VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
				pList->UpdateWindow ();
				m_nDropIndex = -1;
			}
		}

		// save current window pointer
		m_pDropWnd = pDropWnd;

		// convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient (&pt);

		// if window is CTreeCtrl
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CTreeCtrl)))
		{			
			UINT uFlags;
			// get the item that is below cursor
			m_hDropItem = ((CTreeCtrl*)pDropWnd)->HitTest (pt, &uFlags);
			// highlight it
			((CTreeCtrl*)pDropWnd)->SelectDropTarget (m_hDropItem);
		}
		else if (pDropWnd->IsKindOf (RUNTIME_CLASS (CListCtrl)))
		{			
			UINT uFlags;
			CListCtrl* pList = (CListCtrl*)pDropWnd;

			// turn off hilight for previous drop target
			pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
			// redraw previous item
			pList->RedrawItems (m_nDropIndex, m_nDropIndex);
			
			// get the item that is below cursor
			m_nDropIndex = ((CListCtrl*)pDropWnd)->HitTest (pt, &uFlags);
			// highlight it
			pList->SetItemState (m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
			// redraw item
			pList->RedrawItems (m_nDropIndex, m_nDropIndex);
			pList->UpdateWindow ();
		}
		// lock window updates
		VERIFY (m_pDragImage->DragShowNolock (TRUE));
	}
	
	CFixedCtrlPane::OnMouseMove(nFlags, point);
}

void	CDragAndDropList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		// release mouse capture
		VERIFY (::ReleaseCapture ());
		m_bDragging = FALSE;
		// end dragging
		VERIFY (m_pDragImage->DragLeave (GetDesktopWindow ()));
		m_pDragImage->EndDrag ();

		CPoint pt (point);
		ClientToScreen (&pt);
		// get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd);
		// if window is CTreeCtrl
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CTreeCtrl)))
			DropItemOnTree ((CTreeCtrl*)pDropWnd);
		// if window is CListCtrl
		else if (pDropWnd->IsKindOf (RUNTIME_CLASS (CListCtrl)))
			DropItemOnList ((CListCtrl*)pDropWnd);
	}
	
	CFixedCtrlPane::OnLButtonUp(nFlags, point);
}

// this function drops an item from a CListCtrl onto a CTreeCtrl
void	CDragAndDropList::DropItemOnTree (CTreeCtrl* pDropTree)
{
	// remove hilight from drop target
	pDropTree->SelectDropTarget (NULL);

	TCHAR szLabel[256];
	LV_ITEM lvi;
	TV_ITEM tvi;
	TV_INSERTSTRUCT tvis;

	ZeroMemory (&lvi, sizeof (LV_ITEM));
	lvi.iItem		= m_nDragIndex;
	lvi.mask		= TVIF_IMAGE | TVIF_TEXT;
	lvi.pszText		= szLabel;
	lvi.cchTextMax	= 255;
	// get item that was dragged
	VERIFY (m_List.GetItem (&lvi));

	tvi.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvi.pszText			= lvi.pszText;
	tvi.cchTextMax		= lvi.cchTextMax;
	tvi.iImage			= lvi.iImage;
	tvi.iSelectedImage	= lvi.iImage;

	// if cursor was over an tree item, insert new item above it
	if (m_hDropItem)
	{
		HTREEITEM hItem = pDropTree->GetNextItem (m_hDropItem, TVGN_PREVIOUS);
		tvis.hInsertAfter	= (hItem) ? hItem : TVI_FIRST;
	}
	// if cursor is not over an item but still inside tree, insert new item at the end
	else
		tvis.hInsertAfter	= TVI_LAST;

	tvis.hParent		= NULL;
	tvis.item			= tvi;
	// insert item into CTreeCtrl
	HTREEITEM hNewItem = pDropTree->InsertItem (&tvis);
	ASSERT (hNewItem);
	// select the new item we just inserted
	VERIFY (pDropTree->SelectItem (hNewItem));
	// delete the original item (move operation)
	VERIFY (m_List.DeleteItem (m_nDragIndex));
}

// this function drops an item from a CListCtrl to a CListCtrl 
void	CDragAndDropList::DropItemOnList (CListCtrl* pDropList)
{
	// unhilite the drop target
	pDropList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);

	TCHAR szLabel[256];
	LV_ITEM lvi;

	ZeroMemory (&lvi, sizeof (LV_ITEM));
	lvi.iItem		= m_nDragIndex;
	lvi.mask		= LVIF_IMAGE | LVIF_TEXT;
	lvi.pszText		= szLabel;
	lvi.cchTextMax	= 255;
	// get item that was dragged
	VERIFY (m_List.GetItem (&lvi));

	// insert item into CListCtrl
	lvi.iItem = (m_nDropIndex == -1) ? pDropList->GetItemCount () : m_nDropIndex;
	VERIFY (-1 != pDropList->InsertItem (&lvi));
	// select the new item we just inserted
	VERIFY (pDropList->SetItemState (lvi.iItem, LVIS_SELECTED, LVIS_SELECTED));
	// delete the original item (move operation)
	VERIFY (m_List.DeleteItem (m_nDragIndex));
}

