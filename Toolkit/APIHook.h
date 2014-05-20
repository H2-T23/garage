#pragma once

/**********************************************************************************
 *
 *
 *
 */
class CAPIHook {
protected:
	BYTE*						m_pImageBase;
	IMAGE_DOS_HEADER*			m_pDosHdr;
	IMAGE_NT_HEADERS*			m_pNtHdrs;
	IMAGE_SECTION_HEADER*		m_pSecHdr;

	BOOL	Load( HMODULE hModule ){
		if( hModule ){
			m_pImageBase			= (BYTE*)hModule;
			m_pDosHdr				= (IMAGE_DOS_HEADER*)(m_pImageBase);
			m_pNtHdrs				= (IMAGE_NT_HEADERS*)(m_pImageBase + m_pDosHdr->e_lfanew);
			m_pSecHdr				= (IMAGE_SECTION_HEADER*)(m_pNtHdrs + 1);
		}
		return Verify();
	}

	BOOL	Verify( void ) const {
		return TRUE;
	}

public:
	CAPIHook( void ){
		Load( ::GetModuleHandle( NULL ) );
	}

	BOOL	Replace( void* pOldFunc, void* pNewFunc )
	{
		IMAGE_IMPORT_DESCRIPTOR*	pImport	= NULL;

		if( m_pNtHdrs->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].Size
		&&	m_pNtHdrs->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress	)
		{
			pImport	= (IMAGE_IMPORT_DESCRIPTOR*)(m_pImageBase + m_pNtHdrs->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress);
		}

		if( pImport )
		{
			for( DWORD i=0; i<pImport[ i ].FirstThunk; i++ )
			{
				void**	ppIAT	= (void**)(m_pImageBase + pImport[ i ].FirstThunk);

				for( int j=0; ppIAT[ j ]; j++ )
				{
					if( ppIAT[ j ] == (FARPROC)pOldFunc )
					{
						DWORD	dwProtect	= 0;
						::VirtualProtect( &ppIAT[ j ], sizeof(FARPROC), PAGE_READWRITE, &dwProtect );

						void**	pFunc	= &ppIAT[ j ];
						ppIAT[ j ]		= (FARPROC)pNewFunc;

						::VirtualProtect( &ppIAT[ j ], sizeof(FARPROC), dwProtect, &dwProtect );

						return TRUE;
					}
				}
			}
		}

		return FALSE;
	}
};
/**********************************************************************************
 * Exapmle:
 *
 *
typedef int	(WINAPI* FUNC)(HWND, const char*, const char*, int);

extern "C" {
	static FUNC		pMessageBox	= NULL;

	int WINAPI	MsgBox( HWND hwnd, const char* pMsg, const char* pCaption, int nType ){
		return pMessageBox(hwnd, pMsg, "Hook", nType);
	}
}
 */
/**********************************************************************************
 * Exapmle:
 *
 *
void	EntryHook( void )
{
	MessageBoxA(NULL, "Before", "Entry", MB_OK);

	pMessageBox	= (FUNC)::GetProcAddress(::GetModuleHandleA("user32.dll"), "MessageBoxA");

	CAPIHook	APIHook;
	APIHook.Replace( pMessageBox, &MsgBox );

	MessageBoxA(NULL, "After", "Entry", MB_OK);
}
 */
