// ListView.cpp 

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

/**********************************************************
 **/
namespace SDK {
/**********************************************************
 **/
class CObject {
public:
	CObject(void){}
	virtual ~CObject(void){}
};

/**********************************************************
 **/
class CWnd : public CObject {
protected:
	HWND			m_hWnd;

	BOOL	ThisCopy(CWnd& rWnd){
		if(&rWnd!=this){
			m_hWnd = rWnd.m_hWnd;
			return(TRUE);
		}
		return(FALSE);
	}

public:
	CWnd(void):m_hWnd(NULL){}
	CWnd(HWND hWnd):m_hWnd(hWnd){}
	CWnd(CWnd& rWnd):m_hWnd(rWnd.m_hWnd){}

	BOOL	IsValid(void) const {
		return(m_hWnd!=NULL);
	}

	virtual LRESUTL	WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam){
		return DefWindowProc(m_hWnd,uMsg,wParam,lParam);
	}

public:
	virtual ~CWnd(void){}

public:
	CWnd&	operator=(CWnd& rWnd){
		ThisCopy(rWnd);
		return(*this);
	}

	operator HWND(){return(m_hWnd);}
};

/**********************************************************
 **/
class CMapWnd {
private:
	typedef std::map<HWND,CWnd*>	MapWnd;
	MapWnd		m_mapWnd;	
public:
	void	Register( CWnd* pWnd ){
		m_mapWnd[ (HWND)(*pWnd) ] = pWnd;
	}
	void	Unregister( CWnd* pWnd ){
		m_mapWnd[ (HWND)(*pWnd) ] = NULL;
	}
	CWnd*	Find( HWND hWnd ) const {
		MapWnd::const_iterator it = m_mapWnd.find( hWnd );
		if( it != m_mapWnd.end() ){
			return(it->second);
		}
		return NULL;
	}
};

/**********************************************************
 **/
class CComponentWnd : public CWnd {
public:
	CComponentWnd(void){}
	virtual ~CComponentWnd(void){}
};

/**********************************************************
 **/
class CContainerWnd : public CWnd {
public:
	CContainerWnd(void){}
	CContainerWnd(CWnd* pParentWnd) : 

public:
	virtual ~CContainerWnd(void){}

protected:
	typedef std::map<HWND,CWnd*>	MapWnd;
	typedef MapWnd::iterator		MapWndIterator;

	CWnd*			m_pParentWnd;
	CMapWnd			m_mapWnd;

	void		Constractor(CWnd* pParentWnd){
		m_pParentWnd = pParentWnd;
	}

public:
	void		Append( CWnd* pWnd ){
		m_mapWnd.Register( pWnd );
	}

	void		Remove( CWnd* pWnd ){
		CWnd* pTargetWnd = Find( pWnd );
		if( pTargetWnd ){
			m_mapWnd[ (HWND)pTargetWnd ] = NULL;
		}
	}

	CWnd*		Find( CWnd* pWnd ) const {
		return m_mapWnd.Find( (HWND)pWnd );
	}
};

/**********************************************************
 **/
class CPanel : public CContainerWnd {
public:
	CPanel(void){}
	virtual ~CPanel(void){}

};

/**********************************************************
 **/
class CFrame : public CContainerWnd {
public:
	CFrame(void){}
	virtual ~CFrame(void){}
};

/**********************************************************
 **/
class CListView : public CComponentWnd {
public:
	CListView(void){}

protected:

public:
	virtual ~CListView(void){}

	BOOL	Create(LPCTSTR lpszName = NULL,HWND hParentWnd = NULL,HINSTANCE hInstance = NULL,DWORD dwStyle = LVS_LIST,DWORD dwStyleEx = 0){
		m_hWnd	= CreateWindowEx(dwStyleEx, WC_LISTVIEW, lpszName, (WS_CHILD | WS_VISIBLE | dwStyle), 0, 0, 0, 0, hParentWnd, (HMENU)1, hInstance, NULL);
		if( m_hWnd == NULL ){
			return(FALSE);
		}
		return(TRUE);
	}

	inline
	void	InsertItem(LVITEM* pItem){
		ListView_InsertItem(m_hWnd,pItem);		
	}

	void	InsertItem(int row, int col, LPTSTR lpszItem){
		LVITEM	lvItem = {0};
		lvItem.mask		= LVIF_TEXT;
		lvItem.iItem	= row;
		lvItem.iSubItem	= col;
		lvItem.pszText	= lpszItem;
		InsertItem(&lvItem);
	}
};


/**********************************************************
 **/
namespace WindowManager {
	/**********************************************************
	 **/
	CMapWnd		m_mapWnd;
	
	/**********************************************************
	 **/
	LRESULT CALLBACK	WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
		CWnd* pWnd = m_mapWnd.Find(hWnd);
		if( pWnd ){
			return pWnd->WindowProc(uMsg,wParam,lParam);
		}
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}

	/**********************************************************
	 **/
	int		MessageLoop( void ){
		MSG	msg;
		while( GetMessage(&msg,NULL,0,0) > 0 ){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return(int)msg.wParam;
	}
}// end of "namespace WindowManager".

/**********************************************************
 **/
void	Main( void )
{
	HWND	hwnd = (HWND)123;
	CWnd	wnd1( (HWND)456 );
	CWnd	wnd2;
	CWnd	wnd3( hwnd );
	CWnd	wnd4( wnd3 );
	
	wnd2 = wnd1;


	printf( "%d\n", (HWND)wnd1 );
	printf( "%d\n", wnd2.operator HWND() );
	printf( "%d\n", (HWND)wnd3 );
	printf( "%d\n", (HWND)wnd4 );
}

}// end of "namespace SDK".


void	main( void )
{
	SDK::Main();
}
