#pragma once


// CTreeCtrlEx

class CTreeCtrlEx : public CTreeCtrl {
	DECLARE_DYNAMIC(CTreeCtrlEx)
public:
	CTreeCtrlEx() 
		: m_bDrag(FALSE)
		, m_hDragItem(NULL)
		, m_pDragImage(NULL)
	{
	}

	virtual ~CTreeCtrlEx(){
		if( m_pDragImage ){
			delete m_pDragImage;
		}
	}

protected:
	BOOL			m_bDrag;
	HTREEITEM		m_hDragItem;
	CImageList*		m_pDragImage;


protected:
	afx_msg void	OnTvnBegindrag( NMHDR *pNMHDR, LRESULT *pResult )
	{
		LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
		// TODO: ここにコントロール通知ハンドラー コードを追加します。
		*pResult = 0;

		if( m_bDrag == FALSE )
		{
			CPoint	point( pNMTreeView->ptDrag );

			UINT	uFlags;
			HTREEITEM	hItem	= HitTest( point, &uFlags );
			if( (hItem != NULL) && (TVHT_ONITEM & uFlags) )
			{
				if( m_pDragImage ){
					delete m_pDragImage;
				}
				m_pDragImage	= CreateDragImage( hItem );
				if( m_pDragImage ){
					m_pDragImage->BeginDrag( 0, CPoint(8,8) );
					m_pDragImage->DragEnter( this, point );
				}

				m_hDragItem		= hItem;	
				m_bDrag			= TRUE;
			}
		}
	}

	afx_msg void	OnLButtonDown( UINT nFlags, CPoint point ){
		__super::OnLButtonDown(nFlags, point);
	}

	afx_msg void	OnMouseMove( UINT nFlags, CPoint point ){
		if( m_bDrag )
		{
			if( m_pDragImage ){
				m_pDragImage->DragMove( point );
				m_pDragImage->DragShowNolock( FALSE );
			}

			UINT uFlags;
			HTREEITEM hItem		= HitTest( point, &uFlags );

			if( (hItem != NULL) && (TVHT_ONITEM & uFlags) )
			{
				if( ItemHasChildren(hItem) ){
					Expand( hItem, TVE_EXPAND );
				}

			   SelectDropTarget( hItem );
			   ASSERT(GetDropHilightItem() == hItem);
			}

			if( m_pDragImage ){
				m_pDragImage->DragShowNolock( TRUE );
			}
		}
		__super::OnMouseMove(nFlags, point);
	}

	afx_msg void	OnLButtonUp( UINT nFlags, CPoint point ){
		if( m_bDrag )
		{
			if( m_pDragImage ){
				m_pDragImage->DragLeave( this );
				m_pDragImage->EndDrag();
			}

			UINT uFlags;
			HTREEITEM hItem		= HitTest( point, &uFlags );

			if( (hItem != NULL) && (TVHT_ONITEM & uFlags) )
			{
				InsertItem( GetItemText(m_hDragItem), hItem );
				DeleteItem( m_hDragItem );
				Expand( hItem, TVE_EXPAND );
				SelectDropTarget( hItem );
			}

			m_hDragItem	= NULL;
			m_bDrag		= FALSE;
		}
		__super::OnLButtonUp(nFlags, point);
	}

	DECLARE_MESSAGE_MAP()
};

