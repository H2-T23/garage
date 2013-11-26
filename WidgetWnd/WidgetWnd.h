// WidgetWnd.h 
#pragma once

#pragma	comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma	comment(lib,"gdi32.lib")
#pragma comment(lib,"comctl32.lib")

#include <tchar.h>
#include <windows.h>
#include <commctrl.h>

#pragma warning(push)
#pragma warning(disable:4530)

#include <iostream>
#include <map>

#pragma warning(pop)

#define		DEFAULT_ICON	((HICON)(LoadImage(NULL,IDI_APPLICATION,IMAGE_ICON,0,0,LR_SHARED)))
#define		DEFAULT_CURSOR	((HCURSOR)(LoadImage(NULL,IDC_ARROW,IMAGE_CURSOR,0,0,LR_SHARED)))

#define		TRACE		OutputDebugStringA

class CWidgetWnd;

/*-----------------------------------------------------------------------------------
 *
 *
 *
 *
 */
namespace WindowCore {
	std::map<HWND,CWidgetWnd*>	WindowMap;

	void	EntryWindowMap(HWND hwnd,CWidgetWnd* pObj){
		WindowMap[ hwnd ] = pObj;
	}

	void	LeaveWindowMap(HWND hwnd,CWidgetWnd* pObj){
	}

	HRESULT	CALLBACK	WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
}

/*-----------------------------------------------------------------------------------
 *
 *
 *
 *
 */
class CWidgetWnd {
public:
	std::map<HWND,CWidgetWnd*>	WindowMap;

	HWND			m_hWnd;
	DWORD			m_dwStyle;
	DWORD			m_dwStyleEx;

