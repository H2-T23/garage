// CtrlWnd.h 
#pragma once

#include <commctrl.h>

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

#include <vector>

#include "Wnd.h"

/**********************************************************************************
 *
 *
 *
 */
class ICtrl {
public:
	virtual void		Update( void ) = 0;
};

/**********************************************************************************
 *
 *
 *
 */
class CCtrlWnd	: public CWnd
				, public ICtrl
{
protected:
	virtual DWORD		DefaultStyle( void ) const {
		return(WS_CHILD | WS_VISIBLE);
	}

	virtual DWORD		DefaultExstyle( void ) const {
		return 0;
	}
	
	virtual LRESULT		WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		if( uMsg == WM_COMMAND ){
			MessageBox(NULL,_T("WM_COMMAND"),_T("OK"),MB_OK);
		}
		return CWnd::WindowProc(uMsg,wParam,lParam);
	}
	
public:
	virtual void		Update( void ) = 0;
	
	BOOL	Create(	LPCTSTR		lpszWindowText
				,	int			nX
				,	int			nY
				,	int			nWidth
				,	int			nHeight
				,	CWnd*		pParentWnd
				,	HMENU 		hResourceID
				,	DWORD		dwStyle				= 0
				,	DWORD		dwExStyle			= 0 )
	{
		return	CCtrlWnd::Create( lpszWindowText, nX, nY, nWidth, nHeight, (HWND)(*pParentWnd), hResourceID, dwStyle, dwExStyle );
	}
	
	BOOL	Create(	LPCTSTR		lpszWindowText
				,	int			nX
				,	int			nY
				,	int			nWidth
				,	int			nHeight
				,	HWND		pParentWnd
				,	HMENU 		hResourceID
				,	DWORD		dwStyle				= 0
				,	DWORD		dwExStyle			= 0 )
	{
		m_hWnd = CreateWindowEx(	(dwExStyle ? dwExStyle : DefaultExstyle())
								,	ClassName()
								,	lpszWindowText
								,	(dwStyle ? dwStyle : DefaultStyle())
								, 	nX
								,	nY
								,	nWidth
								,	nHeight
								,	pParentWnd
								,	hResourceID
								,	GetModuleHandle(NULL)
								,	NULL
								);
		return(m_hWnd == NULL ? FALSE : TRUE);
	}

	
	HWND	SetParent( CWnd* pWnd ){
		return ::SetParent( m_hWnd, (HWND)*pWnd );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CButtonWnd : public CCtrlWnd {
public:
	virtual LPCTSTR	ClassName( void ) const {
		return WC_BUTTON;
	}
	
	virtual void	Update( void ){
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CStaticWnd : public CCtrlWnd {
public:
	virtual LPCTSTR	ClassName( void ) const {
		return WC_STATIC;
	}

	virtual void	Update( void ){
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CComboBoxWnd : public CCtrlWnd {
protected:
	virtual DWORD	DefaultStyle( void ) const {
		return(CCtrlWnd::DefaultStyle() | CBS_DROPDOWNLIST);
	}
public:
	virtual LPCTSTR	ClassName( void ) const {
		return WC_COMBOBOX;
	}

	virtual void	Update( void ){
	}
	
public:
	void			AddString( LPCTSTR lpszItem ){
		SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpszItem );
	}
	
	int				GetCurSel( void ) const {
		return(int)SendMessage(m_hWnd, CB_GETCURSEL, 0, 0);
	}
	
	void			GetItemString( int nIdx, LPTSTR lpszText ){
		SendMessage(m_hWnd, CB_GETLBTEXT, nIdx, (LPARAM)lpszText);
	}
};

/**********************************************************************************
 *
 *
 *
class CPanelWnd : public CStaticWnd {
public:
	typedef std::vector<CCtrlWnd*>		VECTER_CTRLWND;
	VECTER_CTRLWND		m_vecCtrls;
	
public:
	virtual LPCTSTR	ClassName( void ) const {
		return CStaticWnd::ClassName();
	}
	
	virtual void	Update( void ){
	}
	
	void	Append( CCtrlWnd* pCtrlWnd ){
		if( pCtrlWnd->SetParent( this ) )
			m_vecCtrls.push_back( pCtrlWnd );
	}

	virtual LRESULT		WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam){
		MessageBox(NULL, _T("ボタンが押されました。"), _T("OK"), MB_OK);
		if( uMsg == WM_COMMAND )
		{
			MessageBox(NULL, _T("ボタンが押されました。"), _T("OK"), MB_OK);
			return 0;
		}
		return CCtrlWnd::WindowProc(uMsg,wParam,lParam);
	}
}; 
 */

