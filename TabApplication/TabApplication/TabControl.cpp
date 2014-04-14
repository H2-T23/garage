// TabControl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TabApplication.h"
#include "TabControl.h"


// CTabControl

IMPLEMENT_DYNAMIC(CTabControl, CTabCtrl)

CTabControl::CTabControl()
	: m_pActiveWnd(NULL)
{
}

CTabControl::~CTabControl()
{
}

BEGIN_MESSAGE_MAP(CTabControl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CTabControl::OnTcnSelchange)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CTabControl メッセージ ハンドラ
int		CTabControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ここに特定な作成コードを追加してください。

	return 0;
}

void CTabControl::OnDestroy()
{
	CTabCtrl::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
}

void CTabControl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	int idx = GetCurSel();

//	int i=0;
//	std::vector<CWnd*>::iterator it = m_vecWnd.begin();
//	while( it != m_vecWnd.end() ){
//		(*it)->ShowWindow( i == idx ? SW_SHOW : SW_HIDE );
//		i++; it++;
//	}
	SetActivePage( idx );

	Resize();

	*pResult = 0;
}

void CTabControl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);

	// TODO: ここにメッセージ ハンドラ コードを追加します。
	Resize();
}

void CTabControl::OnSetFocus(CWnd* pOldWnd)
{
	CTabCtrl::OnSetFocus(pOldWnd);

	// TODO: ここにメッセージ ハンドラ コードを追加します。
	CWnd* pWnd = GetActivePage();
	if( pWnd ){
		pWnd->SetFocus();
	}
}
