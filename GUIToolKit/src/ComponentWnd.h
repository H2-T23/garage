#pragma once
#pragma comment(lib,"comctl32.lib")
#include <commctrl.h>

#include "Wnd.h"



/*
WC_BUTTON				_T("BUTTON")
WC_STATIC				_T("STATIC")
WC_EDIT					_T("EDIT")
WC_LISTBOX				_T("LISTBOX")
WC_COMBOBOX				_T("COMBOBOX")
WC_SCROLLBAR			_T("SCROLLBAR")
WC_HEADER				_T("SysHeader32")
WNDCLS_ANIMATE				_T("SysAnimate32")
WNDCLS_HOTKEY				_T("msctls_hotkey32")
WNDCLS_PROGRESS				_T("msctls_progress32")
WNDCLS_STATUS				_T("msctls_statusbar32")
WNDCLS_TOOLBAR				_T("ToolbarWindow32")
WNDCLS_TOOLTIP				_T("tooltips_class32")
WNDCLS_TRACKBAR				_T("msctls_trackbar32")
WNDCLS_UPDOWN				_T("msctls_updown32")
WNDCLS_COMBOBOX_EX			_T("ComboBoxEx32")
WNDCLS_HEADER				_T("SysHeader32")
WNDCLS_LISTVIEW				_T("SysListView32")
WNDCLS_TAB_CONTROL			_T("SysTabControl32")	
WNDCLS_TREEVIEW				_T("SysTreeView32")
WNDCLS_DATETIME				_T("SysDateTimePick32")
WNDCLS_MONTH				_T("SysMonthCal32")
WNDCLS_REBAR		REBARCLASSNAME\		_T("ReBarWindow32")
WNDCLS_IPADDRESS	WC_IPADDRESS		_T("SysIPAddress32")
WNDCLS_PAGER		WC_PAGESCROLLER		_T("SysPager")
*/

/**********************************************************************************
 *
 *
 *
 */
class CComponentWnd : public CWnd {
protected:
	virtual LPCTSTR		ClassName( void ) const		{	return(_T(""));					}
	virtual DWORD		WindowStyle( void ) const	{	return(WS_CHILD | WS_VISIBLE | WS_BORDER);	}

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
			icc.dwICC	= ICC_WIN95_CLASSES;
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
	virtual LPCTSTR			ClassName( void ) const		{	return(WC_STATIC);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CButton	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{	return(WC_BUTTON);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CEdit	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{	return(WC_EDIT);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CUpDown	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{	return(UPDOWN_CLASS);	}
	virtual DWORD			WindowStyle( void ) const	{	return(CComponentWnd::WindowStyle() | UDS_ALIGNRIGHT | UDS_SETBUDDYINT );	}

public:
	void	SetBuddy( CWnd* pBuddyWnd ){
		SendMessage( UDM_SETBUDDY, (WPARAM)pBuddyWnd->m_hWnd, 0 );
	}

	void	SetRange( int nMin, int nMax ){
		SendMessage( UDM_SETRANGE, 0, MAKELPARAM(nMin,nMax) );
	}

	void	SetPos( int nPos ){
		SendMessage( UDM_SETPOS, 0, MAKELPARAM(nPos,0) );
	}
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
	virtual DWORD			WindowStyle( void ) const	{	return(CComponentWnd::WindowStyle() | LVS_REPORT | LVS_OWNERDATA);	}
};
