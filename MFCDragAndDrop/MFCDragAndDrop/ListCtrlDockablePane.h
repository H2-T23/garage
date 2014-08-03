#pragma once
#include "DragAndDropList.h"

// CListCtrlDockablePane

class CListCtrlDockablePane : public CDockablePane {
	DECLARE_DYNAMIC(CListCtrlDockablePane)
public:
	CListCtrlDockablePane();
	virtual ~CListCtrlDockablePane();

protected:
	CDragAndDropList		m_ListCtrl;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


