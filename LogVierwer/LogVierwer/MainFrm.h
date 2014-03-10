
// MainFrm.h : CMainFrame �N���X�̃C���^�[�t�F�C�X
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

// ����
public:

// ����
public:

// �I�[�o�[���C�h
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// ����
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

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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


