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
class CButton	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const {	return(_T("BUTTON"));	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CTabCtrl	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const { return(WC_TABCONTROL);	}
	virtual DWORD			WindowStyle( void ) const { return(CComponentWnd::WindowStyle() | TCS_TOOLTIPS);	}

public:
	int		InsertItem( int nItem, TCITEM& tcItem ){
		return TabCtrl_InsertItem(m_hWnd, nItem, &tcItem);
	}

	int		InsertItem( int nItem, LPTSTR lpszCaption ){
		TCITEM	tcItem;
		tcItem.mask		= TCIF_TEXT;
		tcItem.pszText	= lpszCaption;
		return InsertItem(nItem, tcItem);
	}

	BOOL	DeleteItem( int nItem ){
		return TabCtrl_DeleteItem(m_hWnd, nItem);
	}

	BOOL	DeleteAllItems( void ){
		return TabCtrl_DeleteAllItems(m_hWnd);
	}

	BOOL	GetItemRect( int nItem, LPRECT lpRect ) const {
		return TabCtrl_GetItemRect(m_hWnd, nItem, lpRect);
	}

	int		SetCurSel( int nItem ){
		return TabCtrl_SetCurSel(m_hWnd, nItem);
	}

	int		GetCurSel( void ) const {
		return TabCtrl_GetCurSel(m_hWnd);
	}

	int		HitTest( LPTCHITTESTINFO lpInfo ){
		return TabCtrl_HitTest(m_hWnd, lpInfo );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CHeaderCtrl : public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{	return(WC_HEADER);	}

public:
	int		GetItemCount( void ) const {
		return Header_GetItemCount(m_hWnd);
	}

	int		InsertItem( int nItem, LPTSTR lpszCaption, int nWidth = 100 ){
		HD_ITEM	hdItem;
		hdItem.mask			= HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
		hdItem.pszText		= lpszCaption;
		hdItem.fmt			= HDF_CENTER | HDF_STRING;
		hdItem.cchTextMax	= lstrlen(hdItem.pszText);
		return InsertItem(nItem, &hdItem );
	}

	int		InsertItem( int nItem, HD_ITEM* pItem ){
		return Header_InsertItem(m_hWnd, nItem, pItem);
	}

	int		DeleteItem( int nItem ){
		return Header_DeleteItem(m_hWnd, nItem);
	}

	BOOL	GetItem( int nItem, HD_ITEM* pItem ){
		return Header_GetItem(m_hWnd, nItem, pItem);
	}

	BOOL	SetItem( int nItem, HD_ITEM* pItem ){
		return Header_SetItem(m_hWnd, nItem, pItem);
	}

	BOOL	GetItemRect( int nItem, LPRECT lpRect ){
		return Header_GetItemRect(m_hWnd, nItem, lpRect);
	}
};


/**********************************************************************************
 *
 *
 *
 */
class CListView : public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{	return(WC_LISTVIEW);	}
	virtual DWORD			WindowStyle( void ) const	{	return(CComponentWnd::WindowStyle() | LVS_REPORT);	}

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

	int		HitTest( LVHITTESTINFO* pHitTest ){
		return ListView_HitTest(m_hWnd, pHitTest);
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
