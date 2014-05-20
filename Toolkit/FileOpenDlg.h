#pragma once

#include <Windows.h>
#include <Commdlg.h>
#include "TString.h"

/**********************************************************************************
 *
 *
 *
 */
class CFileOpenDlg {
protected:
	TCHAR			m_szFileName[ MAX_PATH ];

	virtual LPTSTR			Title( void ) const		{ return _T("File Open");	}
	virtual LPCTSTR			Filter( void ) const	{ return _T("Text(*.txt)\0*.txt\0All files(*.*)\0*.*\0\0");	}
	virtual LPCTSTR			DefExt( void ) const	{ return _T("txt"); }
	virtual	DWORD			Flags( void ) const		{ return(OFN_FILEMUSTEXIST);	}

public:
	BOOL	DoModal( HWND hWnd ){
		ZeroMemory(m_szFileName, sizeof(m_szFileName));

		OPENFILENAME		ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize		= sizeof(OPENFILENAME);
		ofn.hwndOwner		= hWnd;
		ofn.lpstrFilter		= Filter();
		ofn.lpstrFile		= m_szFileName;
		ofn.nMaxFile		= MAX_PATH;
		ofn.Flags			= Flags();
		ofn.lpstrFileTitle	= Title();
		ofn.lpstrDefExt		= DefExt();

		return ::GetOpenFileName( &ofn );
	}

	void	GetFileName( TString& str ){
		str.Format(_T("%s\0"), m_szFileName);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CObjOpenDlg : public CFileOpenDlg {
protected:
	virtual LPCTSTR			Filter( void ) const	{ return _T("Object(*.obj)\0*.obj\0All files(*.*)\0*.*\0\0");	}
	virtual LPCTSTR			DefExt( void ) const	{ return _T("obj"); }
};
/**********************************************************************************
 *
 *
 *
 */
class CExeOpenDlg : public CFileOpenDlg {
protected:
	virtual LPCTSTR			Filter( void ) const	{ return _T("Execute(*.exe)\0*.exe\0All files(*.*)\0*.*\0\0");	}
	virtual LPCTSTR			DefExt( void ) const	{ return _T("exe"); }
};
