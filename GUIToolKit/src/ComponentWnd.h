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
	virtual LPCTSTR		ClassName( void ) const		{	return(_T(""));								}
	virtual DWORD		WindowStyle( void ) const	{	return(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);	}

public:
	BOOL	Create( CWnd& Parent, CRect& rect, INT nID, LPCTSTR lpszText = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0 ){
		m_hWnd	= CreateWindowEx( dwExStyle == 0 ? WindowExStyle() : dwExStyle, ClassName(), lpszText, dwStyle == 0 ? WindowStyle() : dwStyle, rect.X(), rect.Y(), rect.Width(), rect.Height(), Parent.m_hWnd, (HMENU)nID, GetModuleHandle(NULL), NULL );
		return(m_hWnd ? TRUE : FALSE);
	}
	BOOL	Create( CWnd* pParent, CRect* lpRect, INT nID, LPCTSTR lpszText = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0 ){
		return(CComponentWnd::Create(*pParent, *lpRect, nID, lpszText, dwStyle, dwExStyle));
	}
	BOOL	Create( CWnd* pParent, CRect& rect, INT nID, LPCTSTR lpszText = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0 ){
		return(CComponentWnd::Create(*pParent, rect, nID, lpszText, dwStyle, dwExStyle));
	}
	BOOL	Create( CWnd* pParent, int x, int y, int w, int h, INT nID, LPCTSTR lpszText = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0 ){
		return(CComponentWnd::Create(*pParent, CRect(x,y,w,h), nID, lpszText, dwStyle, dwExStyle));
	}

