// HowtoUse.cpp 
#include <stdio.h>
#include "Wnd.h"

template<typename T>
class TSize {
public:
	T		cx, cy;
};


template<typename T>
class TRect {
public:
	T		x, y, w, h;
};

typedef TRect<int> CRect;


class CCtrl : public CWnd {
public:
	DWORD		m_dwStyle;
	DWORD		m_dwExStyle;
	
	CCtrl(DWORD dwStyle, DWORD dwExStyle = 0) : m_dwStyle(dwStyle), m_dwExStyle(dwExStyle) {}
};


class CStatic : public CCtrl {
public:
	CStatic( void )
	: CCtrl((WS_CHILD | WS_VISIBLE)){}

	BOOL	Create( LPCTSTR lpCaption, CRect& rect, CWnd* pParentWnd, UINT uID, HINSTANCE hInstance ){
		m_hWnd =  CreateWindowEx( m_dwExStyle, _T("static"), lpCaption, m_dwStyle, rect.x, rect.y, rect.w, rect.h, pParentWnd->m_hWnd, (HMENU)uID, hInstance, NULL );
		return(m_hWnd ? TRUE : FALSE);
	}
};

class CButton : public CCtrl {
public:
	CButton( void )
	: CCtrl((WS_CHILD | WS_VISIBLE)){}
	
	BOOL	Create( LPCTSTR lpCaption, CRect& rect, CWnd* pParentWnd, UINT uID, HINSTANCE hInstance ){
		m_hWnd =  CreateWindowEx( m_dwExStyle, _T("button"), lpCaption, m_dwStyle, rect.x, rect.y, rect.w, rect.h, pParentWnd->m_hWnd, (HMENU)uID, hInstance, NULL );
		return(m_hWnd ? TRUE : FALSE);
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CInputPanel : public CPanelWnd {
protected:
	CStatic		m_stc;
	CButton		m_btnA, m_btnB;
	
	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		
		MessageBox(NULL, _T("InputPanel"), _T("OK"), MB_OK);
		
		CRect	rect;
		rect.x = 10;
		rect.y = 10;
		rect.w = 100;
		rect.h = 30;
		
		m_stc.Create( _T("static text"), rect, this, 1000, lpCreateStruct->hInstance );
		
		rect.y += rect.h;
		
		if( m_btnA.Create( _T("A"), rect, this, 1001, lpCreateStruct->hInstance ) ){
			MessageBox(NULL,_T("Create Button A."),_T("OK"),MB_OK);
		}
		
		rect.x += rect.w;
		
		if( m_btnA.Create( _T("B"), rect, this, 1002, lpCreateStruct->hInstance ) ){
			MessageBox(NULL,_T("Create Button B."),_T("OK"),MB_OK);
		}
		return TRUE;
	}
	
	virtual void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		switch(uID){
		case 1001:
			MessageBox(NULL,_T("button A click"),_T("OK"),MB_OK);
			break;
		case 1002:
			MessageBox(NULL,_T("button B click"),_T("OK"),MB_OK);
			break;
		}
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CSampleFrame : public CFrameWnd {
protected:
	CInputPanel		m_wndInput;
	
	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( IsWindow(m_hWnd) )
			MessageBox( NULL, _T("IsWindow()==TRUE."), _T("OK"), MB_OK );
		else
			MessageBox( NULL, _T("IsWindow()==FALSE."), _T("OK"), MB_OK );
		
		m_wndInput.Create(_T(""),WS_BORDER | WS_CHILD | WS_VISIBLE,0,300,0,300,300,m_hWnd);
		return TRUE;
	}
};

/**********************************************************************************
 *
 *
 *
 */
int	main( void )
{
	CWnd	wnd1;
	wnd1 = (HWND)0x1;
	CWnd	wnd2( (HWND)0x2 );
	CWnd	wnd3( (HWND)0x3 );

	printf( "0x%x.\n", wnd1.m_hWnd );
	printf( "0x%x.\n", wnd2.m_hWnd );
	printf( "0x%x.\n", wnd3.m_hWnd );
	wnd1 = wnd3;
	printf( "0x%x.\n", wnd1.m_hWnd );
	printf( "0x%x.\n", wnd2.m_hWnd );
	printf( "0x%x.\n", wnd3.m_hWnd );
	wnd1 = (HWND)0x4;
	printf( "0x%x.\n", (HWND)wnd1 );
	printf( "0x%x.\n", (HWND)wnd2 );
	printf( "0x%x.\n", (HWND)wnd3 );
	
	CSampleFrame	FrameWnd;
	FrameWnd.Create(_T("Frame Window"));
	return FrameWnd.MainLoop();
}
