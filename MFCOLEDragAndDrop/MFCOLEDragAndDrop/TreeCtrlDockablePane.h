#pragma once
#include "DragAndDropTree.h"


// TreeCtrlDockablePane

class CTreeCtrlDockablePane : public CDockablePane {
	DECLARE_DYNAMIC(CTreeCtrlDockablePane)
public:
	CTreeCtrlDockablePane();
	virtual ~CTreeCtrlDockablePane();

protected:
	CDragAndDropTree		m_TreeCtrl;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


