#pragma once
#include "splitterpanel.h"
#include "VirtualListCtrl.h"
#include "MultiEdit.h"

/**********************************************************************************
 *
 *
 *
 */
class CListPanel : public CSplitterPanel {
	DECLARE_DYNAMIC(CListPanel)
public:
	enum { 
		IDC_LIST = 1501, IDC_EDIT 
	};

protected:
	CVirtualListCtrl		m_wndList;
	CMultiEdit				m_wndEdit;

public:
	CListPanel(void);
	virtual ~CListPanel(void);

protected:
	afx_msg	int			OnCreate(LPCREATESTRUCT lpCreateStruct){
		if( CSplitterPanel::OnCreate(lpCreateStruct) == -1 ){
			return -1;
		}

		CRect	rc;
		GetClientRect(rc);

		DWORD	dwExStyle	= 0;
		DWORD	dwStyle		= (WS_CHILD | WS_VISIBLE | WS_BORDER);
		//		dwStyle		|= WS_CLIPSIBLINGS;

		if( !m_wndList.Create((dwStyle | LVS_REPORT), rc, this, IDC_LIST) ){
			return -1;
		}

		if( !m_wndEdit.Create((dwStyle), rc, this, IDC_EDIT) ){
			return -1;
		}

		m_Splitter.Bind( this );
		m_Splitter.Append( &m_wndList	);
		m_Splitter.Append( &m_wndEdit	);
		return 0;
	}

	afx_msg void		OnSize(UINT nType, int cx, int cy){
		CSplitterPanel::OnSize(nType, cx, cy);
	//	m_wndList.Invalidate();
	//	m_wndEdit.Invalidate();
	//	UpdateData();
	//	Invalidate(FALSE);
	}

protected:
	DECLARE_MESSAGE_MAP()
};

