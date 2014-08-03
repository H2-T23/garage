
// MainFrm.h : CMainFrame クラスのインターフェイス
//

#pragma once
#include "ChildView.h"
#include "FixedCtrlPane.h"
#include "DragAndDropTree.h"
#include "DragAndDropList.h"
#include "ListCtrlDockablePane.h"
#include "TreeCtrlDockablePane.h"

class CMainFrame : public CFrameWndEx
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 属性
public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 実装
public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // コントロール バー用メンバー
	CToolBar			m_wndToolBar;
	CStatusBar			m_wndStatusBar;
	CChildView			m_wndView;
//	CDragAndDropList		m_wndFixedCtrl;
//	CDragAndDropTree		m_wndFixedCtrl2;

	CListCtrlDockablePane	m_wndListCtrlDock;
	CTreeCtrlDockablePane	m_wndTreeCtrlDock;

// 生成された、メッセージ割り当て関数
protected:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSetFocus(CWnd *pOldWnd);

	DECLARE_MESSAGE_MAP()
};


