#pragma once
#include <psapi.h>

#pragma comment(lib,"psapi.lib")
/**********************************************************************************
 *
 *
 *
 */
class CProcessInfo {
public:
	DWORD				m_dwPID;
	HANDLE				m_hProcess;

	inline bool		IsValid( void ) const {
		return(m_hProcess ? true : false);
	}

	inline bool		IsInvalid( void ) const {
		return !IsValid();
	}

	operator DWORD () { return(m_dwPID); }
	operator HANDLE () { return(m_hProcess); }

	CProcessInfo( void ) : m_dwPID(0), m_hProcess(NULL) {
	}

	CProcessInfo( DWORD dwPID ) : m_dwPID(dwPID), m_hProcess(NULL) {
		GetProcessHandle();
	}

	~CProcessInfo( void ){
		if( m_hProcess )
			::CloseHandle( m_hProcess );
		m_hProcess	= NULL;
	}

	HANDLE		SetPID( DWORD dwPID ){
		return GetProcessHandle( m_dwPID = dwPID );
	}

	HANDLE		GetProcessHandle( DWORD dwPID ){
		return(m_hProcess = ::OpenProcess(GENERIC_ALL, FALSE, dwPID));
	}

	HANDLE		GetProcessHandle( void ){
		return GetProcessHandle(m_dwPID);
	}

	DWORD		GetProcessImageFlleName( LPWSTR lpszImageFileName, DWORD dwSize ){
		return ::GetProcessImageFileName(m_hProcess, lpszImageFileName, dwSize);
	}

	DWORD		GetProcessImageFileNameEx( LPWSTR lpszImageFileName, DWORD dwSize )
	{
		DWORD	dwRet	= GetProcessImageFlleName(lpszImageFileName, dwSize);
		if( dwRet )
		{
			WCHAR* pszSlash = wcschr(&lpszImageFileName[1], L'\\');
			if( pszSlash )
				pszSlash	= wcschr(pszSlash + 1, L'\\');
			if( !pszSlash )
				return 0;

			WCHAR	cSave	= *pszSlash;
			*pszSlash		= 0;

			WCHAR	szNTPath[ MAX_PATH ];
			WCHAR	szDrive[ MAX_PATH ]		= L"A:";

			for( WCHAR cDrive = L'A'; cDrive < L'Z'; cDrive++ )
			{
				szDrive[ 0 ]	= cDrive;
				szNTPath[ 0 ]	= 0;
				if( 0 != ::QueryDosDevice(szDrive, szNTPath, _countof(szNTPath))
				&&	0 == _wcsicmp(szNTPath, lpszImageFileName)	)
				{
					wcscat_s(szDrive, _countof(szDrive), L"\\");
					wcscat_s(szDrive, _countof(szDrive), pszSlash + 1);
					wcscpy_s(lpszImageFileName, dwSize, szDrive);
					return dwRet;
				}
			}
			*pszSlash	= cSave;
		}
		return 0;
	}
};
