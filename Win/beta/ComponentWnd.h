#pragma once
#pragma comment(lib,"comctl32.lib")
#include <commctrl.h>
#include "Wnd.h"

/**********************************************************************************
 *
 *
 *
 */
class CComponentWnd : public CWnd {
protected:
	virtual LPCTSTR		ClassName( void ) const		{	return(_T(""));					}
	virtual DWORD		WindowStyle( void ) const	{	return(WS_CHILD | WS_VISIBLE);	}

public:
	BOOL	Create( CWnd* pParent, CRect& rect, INT nID, LPCTSTR lpszText = NULL ){
		m_hWnd	= CreateWindowEx( WindowExStyle(), ClassName(), lpszText, WindowStyle(), rect.X(), rect.Y(), rect.Width(), rect.Height(), pParent->m_hWnd, (HMENU)nID, GetModuleHandle(NULL), NULL );
		return(m_hWnd ? TRUE : FALSE);
	}

public:
	static void		InitCommCtrlEx( void ){
		static bool	fInit = false;
		if( fInit == false ){
			fInit = true;

			INITCOMMONCONTROLSEX	icc;
			icc.dwSize	= sizeof(INITCOMMONCONTROLSEX);
			icc.dwICC	= ICC_LISTVIEW_CLASSES;
			InitCommonControlsEx( &icc );
		}
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CLabel : public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const {	return(_T("STATIC"));	}
};

/**********************************************************************************
 *
 *
 *
 */
class CButton : public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const {	return(_T("BUTTON"));	}
};

/**********************************************************************************
 *
 *
 *
 */
class CListView : public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{	return(WC_LISTVIEW);	}
	virtual DWORD			WindowStyle( void ) const	{	return(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT);	}

public:
	int		InsertColumn( int idx, LPTSTR lpszText, int nWidth = 0 ){
		LVCOLUMN	col = {0};
		col.mask	= LVCF_WIDTH | LVCF_TEXT;
		col.cx		= nWidth ? nWidth : 100;
		col.pszText	= lpszText;
		ListView_InsertColumn( m_hWnd, idx, &col );
		return idx;
	}

	int		InsertItem( int nItem, int nSubItem, LPTSTR lpszText ){
		LVITEM		item = {0};
		item.mask		= LVIF_TEXT;
		item.iItem		= nItem;
		item.iSubItem	= nSubItem;
		item.pszText	= lpszText;
		ListView_InsertItem( m_hWnd, &item );
		return nItem;
	}

	int		SetItem( int nItem, int nSubItem, LPTSTR lpszText ){
		LVITEM		item = {0};
		item.mask		= LVIF_TEXT;
		item.iItem		= nItem;
		item.iSubItem	= nSubItem;
		item.pszText	= lpszText;
		ListView_SetItem( m_hWnd, &item );
		return nItem;
	}

	int		SetItemCountEx( int nCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL ){
		ListView_SetItemCountEx(m_hWnd, nCount, dwFlags);
		return nCount;
	}

	void	SetExtendedListViewStyleEx( DWORD dwExMask, DWORD dwExStyle ){
		ListView_SetExtendedListViewStyleEx(m_hWnd, dwExMask, dwExStyle );
	}

	int		GetItemCount( void ) const {
		return ListView_GetItemCount(m_hWnd);
	}

	void	GetItem( LVITEM& item ){
		ListView_GetItem(m_hWnd, &item);
	}
	
	DWORD	GetItemState( int nItem, DWORD dwMask ){
		return ListView_GetItemState(m_hWnd, nItem, dwMask);
	}

	BOOL	GetCheckState( UINT nIdx ) const {
		return ListView_GetCheckState(m_hWnd, nIdx);
	}

	int		HitTest( HWND hwndFrom, LVHITTESTINFO* pHitTest ){
		return ListView_HitTest(hwndFrom, pHitTest);
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CVirtualListView : public CListView {
protected:
	virtual DWORD			WindowStyle( void ) const	{	return(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_OWNERDATA);	}
};
