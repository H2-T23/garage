// ListCtrlDockablePane.cpp : 実装ファイル
//


#include "ListCtrlDockablePane.h"


// CListCtrlDockablePane

IMPLEMENT_DYNAMIC(CListCtrlDockablePane, CDockablePane)

CListCtrlDockablePane::CListCtrlDockablePane()
{

}

CListCtrlDockablePane::~CListCtrlDockablePane()
{
}


BEGIN_MESSAGE_MAP(CListCtrlDockablePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CListCtrlDockablePane メッセージ ハンドラー

int		CListCtrlDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CDockablePane::OnCreate(lpCreateStruct) == -1 ){
		return -1;
	}

	// TODO: ここに特定な作成コードを追加してください。
	BOOL	bResult = FALSE;
	
	DWORD	dwStyle		= (WS_CHILD | WS_VISIBLE | WS_BORDER/* | WS_DLGFRAME | WS_THICKFRAME*/);
	DWORD	dwExStyle	= (0);

	bResult	= m_ListCtrl.Create( NULL, _T("DragAndDropList"), dwStyle, CRect(10, 10, 300, 300), this, 1001);
	if( !bResult ){
		return -1;
	}

	return 0;
}

void	CListCtrlDockablePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	m_ListCtrl.MoveWindow(0, 0, cx, cy);
}
