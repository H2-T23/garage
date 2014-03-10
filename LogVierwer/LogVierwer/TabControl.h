#pragma once


// CTabControl
/**********************************************************************************
 *
 *
 *
 */
class CTabControl : public CTabCtrl {
	DECLARE_DYNAMIC(CTabControl)
protected:
	typedef std::vector<CWnd*>	TABITEMS;
	TABITEMS		m_TabItems;

public:
	CTabControl();
	virtual ~CTabControl();

	LONG	InsertTab( int nItem, LPCTSTR lpszItem, CWnd* pTab ){
		m_TabItems.push_back( pTab );
		return CTabCtrl::InsertItem(nItem, lpszItem);
	}
	CWnd*	TabSelChange( int nCurSel );
	void	TabResize( int cx, int cy );
	CWnd*	GetSelectTab( void ) const {
		return m_TabItems.at( GetCurSel() );
	}

protected:
	DECLARE_MESSAGE_MAP()
};


