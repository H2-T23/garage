#pragma once

class CLogList;

/**********************************************************************************
 *
 *
 *
 */
class CVirtualListCtrl : public CListCtrl {
	DECLARE_DYNAMIC(CVirtualListCtrl)
protected:
	int				m_nPrevCount;
	CLogList*		m_pLogList;

public:
	CVirtualListCtrl();
	virtual ~CVirtualListCtrl();

protected:
	BOOL	PreCreateWindow( CREATESTRUCT& cs );

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int		OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void	OnDestroy();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnLvnGetdispinfo( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void	OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