	/*-----------------------------------------------------------------------------------
	 *
     */
	CWidgetWnd(void)
		: m_hWnd(NULL)
		, m_dwStyle(WS_OVERLAPPEDWINDOW)
		, m_dwStyleEx(0)
	{
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	CWidgetWnd(LPCTSTR lpszClassName)
		: m_hWnd(NULL)
		, m_dwStyle(WS_OVERLAPPEDWINDOW)
		, m_dwStyleEx(0)
	{
		Create(lpszClassName);
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	BOOL	IsWindow(VOID) const {
		return(::IsWindow(m_hWnd));
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	BOOL	Create(LPCTSTR lpszName){
		return(CWidgetWnd::Create(lpszName,lpszName));
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	BOOL	Create(	LPCTSTR		lpszClassName
				,	LPCTSTR		lpszWindowName
				,	HWND		hwndParent		= NULL
				,	DWORD		dwStyle			= WS_OVERLAPPEDWINDOW 
				,	int			nX				= 100//CW_USEDEFAULT
				,	int			nY				= 100//CW_USEDEFAULT
				,	int			nWidth			= 500//CW_USEDEFAULT
				,	int			nHeight			= 500//CW_USEDEFAULT
				,	HINSTANCE	hInstance		= NULL
				,	DWORD		dwStyleEx		= 0
				)
	{
		WindowCore::EntryWindowMap(NULL, this);

		m_dwStyle			= dwStyle;
		m_dwStyleEx			= dwStyleEx;
		m_hWnd				= NULL;

		WNDCLASSEX	wndclass = {
			sizeof(WNDCLASSEX),
			0,
			WindowCore::WindowProc,
			0,
			0,
			GetModuleHandle(NULL),
			DEFAULT_ICON,
			DEFAULT_CURSOR,
			(HBRUSH)GetStockObject(WHITE_BRUSH),
			NULL,
			lpszClassName,
			DEFAULT_ICON
		};

		if(!RegisterClassEx(&wndclass))
			return FALSE;

		if(hInstance == NULL){
			hInstance =	GetModuleHandle(NULL);
		}

		m_hWnd = CreateWindowEx(m_dwStyleEx, lpszClassName, lpszWindowName, m_dwStyle,
								nX, nY, nWidth, nHeight, hwndParent, NULL, hInstance, NULL);

		if(m_hWnd==NULL)
			return FALSE;

		WindowCore::EntryWindowMap(m_hWnd, this);

		return TRUE;
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	BOOL	CreateControl(		LPCTSTR		lpszClassName
						,	LPCTSTR		lpszWindowName
						,	HWND		hwndParent		= NULL
						,	DWORD		dwStyle			= WS_OVERLAPPEDWINDOW 
						,	int			nX				= 100//CW_USEDEFAULT
						,	int			nY				= 100//CW_USEDEFAULT
						,	int			nWidth			= 500//CW_USEDEFAULT
						,	int			nHeight			= 500//CW_USEDEFAULT
						,	HINSTANCE	hInstance		= NULL
						,	DWORD		dwStyleEx		= 0
						)
	{
		WindowCore::EntryWindowMap(NULL, this);

		m_dwStyle			= dwStyle;
		m_dwStyleEx			= dwStyleEx;
		m_hWnd				= NULL;

		if(hInstance == NULL){
			hInstance =	GetModuleHandle(NULL);
		}

		m_hWnd = CreateWindowEx(m_dwStyleEx, lpszClassName, lpszWindowName, m_dwStyle,
								nX, nY, nWidth, nHeight, hwndParent, NULL, hInstance, NULL);

		if(m_hWnd==NULL)
			return FALSE;

		WindowCore::EntryWindowMap(m_hWnd, this);

		return TRUE;
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	int		Dispatch(void){
		ShowWindow(m_hWnd,SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);

		MSG	msg;
		while(GetMessage(&msg,NULL,0,0)>0){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return(int)msg.wParam;
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	void	SetFocus(void){
		if(IsWindow()) ::SetFocus(m_hWnd);
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	void	SetWindowText(LPCTSTR lpszText){
		if(IsWindow()) ::SetWindowText(m_hWnd,lpszText);
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	BOOL	MoveWindow(int x, int y, int w, int h, BOOL bRepaint){
		if(IsWindow()) return(::MoveWindow(m_hWnd,x,y,w,h,bRepaint));
		return FALSE;
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	virtual HRESULT		WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam){
		switch(uMsg){
		case WM_CREATE:
			OnCreate((LPCREATESTRUCT)lParam);
			break;

		case WM_NOTIFY:
			return(OnNotify((LPNMHDR)lParam));
			break;
		case WM_SIZE:
			OnSize(LOWORD(lParam),HIWORD(lParam));
			break;

		case WM_DRAWITEM:
			OnDrawItem((LPDRAWITEMSTRUCT)lParam);
			break;

		case WM_DESTROY:
			OnDestroy();
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(m_hWnd,uMsg,wParam,lParam);
		}
		
		return 0;
	}

	/*-----------------------------------------------------------------------------------
	 *
     */
	virtual void		OnCreate(LPCREATESTRUCT){
	}
	virtual HRESULT		OnNotify(LPNMHDR){
		return 0;
	}
	virtual void		OnSize(int x,int y){
	}
	virtual void		OnDestroy(void){
	}
	virtual void		OnDrawItem(LPDRAWITEMSTRUCT){
	}
};


/*-----------------------------------------------------------------------------------
 *
 *
 *
 *
 */
namespace WindowCore {
	HRESULT	CALLBACK	WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		if(WM_CREATE == uMsg && IsWindow(hWnd)){
			TRACE("WM_CREATE");
			WindowMap[ NULL ]->m_hWnd = hWnd;
			return(WindowMap[ NULL ]->WindowProc(uMsg,wParam,lParam));
		}

		if(WindowMap.find(hWnd) == WindowMap.end()){
			TRACE("window handle not found.");
		}else{
			if(WindowMap[ hWnd ]->IsWindow()){
				return(WindowMap[ hWnd ]->WindowProc(uMsg,wParam,lParam));
			}
		}

		if(WindowMap.find(NULL) == WindowMap.end()){
			TRACE("zero window handle not found.");
		}else{
			TRACE("hit zero window.");
			if(WindowMap[ NULL ]->IsWindow()){
				return(WindowMap[ NULL ]->WindowProc(uMsg,wParam,lParam));
			}
		}

		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	}
}