public:
	static void		InitCommCtrlEx( void ){
		static bool	fInit = false;
		if( fInit == false ){
			fInit = true;

			INITCOMMONCONTROLSEX	icc;
			icc.dwSize	= sizeof(INITCOMMONCONTROLSEX);
			icc.dwICC	= ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES;
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
	virtual LPCTSTR			ClassName( void ) const		{ return(WC_STATIC);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CButton	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{ return(WC_BUTTON);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CCheckBox	: public CButton {
protected:
	virtual DWORD			WindowStyle( void ) const	{ return(BS_AUTOCHECKBOX | CButton::WindowStyle());	}

public:
	int		GetCheck( void ) const			{ return Button_GetCheck(m_hWnd);	}
	void	SetCheck( int nCheck ) const	{ Button_SetCheck(m_hWnd, nCheck);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CRadioButton	: public CButton {
protected:
	virtual DWORD			WindowStyle( void ) const	{ return(BS_AUTORADIOBUTTON | CButton::WindowStyle());	}

public:
	int		GetState( void ) const			{ return Button_GetState(m_hWnd);	}
	void	SetState( int nState ) const	{ Button_SetState(m_hWnd, nState);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CGroupBox	: public CButton {
protected:
	virtual DWORD			WindowStyle( void ) const	{ return(BS_GROUPBOX | CButton::WindowStyle() | WS_CLIPSIBLINGS);	}
};

/**********************************************************************************
 * 
 *
 *
 */
class CEdit	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{ return(WC_EDIT);	}

public:
	int			GetText( LPTSTR lpch, int cchMax ) const			{ return GetWindowText(lpch, cchMax);	}
	int			GetTextLength( void ) const							{ return GetWindowTextLength();			}
	BOOL		SetText( LPCTSTR lpsz ) const						{ return SetWindowText(lpsz);			}

	void		LimitText( int cchMax ) const						{ return Edit_LimitText(m_hWnd, cchMax);		}

	int			GetLineCount( void ) const							{ return Edit_GetLineCount(m_hWnd);					}
	int			GetLine( int line, LPTSTR lpch, int cchMax ) const	{ return Edit_GetLine(m_hWnd, line, lpch, cchMax);	}

	DWORD		GetSel( void ) const					{ return Edit_GetSel(m_hWnd);					}
	void		SetSel( int nStart, int nEnd ) const	{ return Edit_SetSel(m_hWnd, nStart, nEnd);		}
	void		ReplaceSel( LPCTSTR lpszReplace ) const	{ return Edit_ReplaceSel(m_hWnd, lpszReplace);	}

	BOOL		GetModify( void ) const					{ return Edit_GetModify(m_hWnd);				}
	void		SetModify( UINT uModified ) const		{ return Edit_SetModify(m_hWnd, uModified);		}

	BOOL		ScrollCaret( void ) const				{ return Edit_ScrollCaret(m_hWnd);				}

	BOOL		CanUndo( void ) const					{ return Edit_CanUndo(m_hWnd);					}
	BOOL		Undo( void ) const						{ return Edit_Undo(m_hWnd);						}
	void		EmptyUndoBuffer( void ) const			{ return Edit_EmptyUndoBuffer(m_hWnd);			}

	void		SetPasswordChar( UINT ch ) const		{ return Edit_SetPasswordChar(m_hWnd, ch);		}


public:
	BOOL		MultiLine( void ) const {
		return ModifyStyle( 0, (ES_LEFT | ES_MULTILINE | ES_WANTRETURN), TRUE );
	}

	BOOL		ReadOnly( BOOL bOnOff ) const {
		DWORD	dwStyle	= (ES_READONLY);
		if( bOnOff )
			return ModifyStyle( 0, dwStyle, TRUE );
		return ModifyStyle( dwStyle, 0, TRUE );
	}

	BOOL		VScroll( BOOL bOnOff ) const {
		DWORD	dwStyle	= (WS_VSCROLL | ES_AUTOVSCROLL);
		if( bOnOff )
			return ModifyStyle( 0, dwStyle, TRUE );
		return ModifyStyle( dwStyle, 0, TRUE );
	}

	BOOL		HScroll( BOOL bOnOff ) const {
		DWORD	dwStyle	= (WS_HSCROLL | ES_AUTOHSCROLL);
		if( bOnOff )
			return ModifyStyle( 0, dwStyle, TRUE );
		return ModifyStyle( dwStyle, 0, TRUE );
	}
};

class CMultiEdit	: public CEdit {
protected:
	virtual DWORD			WindowStyle( void ) const	{ return(CEdit::WindowStyle() | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | ES_AUTOHSCROLL);	}
};

class CNumberEdit	: public CEdit {
protected:
	virtual DWORD			WindowStyle( void ) const	{ return(CEdit::WindowStyle() | ES_NUMBER);	}
};

class CPasswordEdit	: public CEdit {
protected:
	virtual DWORD			WindowStyle( void ) const	{ return(CEdit::WindowStyle() | ES_PASSWORD);	}
};



/**********************************************************************************
 * 
 *
 *
 */
class CUpDown	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{ return(UPDOWN_CLASS);	}
	virtual DWORD			WindowStyle( void ) const	{ return(CComponentWnd::WindowStyle() | UDS_ALIGNRIGHT | UDS_SETBUDDYINT );	}

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
	virtual LPCTSTR			ClassName( void ) const		{ return(WC_TABCONTROL);	}
	virtual DWORD			WindowStyle( void ) const	{ return(CComponentWnd::WindowStyle() | TCS_TOOLTIPS);	}

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

	int		AdjustRect( BOOL bLarger, CRect& rc ){
		return TabCtrl_AdjustRect(m_hWnd, bLarger, &rc);
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
	virtual DWORD			WindowStyle( void ) const	{	return(CComponentWnd::WindowStyle() | HDS_BUTTONS);	}

public:
	int		GetItemCount( void ) const {
		return Header_GetItemCount(m_hWnd);
	}

	int		InsertItem( int nItem, LPTSTR lpszCaption, int nWidth = 100 ){
		HD_ITEM	hdItem = {0};
		hdItem.mask			= HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
		hdItem.cxy			= nWidth;
		hdItem.pszText		= lpszCaption;
		hdItem.fmt			= HDF_CENTER | HDF_STRING;
		hdItem.cchTextMax	= lstrlen(hdItem.pszText);
		return InsertItem(nItem, &hdItem);
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

	BOOL	SetLayout( RECT& rc, WINDOWPOS& wpos ){
		HDLAYOUT	hdLayout;
		hdLayout.prc	= &rc;
		hdLayout.pwpos	= &wpos; 
		return Header_Layout(m_hWnd, &hdLayout); 
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

/**********************************************************************************
 *
 *
 *
 */
class CToolbar	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{	return(TOOLBARCLASSNAME);	}

public:
	void	ButtonStructSize( void ){
		CWnd::SendMessage( TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0 );
	}

	void	AddBitmap( LPTBADDBITMAP lpTbAddBitmap ){
		CWnd::SendMessage( TB_ADDBITMAP, 0, (LPARAM)lpTbAddBitmap );
	}

	void	AddButtons( int nButtons, LPTBBUTTON lpButtons ){
		CWnd::SendMessage( TB_ADDBUTTONS, (WPARAM)nButtons, (LPARAM)lpButtons );
	}

	void	AutoSize( void ){
		CWnd::SendMessage( TB_AUTOSIZE, 0, 0 );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CRebar	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{ return(REBARCLASSNAME);	}
	virtual DWORD			WindowStyle( void ) const	{ return(CComponentWnd::WindowStyle() |/* RBS_AUTOSIZE |*/ RBS_BANDBORDERS | CCS_NODIVIDER);	}
	virtual DWORD			WindowExStyle( void ) const	{ return(WS_EX_TOOLWINDOW);	}

public:
	void		SetBarInfo( LPREBARINFO lpRebarInfo ){
		CWnd::SendMessage( RB_SETBARINFO, 0, (LPARAM)lpRebarInfo );
	}

	void		SetBarInfo( void ){
		REBARINFO info = {0};
		info.cbSize	= sizeof(REBARINFO);
		this->SetBarInfo( &info );
	}

	void		InsertBand( LPREBARBANDINFO lpRebarBandInfo ){
		CWnd::SendMessage( RB_INSERTBAND, (WPARAM)-1, (LPARAM)lpRebarBandInfo );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CComboBox	: public CComponentWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const		{ return(WC_COMBOBOX);	}
	virtual DWORD			WindowStyle( void ) const	{ return(CComponentWnd::WindowStyle() | CBS_DROPDOWNLIST | CBS_HASSTRINGS);	}

public:
	int			GetText( LPTSTR lpszText, int szText ) const {
		return GetWindowText(lpszText, szText);
	}

	int			GetTextLength( void ) const {
		return GetWindowTextLength();
	}

	BOOL		SetText( LPCTSTR lpszText ) const {
		return SetWindowText(lpszText);
	}

	int			GetCount( void ) const {
		return ComboBox_GetCount(m_hWnd);
	}

	int			ResetContent( void ) const {
		return ComboBox_ResetContent(m_hWnd);
	}

	int			AddString( LPCTSTR lpszText ){
		return ComboBox_AddString(m_hWnd, lpszText);
	}

	int			InsertString( int idx, LPCTSTR lpszText ) const {
		return ComboBox_InsertString(m_hWnd, idx, lpszText);
	}

	int			DeleteString( int idx ) const {
		return ComboBox_DeleteString(m_hWnd, idx);
	}

	int			GetLBTextLen( int idx ) const {
		return ComboBox_GetLBTextLen(m_hWnd, idx);
	}

	int			GetLBText( int idx, LPTSTR lpszText ) const {
		return ComboBox_GetLBText(m_hWnd, idx, lpszText);
	}

	int			GetCurSel( void ) const {
		return ComboBox_GetCurSel(m_hWnd);
	}

	int			SetCurSel( int nCurSel ) const {
		return ComboBox_SetCurSel(m_hWnd, nCurSel);
	}

};

/**********************************************************************************
 *
 *
 *
 */


/**********************************************************************************
 *
 *
 *
 */
