#pragma once
#include <afxdao.h>

class CMdb2Txt {
  CMdb2Txt(const CMdb2Txt&);
	CMdb2Txt& operator=(const CMdb2Txt&);

private:
	CMdb2Txt(void){}
	~CMdb2Txt(void){}

	CMdb2Txt( LPCTSTR	lpszMDBFileName
			, LPCTSTR	lpszTXTFileName = NULL
			, HWND		hWnd = NULL 
			);

	BOOL		Construct(	LPCTSTR		lpszMDBFileName	= NULL
						,	LPCTSTR		lpszTXTFileName	= NULL
						,	HWND		hWnd			= NULL 
						);

	CString			m_strMDBFileName;
	CString			m_strTXTFileName;

	BOOL			Setup( LPCTSTR lpszMDBFileName, LPCTSTR lpszTXTFileName, HWND hWnd );
	BOOL			Mdb2Txt( LPCTSTR, LPCTSTR );
	VOID			ExecuteProcess( LPCTSTR lpszCmd );

	BOOL			IsFileExist( LPCTSTR lpszFileName ) const ;

public:
	static BOOL		Execute( HWND hWnd ){

	}

	static BOOL		Execute(	LPCTSTR		lpszMDBFileName	= NULL
							,	LPCTSTR		lpszTXTFileName	= NULL
							,	HWND		hWnd			= NULL 
							)
	{
	#if 0
		CMdb2Txt	ins( lpszMDBFileName, lpszTXTFileName, hWnd );
		return TRUE;
	#else
		CMdb2Txt	ins;
		return ins.Construct( lpszMDBFileName, lpszTXTFileName, hWnd );
	#endif
	}

	static void		DaoInit(){	::AfxDaoInit();	}
	static void		DaoTerm(){	::AfxDaoTerm();	}
};
