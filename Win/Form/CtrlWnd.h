#pragma once

#include "Wnd.h"
#include <commctrl.h>

#pragma comment(lib,"comctl32.lib")

/**********************************************************************************
 *
 *
 *
 */
class CCtrlWnd : public CWnd {
protected:
	virtual LPCTSTR		ClassName( void ) const		{	return(_T(""));					}
	virtual DWORD		WindowStyle( void ) const	{	return(WS_CHILD | WS_VISIBLE);	}

public:
	BOOL	Create( CWnd* pParent, CRect& rect, INT nID, LPCTSTR lpszText = NULL ){
		m_hWnd	= CreateWindowEx( WindowExStyle(), ClassName(), lpszText, WindowStyle(), rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), pParent->m_hWnd, (HMENU)nID, GetModuleHandle(NULL), NULL );
		return(m_hWnd ? TRUE : FALSE);
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CLabel : public CCtrlWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const {	return(_T("STATIC"));	}
};

/**********************************************************************************
 *
 *
 *
 */
class CButton : public CCtrlWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const {	return(_T("BUTTON"));	}
};

/**********************************************************************************
 *
 *
 *
 */
class CListView : public CCtrlWnd {
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
};
