#pragma once


// CFixedCtrlPane

class CFixedCtrlPane : public CWnd {
	DECLARE_DYNAMIC(CFixedCtrlPane)
public:
	CFixedCtrlPane();
	virtual ~CFixedCtrlPane();

	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs){
		return CWnd::PreCreateWindow(cs);
	}

protected:
	CString		m_strClassName;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct){
		if (CWnd::OnCreate(lpCreateStruct) == -1)
			return -1;
		return 0;
	}

	afx_msg void	OnSize(UINT nType, int cx, int cy){
		CWnd::OnSize(nType, cx, cy);
	}
};