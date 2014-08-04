// DragAndDropList.cpp : 実装ファイル
//
#include "DragAndDropList.h"


#if 0
class CDragAndDrop {
protected:
	BOOL					m_bDragging;

	CImageList*				m_pDragImage;

public:
	CDragAndDrop( void )
		: m_bDragging(FALSE)
	{
	}

public:
	void		OnDragEnter( CPoint& pointp ){
		if( m_pDragImage ){
			delete m_pDragImage;
		}
	
		m_pDragImage	= m_List.CreateDragImage( m_nDragIndex, &pt );
		ASSERT(m_pDragImage);

		m_pDragImage->BeginDrag( 0, CPoint(8, 8) );
		m_pDragImage->DragEnter( GetDesktopWindow(), ((NM_LISTVIEW *)pnmhdr)->ptAction );

		m_bDragging		= TRUE;
		m_hDropItem		= NULL;
		m_nDropIndex	= -1;
		m_pDropWnd		= &m_List;

		SetCapture();
	}

	void		OnDragging( CPoint& point ){
		if( m_bDragging )
		{
			CPoint	pt(point);
			ClientToScreen( &pt );
			m_pDragImage->DragMove( pt );
			m_pDragImage->DragShowNolock( FALSE );

			CWnd* pDropWnd	= CWnd::WindowFromPoint( pt );
			ASSERT(pDropWnd);

			if( pDropWnd != m_pDropWnd )
			{
				if( m_hDropItem )
				{
					((CTreeCtrl*)m_pDropWnd)->SelectDropTarget( NULL );
					m_hDropItem = NULL;
				}

				if( m_nDropIndex != -1 )
				{
					CListCtrl* pList	= (CListCtrl*)m_pDropWnd;
					pList->SetItemState( m_nDropIndex, 0, LVIS_DROPHILITED );
					pList->RedrawItems( m_nDropIndex, m_nDropIndex );
					pList->UpdateWindow();
					m_nDropIndex = -1;
				}
			}

			m_pDropWnd	= pDropWnd;
			pDropWnd->ScreenToClient( &pt );

			if( pDropWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) )
			{			
				UINT uFlags;
				m_hDropItem		= ((CTreeCtrl*)pDropWnd)->HitTest( pt, &uFlags );
				((CTreeCtrl*)pDropWnd)->SelectDropTarget( m_hDropItem );
			}
			else
			if( pDropWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)) )
			{			
				UINT uFlags;
				CListCtrl* pList	= (CListCtrl*)pDropWnd;

				pList->SetItemState( m_nDropIndex, 0, LVIS_DROPHILITED );
				pList->RedrawItems( m_nDropIndex, m_nDropIndex );

				m_nDropIndex	= ((CListCtrl*)pDropWnd)->HitTest( pt, &uFlags );

				pList->SetItemState( m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED );
				pList->RedrawItems( m_nDropIndex, m_nDropIndex );
				pList->UpdateWindow();
			}

			m_pDragImage->DragShowNolock( TRUE );
		}
	}

	void		OnDrop( CPoint& point ){
		if( m_bDragging )
		{
			::ReleaseCapture();
			m_bDragging	= FALSE;

			m_pDragImage->DragLeave( CWnd::GetDesktopWindow() );
			m_pDragImage->EndDrag();

			CPoint		pt(point);
			ClientToScreen( &pt );

			CWnd* pDropWnd	= CWnd::WindowFromPoint( pt );
			ASSERT(pDropWnd);

			if( pDropWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)) )
			{
				DropItemOnTree( (CTreeCtrl*)pDropWnd );
			}
			else
			if( pDropWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)) )
			{
				DropItemOnList( (CListCtrl*)pDropWnd );
			}
		}
	}
};
#endif



// CDragAndDropList

IMPLEMENT_DYNAMIC(CDragAndDropList, CFixedCtrlPane)

BEGIN_MESSAGE_MAP(CDragAndDropList, CFixedCtrlPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_BEGINDRAG, IDI_LIST, OnBeginDrag)
END_MESSAGE_MAP()

// CDragAndDropList メッセージ ハンドラー

