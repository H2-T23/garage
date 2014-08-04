// TreeCtrlDockablePane.cpp : 実装ファイル
//


#include "TreeCtrlDockablePane.h"


// TreeCtrlDockablePane

IMPLEMENT_DYNAMIC(CTreeCtrlDockablePane, CDockablePane)

CTreeCtrlDockablePane::CTreeCtrlDockablePane()
{

}

CTreeCtrlDockablePane::~CTreeCtrlDockablePane()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlDockablePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// TreeCtrlDockablePane メッセージ ハンドラー




int CTreeCtrlDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: ここに特定な作成コードを追加してください。
	BOOL	bResult = FALSE;
	
	DWORD	dwStyle		= (WS_CHILD | WS_VISIBLE | WS_BORDER/* | WS_DLGFRAME | WS_THICKFRAME*/);
	DWORD	dwExStyle	= (0);

	bResult	= m_TreeCtrl.Create( NULL, _T("DragAndDropTree"), dwStyle, CRect(10, 10, 300, 300), this, 1001);
	if( !bResult ){
		return -1;
	}

	return 0;
}

void	CTreeCtrlDockablePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	m_TreeCtrl.MoveWindow( 0, 0, cx, cy );
}
