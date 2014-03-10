#include "VirtualListCtrl.h"
#include "Log.h"

/**********************************************************************************
 *
 *
 *
 */
IMPLEMENT_DYNAMIC(CVirtualListCtrl, CListCtrl)
/**********************************************************************************
 *
 *
 *
 */
CVirtualListCtrl::CVirtualListCtrl()
{
}
/**********************************************************************************
 *
 *
 *
 */
CVirtualListCtrl::~CVirtualListCtrl()
{
}
/**********************************************************************************
 *
 *
 *
 */
BOOL	CVirtualListCtrl::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CListCtrl::PreCreateWindow(cs) ){
		return FALSE;
	}

	cs.dwExStyle	|= (WS_EX_CLIENTEDGE);
	cs.style		|= (LVS_REPORT | LVS_OWNERDATA);

	return TRUE;
}
/**********************************************************************************
 *
 *
 *
 */
BEGIN_MESSAGE_MAP(CVirtualListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CVirtualListCtrl::OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CVirtualListCtrl::OnNMDblclk)
END_MESSAGE_MAP()
// CVirtualListCtrl メッセージ ハンドラ
/**********************************************************************************
 *
 *
 *
 */
int		CVirtualListCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CListCtrl::OnCreate(lpCreateStruct) == -1 ){
		return -1;
	}
	
	SetExtendedStyle( LVS_EX_FULLROWSELECT /*| LVS_EX_GRIDLINES*/ );

	m_nPrevCount	= 0;
	m_pLogList		= CLogManager::GetInstance().GetLogLlist();

	InsertColumn( 0, _T("ID")			, LVCFMT_LEFT, 50 );
	InsertColumn( 1, _T("Time")			, LVCFMT_LEFT, 100 );
	InsertColumn( 2, _T("Text")			, LVCFMT_LEFT, 1024 );

	//CString		strBody;
	//strBody	= _T("0123456789");

	//m_ItemList.Append( _T("00001") );
	//m_ItemList.Append( _T("00002") );
	//m_ItemList.Append( _T("00003") );
	//m_ItemList.Append( _T("00004") );
	//m_ItemList.Append( _T("00005") );
	//m_ItemList.Append( _T("00006") );
	//m_ItemList.Append( _T("00007") );
	//m_ItemList.Append( _T("00008"), (strBody.GetLength()+1)*sizeof(TCHAR), (LPVOID)(LPCTSTR)strBody );
	//Sleep( 100 );
	//CString	strSubject;
	//for( int i=9; i<1024; i++ ){
	//	strSubject.Format(_T("Subject:%06d"), i);
	//	m_ItemList.Append( strSubject );
	//}
	//SetItemCountEx( (int)m_ItemList.Count() );
	SetItemCountEx( int(0xFFFF) );

	SetTimer( 1111, 1000/2, NULL );
	return 0;
}
/**********************************************************************************
 *
 *
 *
 */
void	CVirtualListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
	KillTimer( 1111 );
	m_pLogList->RemoveAll();
}
/********************************************************************************************************************************************************************
 *
 *
 *
 */
void	CVirtualListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if( nIDEvent == 1111 )
	{
		int nCount	= m_pLogList->Count();
		if( nCount > m_nPrevCount )
		{
			EnsureVisible( nCount-1, FALSE );
			m_nPrevCount	= nCount;
	
		}
		UpdateData();
		return;
	}

	CListCtrl::OnTimer(nIDEvent);
}
/**********************************************************************************
 *
 *
 *
 */
void	CVirtualListCtrl::OnLvnGetdispinfo( NMHDR* pNMHDR, LRESULT*	pResult )
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	LVITEM&	LVItem = pDispInfo->item;

	if( LVItem.mask & LVIF_TEXT )
	{
	//	CString	str;
	//	str.AppendFormat(_T("Item:%d SubItem:%d\t"), LVItem.iItem, LVItem.iSubItem);
	//	str.AppendFormat(_T("lParam:0x%X\t"), LVItem.lParam);
	//	str.AppendFormat(_T("mask:0x%X\t"), LVItem.mask);
	//	str.AppendFormat(_T("state:0x%X\t"), LVItem.state);
	//	str.AppendFormat(_T("stateMask:0x%X\t"), LVItem.stateMask);
	//	str.AppendFormat(_T("cchTextMax:%d\t"), LVItem.cchTextMax);
	//	str.AppendFormat(_T("pszText:%s\n"), LVItem.pszText?LVItem.pszText:_T("NULL"));

	//	TRACE( str );

		CLogItem*	pItem	= m_pLogList->Find( (DWORD)LVItem.iItem );
		if( pItem )
		{
			int	nLen = 0;
			switch( LVItem.iSubItem ){
			case 0:
				_stprintf( LVItem.pszText, _T("%d"), pItem->ID() );
				break;
			case 1:
				_tcscpy( LVItem.pszText, pItem->Time() );
				break;
			case 2:
				_tcscpy( LVItem.pszText, pItem->Subject() );
				break;
			}
		}
	}

 	*pResult = 0;
}
/********************************************************************************************************************************************************************
 *
 *
 *
 */
void	CVirtualListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	CLogItem*	pItem = m_pLogList->Find( pNMItemActivate->iItem  );
	if( pItem && pItem->IsValidBody() )
	{
		GetParent()->SendMessage( WM_SET_MULTITEXT, 0, (LPARAM)pItem->Body() );
	}
	else
	{
		GetParent()->SendMessage( WM_SET_MULTITEXT, 0, 0 );
	}

	*pResult = 0;
}

