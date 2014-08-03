// DragAndDropTree.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCDragAndDrop.h"
#include "DragAndDropTree.h"


// CDragAndDropTree

IMPLEMENT_DYNAMIC(CDragAndDropTree, CFixedCtrlPane)

BEGIN_MESSAGE_MAP(CDragAndDropTree, CFixedCtrlPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY (TVN_BEGINDRAG, IDI_TREE, OnBeginDrag)
END_MESSAGE_MAP()

// CDragAndDropTree メッセージ ハンドラー
int		CDragAndDropTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFixedCtrlPane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create CImageList
	VERIFY (m_Image.Create (16, 15, TRUE, 0, 1));
	CBitmap bm;
	VERIFY (bm.LoadBitmap (IDR_MAINFRAME));
	VERIFY (-1 != m_Image.Add (&bm, RGB (192, 192, 192)));
	// create CTreeCtrl
	VERIFY (m_Tree.Create (WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, CRect (0, 0, 0, 0), this, IDI_TREE));
	
	m_Tree.SetImageList (&m_Image, TVSIL_NORMAL);
	TV_INSERTSTRUCT tvis;
	TV_ITEM tvi;
	TCHAR *szLabels[] = {	_T("New"),
		_T("Open"),
		_T("Save"),
		_T("Cut"),
		_T("Copy"),
		_T("Paste"),
		_T("Print"),
		_T("Help"),
		_T("About") };

	// insert items into CTreeCtrl
	for (int iCntr = 0; iCntr < NUM_BUTTONS; iCntr++)
	{
		tvi.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
		tvi.pszText			= szLabels[iCntr];
		tvi.iImage			= iCntr;
		tvi.iSelectedImage	= iCntr;

		tvis.hParent		= NULL;
		tvis.hInsertAfter	= TVI_LAST;
		tvis.item			= tvi;
		VERIFY(m_Tree.InsertItem (&tvis));
	}

	return 0;
}

void	CDragAndDropTree::OnSize(UINT nType, int cx, int cy) 
{
	CFixedCtrlPane::OnSize(nType, cx, cy);
	m_Tree.MoveWindow (0, 0, cx, cy);	
}

void	CDragAndDropTree::OnBeginDrag (NMHDR* pnmhdr, LRESULT* pResult)
{
	m_hDragItem = ((NM_TREEVIEW *)pnmhdr)->itemNew.hItem;
	// create a drag image

	// Added 7/29/96
	if(m_pDragImage)
		delete m_pDragImage;

	m_pDragImage = m_Tree.CreateDragImage (m_hDragItem);
	ASSERT (m_pDragImage);
	// changes the cursor to the drag image (DragMove() is still required in 
	// OnMouseMove())
	VERIFY (m_pDragImage->BeginDrag (0, CPoint (8, 8)));
	VERIFY (m_pDragImage->DragEnter (GetDesktopWindow (), ((NM_TREEVIEW *)pnmhdr)->ptDrag));
	// set dragging flag
	m_bDragging = TRUE;
	m_hDropItem = NULL;
	m_nDropIndex = -1;
	m_pDropWnd = &m_Tree;
	// capture all mouse messages
	SetCapture ();
}

void	CDragAndDropTree::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
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
			// turn off drag hilite for tree control
			if (m_hDropItem)
			{
				((CTreeCtrl*)m_pDropWnd)->SelectDropTarget (NULL);
				m_hDropItem = NULL;
			}
			// turn off drag hilite for list control
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

			// unhilight previous item
			pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
			// redraw previous item
			pList->RedrawItems (m_nDropIndex, m_nDropIndex);
			
			// get the item that is below cursor
			m_nDropIndex = ((CListCtrl*)pDropWnd)->HitTest (pt, &uFlags);

			// highlight it
			pList->SetItemState( m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED );

			// redraw item
			pList->RedrawItems( m_nDropIndex, m_nDropIndex );
			pList->UpdateWindow();
		}
		// lock window updates
		VERIFY (m_pDragImage->DragShowNolock (TRUE));
	}
	CFixedCtrlPane::OnMouseMove(nFlags, point);
}

void	CDragAndDropTree::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		// end dragging
		VERIFY (m_pDragImage->DragLeave (GetDesktopWindow ()));	
		m_pDragImage->EndDrag ();
		// stop intercepting all mouse messages
		VERIFY (::ReleaseCapture ());
		m_bDragging = FALSE;

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

// this function drops an item from a CTreeCtrl onto a CTreeCtrl
void	CDragAndDropTree::DropItemOnTree (CTreeCtrl* pDropTree)
{
	// unhilite the drop target
	pDropTree->SelectDropTarget (NULL);

	TCHAR szLabel[256];
	TV_ITEM tvi;
	TV_INSERTSTRUCT tvis;

	tvi.hItem		= m_hDragItem;
	tvi.mask		= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_HANDLE;
	tvi.pszText		= szLabel;
	tvi.cchTextMax	= 255;
	// get item that was dragged
	VERIFY (m_Tree.GetItem (&tvi));
	
	if (m_hDropItem)
	{
		HTREEITEM hItem = pDropTree->GetNextItem (m_hDropItem, TVGN_PREVIOUS);
		tvis.hInsertAfter	= (hItem) ? hItem : TVI_FIRST;
	}
	else
		tvis.hInsertAfter	= TVI_LAST;

	tvis.hParent	= NULL;
	tvis.item		= tvi;
	// insert item into CTreeCtrl
	HTREEITEM hNewItem = pDropTree->InsertItem (&tvis);
	ASSERT (hNewItem);
	// select the new item we just inserted
	VERIFY (pDropTree->SelectItem (hNewItem));
	// delete the original item (move operation)
	VERIFY (m_Tree.DeleteItem (m_hDragItem));
}

// this function drops an item from a CTreeCtrl to a CListCtrl 
void	CDragAndDropTree::DropItemOnList (CListCtrl* pDropList)
{
	// unhilite the drop target
	pDropList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);

	TCHAR szLabel[256];
	TV_ITEM tvi;

	tvi.hItem = m_hDragItem;
	tvi.mask  = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_HANDLE;
	tvi.pszText	= szLabel;
	tvi.cchTextMax = 255;
	// get item that was dragged
	VERIFY (m_Tree.GetItem (&tvi));

	int iIndex = (m_nDropIndex == -1) ? pDropList->GetItemCount () : m_nDropIndex;
	// insert item into CListCtrl
	VERIFY (-1 != pDropList->InsertItem (LVIF_IMAGE | LVIF_TEXT, iIndex, tvi.pszText,	0, 0, tvi.iImage, 0L));
	// select the new item we just inserted
	VERIFY (pDropList->SetItemState (iIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED));
	// delete the original item (move operation)
	VERIFY (m_Tree.DeleteItem (m_hDragItem));
}
