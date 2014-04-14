// VirtualListCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TabApplication.h"
#include "VirtualListCtrl.h"


// CVirtualListCtrl
/*******************************************************************************************
 *	
 *	
 *	
 */
IMPLEMENT_DYNAMIC(CVirtualListCtrl, CListCtrl)

/*******************************************************************************************
 *	
 *	
 *	
 */
CVirtualListCtrl::CVirtualListCtrl()
	: m_nMaxItemNum(USHRT_MAX)
{
}

/*******************************************************************************************
 *	
 *	
 *	
 */
CVirtualListCtrl::~CVirtualListCtrl()
{
}

/*******************************************************************************************
 *	
 *	
 *	
 */
BEGIN_MESSAGE_MAP(CVirtualListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CVirtualListCtrl::OnLvnGetdispinfo)
//	ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, &CVirtualListCtrl::OnLvnOdcachehint)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CVirtualListCtrl メッセージ ハンドラ
/*******************************************************************************************
 *	
 *	
 *	
 */
BOOL	CVirtualListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	cs.style |= (LVS_REPORT | LVS_OWNERDATA | LVS_SINGLESEL);
	return CListCtrl::PreCreateWindow(cs);
}

/*******************************************************************************************
 *	
 *	
 *	
 */
int		CVirtualListCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ここに特定な作成コードを追加してください。
	SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT /*| LVS_EX_GRIDLINES*/ );
	SetItemCount( USHRT_MAX );

	InsertColumn( 0, _T(""), LVCFMT_LEFT, SHRT_MAX/2 );

	SetTimer( 100, 200, NULL );

	return 0;
}

/*******************************************************************************************
 *	
 *	
 *	
 */
void	CVirtualListCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
/*
	LVIF_TEXT	: pszTextメンバを指定する必要がある
	LVIF_IMAGE	: iImageメンバを指定する必要がある 
	LVIF_INDENT	: iIndentメンバを指定する必要がある
	LVIF_PARAM	: iParamメンバを指定する必要がある
	LVIF_STATE	: stateメンバを指定する必要がある
*/

	LV_ITEM*	pItem = &(pDispInfo)->item;
	int	iItem	= pItem->iItem;

//	if( m_lstString.GetSize() <= iItem ){
//		return;
//	}
	int num = m_lstString.GetCount();
	if( !((0 < iItem) && (iItem < num)) ){
		return;
	}

	if( pItem->mask & LVIF_TEXT )
	{
		switch( pItem->iSubItem ){
		case 0:	// fill in main text.
			{
				CString	str;
				str.Format( _T("%d[%s]"),iItem, m_lstString.GetAt( m_lstString.FindIndex(iItem) ) );
				_tcscpy_s( pItem->pszText, pItem->cchTextMax, (LPCTSTR)str );
			}
			break;
		case 1:	// fill in sub item 1 text.
			{
//				LPCTSTR	lpszText = m_lstString.GetAt( (POSITION)iItem );
//				if( lpszText ){
//					_tcscpy_s( pItem->pszText, pItem->cchTextMax, lpszText );
//				}
			}
			break;
		case 2:	// fill in sub item 1 text.
			break;
		}
	}

//	if( pItem->mask & LVIF_IMAGE ){	// valid image ?
//		pItem->iImage	= m_Items[ iItem ].m_iImage;
//	}

	*pResult = 0;
}

/*******************************************************************************************
 *	
 *	
 *	
 */
void	CVirtualListCtrl::OnLvnOdcachehint( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	
	// Update the cache with the recommended range.
	for( int i=pCacheHint->iFrom; i<=pCacheHint->iTo; i++ ){
		
	}

	*pResult = 0;
}

/*******************************************************************************************
 *	
 *	
 *	
 */
void	CVirtualListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if( nIDEvent == 100 ){
		CString	str;
		str.Format( _T("%d"), GetTickCount() );
		AppendText( str );
		return;
	}

	CListCtrl::OnTimer(nIDEvent);
}
