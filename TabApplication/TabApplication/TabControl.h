#pragma once


// CTabControl

class CTabControl : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabControl)

protected:

	std::vector<CWnd*>	m_vecWnd;
	CWnd*				m_pActiveWnd;

public:
	CTabControl();
	virtual ~CTabControl();

	inline
	int			GetTabCount( void ) const {
		return( m_vecWnd.size() );
	}

	inline
	bool		IsValidTag( int nItem ) const {
		return( (0<=nItem) && (nItem<GetTabCount()) ) ;
	}

	void		Append( CWnd* pWnd ){
		m_vecWnd.push_back( pWnd );

		CString	str;
		pWnd->GetWindowText( str );

		CTabCtrl::InsertItem( GetItemCount(), str );
	}

	void		Append( CWnd* pWnd, LPCTSTR lpszCaption ){
		Append( pWnd );
		SetTabCaption( GetTabCount() - 1, lpszCaption );
	}

	void		SetActivePage( int nItem ){
		SetCurSel( nItem );
		m_pActiveWnd = m_vecWnd[ nItem ];
		if( m_pActiveWnd ){
			int i=0;
			std::vector<CWnd*>::iterator it = m_vecWnd.begin();
			while( it != m_vecWnd.end() ){
				(*it)->ShowWindow( i == nItem ? SW_SHOW : SW_HIDE );
				i++; it++;
			}
		}
	}

	void		SetTabCaption( int nItem, LPCTSTR lpszCaption ){
		if( IsValidTag(nItem) ){
			TCHAR	buf[256] = {0};
			_tcscpy_s( buf, 256, lpszCaption );

			TCITEM	tcItem;
			tcItem.pszText		= buf;
			tcItem.cchTextMax	= 256;
			tcItem.mask			= TCIF_TEXT;

			SetItem( nItem, &tcItem );
		}
	}

	CWnd*		GetActivePage( void ) const { 
		return(m_pActiveWnd); 
	}

	void		Resize( void ){
		CWnd*	pActiveWnd = GetActivePage();
		if( pActiveWnd ){
			CRect rect;
			GetWindowRect(&rect);
			AdjustRect(FALSE, &rect);
			ScreenToClient(&rect);
			pActiveWnd->MoveWindow( &rect );
		}
	}

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


