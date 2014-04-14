// VirtualListCtrl.cpp : �����t�@�C��
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

// CVirtualListCtrl ���b�Z�[�W �n���h��
/*******************************************************************************************
 *	
 *	
 *	
 */
BOOL	CVirtualListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
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

	// TODO:  �����ɓ���ȍ쐬�R�[�h��ǉ����Ă��������B
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
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
/*
	LVIF_TEXT	: pszText�����o���w�肷��K�v������
	LVIF_IMAGE	: iImage�����o���w�肷��K�v������ 
	LVIF_INDENT	: iIndent�����o���w�肷��K�v������
	LVIF_PARAM	: iParam�����o���w�肷��K�v������
	LVIF_STATE	: state�����o���w�肷��K�v������
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
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	
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
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if( nIDEvent == 100 ){
		CString	str;
		str.Format( _T("%d"), GetTickCount() );
		AppendText( str );
		return;
	}

	CListCtrl::OnTimer(nIDEvent);
}