int	CDragAndDropList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CFixedCtrlPane::OnCreate(lpCreateStruct) == -1 ){
		return -1;
	}
	
	m_Image.Create( 16, 15, TRUE, 0, 1 );
	CBitmap		bm;
	bm.LoadBitmap(IDR_MAINFRAME);
	if( -1 == m_Image.Add( &bm, RGB (192, 192, 192)) ){
		return -1;
	}

	if( !m_List.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL, CRect(0,0,0,0), this, IDI_LIST) ){
		return -1;
	}

	if( -1 == m_List.InsertColumn(0, _T("Button Label"), LVCFMT_LEFT, 100) ){
		return -1;
	}


	m_List.SetImageList( &m_Image, LVSIL_SMALL );



	TCHAR*	szLabels[] = { _T("New"),
							_T("Open"),
							_T("Save"),
							_T("Cut"),
							_T("Copy"),
							_T("Paste"),
							_T("Print"),
							_T("Help"),
							_T("About") };

	for( int i=0; i<NUM_BUTTONS; i++ ){
		m_List.InsertItem( LVIF_IMAGE | LVIF_TEXT, i, szLabels[ i ], 0, 0, i, 0L );
	}
	
	return 0;
}

void	CDragAndDropList::OnSize(UINT nType, int cx, int cy) 
{
	CFixedCtrlPane::OnSize(nType, cx, cy);
	m_List.MoveWindow(0, 0, cx, cy);
}

void	CDragAndDropList::OnBeginDrag(NMHDR* pnmhdr, LRESULT* pResult)
{		
	m_nDragIndex	= ((NM_LISTVIEW*)pnmhdr)->iItem;
	POINT pt;
	pt.x = 8;
	pt.y = 8;

}

void	CDragAndDropList::OnMouseMove( UINT nFlags, CPoint point ) 
{

	CFixedCtrlPane::OnMouseMove(nFlags, point);
}

void	CDragAndDropList::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	
	CFixedCtrlPane::OnLButtonUp(nFlags, point);
}

void	CDragAndDropList::DropItemOnTree( CTreeCtrl* pDropTree )
{
	pDropTree->SelectDropTarget( NULL );

	TCHAR szLabel[256];
	LV_ITEM lvi;
	TV_ITEM tvi;
	TV_INSERTSTRUCT tvis;

	ZeroMemory(&lvi, sizeof (LV_ITEM));
	lvi.iItem		= m_nDragIndex;
	lvi.mask		= TVIF_IMAGE | TVIF_TEXT;
	lvi.pszText		= szLabel;
	lvi.cchTextMax	= 255;

	m_List.GetItem( &lvi );

	tvi.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvi.pszText			= lvi.pszText;
	tvi.cchTextMax		= lvi.cchTextMax;
	tvi.iImage			= lvi.iImage;
	tvi.iSelectedImage	= lvi.iImage;

	if( m_hDropItem )
	{
		HTREEITEM hItem		= pDropTree->GetNextItem (m_hDropItem, TVGN_PREVIOUS);
		tvis.hInsertAfter	= (hItem) ? hItem : TVI_FIRST;
	}
	else
		tvis.hInsertAfter	= TVI_LAST;

	tvis.hParent		= NULL;
	tvis.item			= tvi;

	HTREEITEM hNewItem	= pDropTree->InsertItem( &tvis );
	ASSERT(hNewItem);

	pDropTree->SelectItem( hNewItem );
	m_List.DeleteItem( m_nDragIndex );
}

void	CDragAndDropList::DropItemOnList( CListCtrl* pDropList )
{
	pDropList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);

	TCHAR szLabel[256];
	LV_ITEM lvi;

	ZeroMemory(&lvi, sizeof(LV_ITEM));
	lvi.iItem			= m_nDragIndex;
	lvi.mask			= LVIF_IMAGE | LVIF_TEXT;
	lvi.pszText			= szLabel;
	lvi.cchTextMax		= 255;

	m_List.GetItem( &lvi );

	lvi.iItem	= (m_nDropIndex == -1) ? pDropList->GetItemCount () : m_nDropIndex;
	VERIFY (-1 != pDropList->InsertItem (&lvi));

	pDropList->SetItemState( lvi.iItem, LVIS_SELECTED, LVIS_SELECTED );

	m_List.DeleteItem( m_nDragIndex );
}

