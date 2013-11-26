// TestWindow.cpp
#include <windowsx.h> 
#include "Wnd.h"
#include "CtrlWnd.h"

#define ID_BUTTON			1001

class CTestWnd : public CWnd {
public:
	CChildWnd		m_wndChild;
	CButtonWnd		m_wndButtonA;
	CButtonWnd		m_wndButtonB;
//	CStaticWnd		m_wndStatic;
//	CPanelWnd		m_wndPanel;
	CComboBoxWnd	m_wndComboBox;
	
	virtual LRESULT		WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		switch(uMsg){
		case WM_CREATE:
			{
			//	m_wnd.Create( _T("Wnd"), 100, 100, 50, 50, this, (HMENU)1111 );
			//	if( m_wndChild.Create(_T("child"), m_hWnd, 10, 10, 100, 30 ) ){
			//		MessageBox( NULL, _T("TRUE"), _T("OK"), MB_OK );
			//	}else{
			//		MessageBox( NULL, _T("FALSE"), _T("OK"), MB_OK );
			//	}
				
			//	m_wndButtonB.Create( _T("B"), 0, 0, 30, 10, &m_wndChild, (HMENU)1 ); 
			
			/*
				m_wndPanel.Create( _T("static window"), 400, 0, 500, 500, this, (HMENU)1000 );
				m_wndButtonA.Create( _T("A"), 30, 30, 80, 40, &m_wndPanel, (HMENU)1001 );
				m_wndComboBox.Create( NULL, 100, 100, 80, 80, &m_wndPanel, (HMENU)1002 );
			*/
				m_wndChild.Create( _T("static window"), m_hWnd, 400, 0, 500, 500 );
				m_wndButtonA.Create( _T("A"), 30, 30, 80, 40, (HWND)m_wndChild, (HMENU)1001 );
				m_wndComboBox.Create( NULL, 100, 100, 80, 80, (HWND)m_wndChild, (HMENU)1002 );
				m_wndComboBox.AddString( _T("Item1") );
				m_wndComboBox.AddString( _T("Item2") );
				m_wndComboBox.AddString( _T("Item3") );
				
				::ShowWindow( (HWND)m_wndChild, SW_SHOW );
				::UpdateWindow( (HWND)m_wndChild );
				
			//	m_wndButtonA.SetParent( this );
			}
			break;
			
		case WM_COMMAND:
			{
				if( HIWORD(wParam) == CBN_SELCHANGE )
				{
					TCHAR szBuf[ 128 ];
					
					int nIdx = m_wndComboBox.GetCurSel();
					m_wndComboBox.GetItemString( nIdx, szBuf );					
					MessageBox(NULL, szBuf, _T("OK"), MB_OK);
				}
				else
					MessageBox(NULL, TEXT("É{É^ÉìÇ™âüÇ≥ÇÍÇ‹ÇµÇΩÅB"), TEXT("OK"), MB_OK);
			}
			break;
/**/			
		default:
			return CWnd::WindowProc(uMsg,wParam,lParam);
		}
		return 0;
	}
};

int	WINAPI	_tWinMain( HINSTANCE hInstance, HINSTANCE, PTSTR, int ){
	CTestWnd	wnd;
	wnd.Create(_T(""),WS_OVERLAPPEDWINDOW);
	return wnd.MainLoop();
}
