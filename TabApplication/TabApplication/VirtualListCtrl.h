#pragma once


// CVirtualListCtrl
class CVirtualListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CVirtualListCtrl)

	const UINT		m_nMaxItemNum;
	CStringList		m_lstString;

public:
	CVirtualListCtrl();
	virtual ~CVirtualListCtrl();

	void	SetSelect( int idx ){
		SetItemState( idx, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
	}

	void	SetItemText( int nRow, int nCol, CString& str ){
		return(SetItemText(nRow,nCol,(LPCTSTR)str));
	}

	void	SetItemText( int nRow, int nCol, LPCTSTR lpszStr ){
		LVITEM	lvItem;
		lvItem.mask		= LVIF_TEXT;
		lvItem.iItem	= nRow;
		lvItem.iSubItem	= nCol;
		lvItem.pszText	= (LPTSTR)lpszStr;

		if( nCol == 0 ){
			InsertItem( &lvItem );
		}else{
			SetItem( &lvItem );
		}
	//	m_lstString.InsertAfter( (POSITION)nRow, lpszStr );
	}

	bool	IsLimit( void ) const {
		return(m_nMaxItemNum<=m_lstString.GetCount());
	}

	void	AppendText( LPCTSTR lpszText ){
		if( IsLimit() ){
			m_lstString.RemoveAll();
		}
		m_lstString.AddTail( lpszText );
		SetSelect( m_lstString.GetCount()-1 );
		EnsureVisible( m_lstString.GetCount()-1, TRUE );
	}


protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


