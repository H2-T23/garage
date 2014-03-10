
// MainFrm.h : CMainFrame クラスのインターフェイス
//

#pragma once
#include "ChildView.h"

#include "TabControl.h"
#include "PropertyPanel.h"
#include "ListPanel.h"

/**********************************************************************************
 *
 *
 *
 */
class CMainFrame : public CFrameWnd {
public:
	enum {
		IDC_TABCTRL			= 1001	,
		IDC_TAB_PROPERTY			,
		IDC_TAB_LIST				,
		IDC_TAB_DEFAULT				,
	};
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

	CTabControl		m_wndTabCtrl;
	CPropertyPanel	m_wndPropertyTab;
	CListPanel		m_wndListTab;
	CChildView		m_wndViewTab;

	CWnd*			m_pActiveWnd;

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSetFocus(CWnd *pOldWnd);
	afx_msg void	OnSize(UINT nType, int cx, int cy){
		m_wndTabCtrl.TabResize(cx,cy);
		CFrameWnd::OnSize(nType, cx, cy);
	}
	afx_msg void	OnTabSelChange( NMHDR* lpNmhdr, LRESULT* pResult ){
		*pResult = 0;
		m_pActiveWnd	= m_wndTabCtrl.TabSelChange( m_wndTabCtrl.GetCurSel() );
	}
};


