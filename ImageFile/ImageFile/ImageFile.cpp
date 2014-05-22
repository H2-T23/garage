// ImageFile.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ImageFile.h"

#include "UseToolkit.h"
#include "FileOpenDlg.h"


#include <imagehlp.h>
#pragma comment(lib,"imagehlp.lib")

/**********************************************************************************
 *
 *
 *
 */
class CVBuffer {
protected:
	BYTE*		m_pData;
	DWORD		m_dwSize;

public:
	CVBuffer( void ) 
	 : m_pData(NULL), m_dwSize(0) 
	{
	}

	CVBuffer( DWORD dwSize )
	 : m_pData(NULL), m_dwSize(0) 
	{
		Alloc(dwSize);
	}

	~CVBuffer( void )
	{
		Free();
	}

public:
	BYTE*		Alloc( DWORD dwSize ){
		LPVOID	pData	= ::VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if( pData != NULL ){
			m_pData		= (BYTE*)pData;
			m_dwSize	= dwSize;
			ZeroMemory(pData, dwSize);
		}
		return m_pData;
	}

	void		Free( void ){
		if( m_pData ){
			::VirtualFree( (LPVOID)m_pData, 0, MEM_RELEASE);
		}
		m_pData		= NULL;
		m_dwSize	= 0;
	}

public:
	BYTE*	GetDataBuffer( void ) const	{ return(m_pData);	}
	DWORD	GetDataSize( void ) const	{ return(m_dwSize);	}
};
/**********************************************************************************
 *
 *
 *
 */
class CImageFile {
protected:
	CVBuffer						m_Buffer;

public:
	BYTE*							m_pBase;
	IMAGE_DOS_HEADER*				m_pDosHdr;
	IMAGE_NT_HEADERS*				m_pNtHdrs;
	IMAGE_SECTION_HEADER*			m_pSecHdr;

public:
	DWORD	GetImageSize( void ) const {
		return m_Buffer.GetDataSize();
	}
	inline BYTE*						GetLoadBase( void ) const		{ return(m_pBase); }
	inline IMAGE_DOS_HEADER*			GetDosHeader( void ) const		{ return GetLoadBase() ? m_pDosHdr : NULL;	}
	inline IMAGE_NT_HEADERS*			GetNtHeaders( void ) const		{ return GetLoadBase() ? m_pNtHdrs : NULL;	}
	inline IMAGE_FILE_HEADER*			GetFileHeader( void ) const		{ return GetLoadBase() ? &m_pNtHdrs->FileHeader : NULL;	}
	inline IMAGE_OPTIONAL_HEADER*		GetOptionalHeader( void ) const	{ return GetLoadBase() ? &m_pNtHdrs->OptionalHeader : NULL;	}

	inline IMAGE_SECTION_HEADER*		GetSectionHeader( int idx ) const 
	{
		if( (0 <= idx) && (idx < m_pNtHdrs->FileHeader.NumberOfSections) )
			return GetLoadBase() ? &m_pSecHdr[idx] : NULL;
		return NULL;
	}

	BYTE*						GetDirEntryData( int idx ) const {
		if( (0 <= idx) && (idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) ){
			if( m_pNtHdrs->OptionalHeader.DataDirectory[ idx ].VirtualAddress && m_pNtHdrs->OptionalHeader.DataDirectory[ idx ].Size ){
				return GetLoadBase() + m_pNtHdrs->OptionalHeader.DataDirectory[ idx ].VirtualAddress;
			}
		}
		return NULL;
	}

	DWORD						GetDirEntryDataSize( int idx ) const {
		if( (0 <= idx) && (idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) ) {
			return GetLoadBase() ? m_pNtHdrs->OptionalHeader.DataDirectory[ idx ].Size : 0;
		}
		return 0;
	}
	/*	Directory Entries
	#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
	#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
	#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
	#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
	#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
	#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
	#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
	//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
	#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
	#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
	#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
	#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
	#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
	#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
	#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
	#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor
	 */
	IMAGE_EXPORT_DIRECTORY*		GetExportDir( void ) const { return (IMAGE_EXPORT_DIRECTORY*)GetDirEntryData( IMAGE_DIRECTORY_ENTRY_EXPORT );	}
	IMAGE_IMPORT_DESCRIPTOR*	GetImportDir( void ) const { return (IMAGE_IMPORT_DESCRIPTOR*)GetDirEntryData( IMAGE_DIRECTORY_ENTRY_IMPORT );	}
	IMAGE_RESOURCE_DIRECTORY*	GetResourceDir( void ) const { return (IMAGE_RESOURCE_DIRECTORY*)GetDirEntryData( IMAGE_DIRECTORY_ENTRY_RESOURCE );	}

	typedef std::vector<const char*>		DllNames;

	DllNames					GetImportDllNames( void ) const 
	{
		DllNames		str;
		IMAGE_IMPORT_DESCRIPTOR*	pImpDesc	= GetImportDir();
		if( pImpDesc )
		{
			for( int i=0; pImpDesc[ i ].FirstThunk; i++ )
			{
				str.push_back( GetPtr<const char*>(pImpDesc[ i ].Name) );
			}
		}
		return str;
	}


	template<typename T>
	T	GetPtr( DWORD dwOffset ) const {
		return reinterpret_cast<T>(m_pBase + dwOffset);
	}

	typedef struct {
		DWORD		iat_entry_rva;
		void**		iat_entry_p;
		WORD		hint;
		union {
			struct {
				WORD	import_by_name;
				WORD	ordinal;
			};
			const char*	name;
		};
	} ImportSymbol;

	typedef std::vector<ImportSymbol>		SYMBOLS;

	SYMBOLS			GetImportSymbols( const char* pDllName )
	{
		SYMBOLS		symbols;
		IMAGE_IMPORT_DESCRIPTOR*	pImpDesc	= GetImportDir();
		if( pImpDesc && pImpDesc[ 0 ].OriginalFirstThunk )
		{
			for( int i=0; pImpDesc[ i ].FirstThunk; i++ )
			{
				const char*	pName	= GetPtr<const char*>( pImpDesc[ i ].Name );
				if( strcmp(pDllName, pName) == 0 )
				{
					DWORD*	pIAT	= GetPtr<DWORD*>( pImpDesc[ i ].FirstThunk );
					DWORD*	pINT	= pIAT;
					if( pImpDesc[ i ].OriginalFirstThunk )
					{
						pINT	= GetPtr<DWORD*>( pImpDesc[ i ].OriginalFirstThunk );
					}

					for( int j=0; pINT[ j ]; j++ )
					{
						if( pINT[ j ] < GetOptionalHeader()->SizeOfImage )
						{
							ImportSymbol	sym	= {0};
							sym.iat_entry_rva	= pImpDesc[ i ].FirstThunk + sizeof(DWORD) * j;
							sym.iat_entry_p		= (void**)&pIAT[ j ];

							if( IMAGE_SNAP_BY_ORDINAL32(pINT[ j ]) )
							{
								sym.ordinal		= (WORD)IMAGE_ORDINAL32( pINT[ j ] );
							}
							else
							{
								IMAGE_IMPORT_BY_NAME*	pByName	= GetPtr<IMAGE_IMPORT_BY_NAME*>( pINT[ j ] );
								sym.hint	= pByName->Hint;
								sym.name	= (char*)pByName->Name;
							}

							symbols.push_back( sym );
						}
					}
				}
			}
		}

		return symbols;
	}

public:
	typedef struct {
		DWORD		symbol_rva;
		void*		symbol_p;
		DWORD		ordinal;
		const char*	name;
		const char*	forwarded_to;
	} EXPORT_SYMBOL;

	typedef std::vector<EXPORT_SYMBOL>	ExportSymbols;

	ExportSymbols		GetExportSymbols( void ){
		ExportSymbols	symbols;

		IMAGE_EXPORT_DIRECTORY*	pExport	= GetExportDir();
		if( pExport )
		{
			for( size_t i=0; i<pExport->NumberOfFunctions; i++ )
			{
				EXPORT_SYMBOL	sym	= {0};

				sym.symbol_rva	= GetExportSymbolRVA( i );
				if( sym.symbol_rva )
				{
					sym.symbol_p		= GetPtr<void*>( sym.symbol_rva );
					sym.ordinal			= i + pExport->Base;
					sym.name			= FindExportSymbolName( i );
					sym.forwarded_to	= IsExportForwarded( sym.symbol_rva ) ? (char*)sym.symbol_p : "";

					symbols.push_back( sym );
				}
			}
		}
		return symbols;
	}

	void*				GetExportSymbolFromOrdinal( WORD wOrdinal ){
		IMAGE_EXPORT_DIRECTORY*	pExport	= GetExportDir();
		if( pExport )
		{
			DWORD	dwRVA	= GetExportSymbolRVA( wOrdinal - pExport->Base );
			if( dwRVA )
			{
				if( IsExportForwarded(dwRVA) )
				{
					char*	pForwarded	= GetPtr<char*>( dwRVA );
					char*	pSymbolName	= strstr( pForwarded, "," ) + 1;
					char	pDllName[MAX_PATH]	= {0};

					memcpy( pDllName, pForwarded, pSymbolName - pForwarded );
					strncat( pDllName, "DLL", sizeof(pDllName) - 1 );

					HMODULE	hModule	= ::GetModuleHandleA( pDllName );
					if( hModule	== NULL ){
						hModule	= ::LoadLibraryA( pDllName );
					}

					if( hModule ){
						return CImageFile( hModule ).GetExportSymbolFromName( pSymbolName );
					}
				}
				else
				{
					return GetPtr<void*>( dwRVA );
				}
			}
		}
		return NULL;
	}

	void*				GetExportSymbolFromName( const char* pName ){
		IMAGE_EXPORT_DIRECTORY*	pExport	= GetExportDir();
		if( pExport )
		{
			DWORD*	pdwRVA		= GetPtr<DWORD*>( pExport->AddressOfNames );
			WORD*	pwOrdinal	= GetPtr<WORD*>( pExport->AddressOfNameOrdinals );

			int	left	= 0;
			int	right	= pExport->NumberOfNames - 1;

			while( left <= right )
			{
				int		middle	= (left + right) / 2;
				char*	name	= GetPtr<char*>( pdwRVA[ middle ] );

				int	ret = strcmp( pName, name );
				if( ret == 0 )
				{
					return GetExportSymbolFromOrdinal( (WORD)(pwOrdinal[ middle ] + pExport->Base) );
				}
				else
				if( ret > 0 )
				{
					left	= middle + 1;
				}
				else
				{
					right	= middle - 1;
				}
			}
		}
		return NULL;
	}

protected:
	const char*			FindExportSymbolName( size_t idx ){
		IMAGE_EXPORT_DIRECTORY*	pExport		= GetExportDir();
		if( pExport )
		{
			DWORD*	pdwName		= GetPtr<DWORD*>( pExport->AddressOfNames );
			WORD*	pwOrdinal	= GetPtr<WORD*>( pExport->AddressOfNameOrdinals );
			
			for( DWORD i=0; i<pExport->NumberOfNames; i++ )
			{
				if( pwOrdinal[ i ] == idx )
				{
					return GetPtr<const char*>( pdwName[ i ] );
				}
			}
		}
		return NULL;
	}

	DWORD				GetExportSymbolRVA( size_t idx ){
		IMAGE_EXPORT_DIRECTORY*	pExport	= GetExportDir();
		if( pExport )
		{
			if( (0 <= idx) && (idx < pExport->NumberOfFunctions) )
			{
				return GetPtr<DWORD*>( pExport->AddressOfFunctions )[ idx ];
			}
		}
		return 0;
	}	

	bool				IsExportForwarded( DWORD dwRVA ){
		if( GetLoadBase() )
		{
			IMAGE_DATA_DIRECTORY*	pDir = &GetOptionalHeader()->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];
			if( pDir->VirtualAddress && pDir->Size )
			{
				if( (pDir->VirtualAddress <= dwRVA) && (dwRVA < (pDir->VirtualAddress + pDir->Size)) )
				{
					return true;
				}
			}
		}
		return false;
	}

public:
	CImageFile( void )
	 : m_pBase(NULL) 
	{
	}

	CImageFile( LPTSTR lpFilename )
	 : m_pBase(NULL) 
	{
		Load( lpFilename );
	}

	CImageFile( BYTE* pData )
	 : m_pBase(NULL) 
	{
		Load( pData );
	}

	CImageFile( HMODULE hModule )
	 : m_pBase(NULL) 
	{
		Load( hModule );
	}

public:
	BOOL		Verify( const CImageFile* pImageFile ) const 
	{
		if( pImageFile->GetDosHeader()->e_magic != *(WORD*)"MZ" ){
			return FALSE;
		}

		if( pImageFile->GetDosHeader()->e_lfanew == 0 ){
			return FALSE;
		}

		if( pImageFile->GetNtHeaders()->Signature != *(DWORD*)"PE\0\0" ){
			return FALSE;
		}

		if( pImageFile->GetOptionalHeader()->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC ){
			return FALSE;
		}

		return TRUE;
	}

	BOOL		Verify( void ) const {
		return Verify( this );
	}

	BOOL		Load( LPTSTR lpFilename )
	{
		std::vector<BYTE>	data;

		CReadOnlyFile	file(lpFilename);
		if( file.IsOpen() )
		{
			DWORD dwSize = file.GetFileSize();
			if( dwSize > 0 )
			{
				data.resize( dwSize );
				if( dwSize == file.Read(&data.at(0), dwSize) )
				{
					CImageFile	img( &data.at(0) );
					if( img.Verify() )
					{
						m_pBase	= (BYTE*)m_Buffer.Alloc( img.GetOptionalHeader()->SizeOfImage );
						if( m_pBase )
						{
							BYTE*		pDst	= m_pBase;
							BYTE*		pSrc	= &data.at(0);
							unsigned	uSize	= img.GetOptionalHeader()->SizeOfHeaders;

							memcpy( pDst, pSrc, uSize );

							for( unsigned i=0; i<img.GetFileHeader()->NumberOfSections; i++ )
							{
								if( img.GetSectionHeader( i )->PointerToRawData )
								{
									pDst	= &m_pBase[ img.GetSectionHeader( i )->VirtualAddress ];
									pSrc	= &data.at( img.GetSectionHeader( i )->PointerToRawData );
									uSize	= img.GetSectionHeader( i )->SizeOfRawData;

									memcpy( pDst, pSrc, uSize );
								}
							}
						}
					}
				}
			}
		}

		if( m_pBase	== NULL )
		{
			Unload();
			return FALSE;
		}

		m_pDosHdr		= (IMAGE_DOS_HEADER*)(m_pBase);
		m_pNtHdrs		= (IMAGE_NT_HEADERS*)(m_pBase + m_pDosHdr->e_lfanew);
		m_pSecHdr		= (IMAGE_SECTION_HEADER*)(m_pNtHdrs + 1);
		if( !Verify() )
		{
			Unload();
			return FALSE;
		}
		return TRUE;
	}

	BOOL		Load( BYTE* pData ){
		m_pBase			= pData;
		m_pDosHdr		= (IMAGE_DOS_HEADER*)(m_pBase);
		m_pNtHdrs		= (IMAGE_NT_HEADERS*)(m_pBase + m_pDosHdr->e_lfanew);
		m_pSecHdr		= (IMAGE_SECTION_HEADER*)(m_pNtHdrs + 1);
		if( !Verify() )
		{
			Unload();
			return FALSE;
		}
		return TRUE;
	}

	BOOL		Load( HMODULE hModule ){
		m_pBase			= (BYTE*)hModule;
		m_pDosHdr		= (IMAGE_DOS_HEADER*)(m_pBase);
		m_pNtHdrs		= (IMAGE_NT_HEADERS*)(m_pBase + m_pDosHdr->e_lfanew);
		m_pSecHdr		= (IMAGE_SECTION_HEADER*)(m_pNtHdrs + 1);
		if( !Verify() )
		{
			Unload();
			return FALSE;
		}
		return TRUE;
	}

	void		Unload( void ){
		m_pBase			= NULL;
		m_pDosHdr		= NULL;
		m_pNtHdrs		= NULL;
		m_pSecHdr		= NULL;
		m_Buffer.Free();
	}

	void**		HookImportSymbol( void* pOldFunc, void* pNewFunc ){
		IMAGE_IMPORT_DESCRIPTOR*	pImport	= GetImportDir();
		if( pImport )
		{
			for( int i=0; pImport[ i ].FirstThunk; i++ )
			{
				void**	ppIAT	= GetPtr<void**>( pImport[ i ].FirstThunk );

				for( int j=0; ppIAT[ j ]; j++ )
				{
					if( ppIAT[ j ] == (FARPROC)pOldFunc )
					{
						DWORD	dwOldProtect	= 0;
						::VirtualProtect( &ppIAT[ j ], sizeof(FARPROC), PAGE_READWRITE, &dwOldProtect );

						void**	pFunc	= &ppIAT[ j ];
						ppIAT[ j ]		= (FARPROC)pNewFunc;

						::VirtualProtect( &ppIAT[ j ], sizeof(FARPROC), dwOldProtect, &dwOldProtect );
						
						return &ppIAT[ j ];
					}
				}
			}
		}
		return NULL;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CImageOpenDlg	: public CFileOpenDlg {
protected:
	virtual LPCTSTR			Filter( void ) const	{ return _T("Image(*.exe;*.obj;*.dll)\0*.exe;*.obj;*.dll\0All files(*.*)\0*.*\0\0");	}
};
/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, class BASECLASS>
class ICommandHandler : public BASECLASS {
protected:
	TCommandHandler<CLASS>	cmd;

public:
	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CTabPanel : public CPanel {
public:
	enum {
		IDC				= 1000	,
		IDC_TABCTRL				,
	};

protected:
	CTabCtrl					m_TabCtrl;
	std::vector<CWnd*>			m_Pages;
	CWnd*						m_pActivePage;

	TNotifyHandler<CTabPanel>	notify;

protected:
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_TabCtrl.Create(this, 0, 0, 0, 0, IDC_TABCTRL);

		notify.Initialize(this);
		notify.Register(TCN_SELCHANGE, IDC_TABCTRL, &CTabPanel::OnSelChange);
		return TRUE;
	}

public:
	CTabPanel( void ) : m_pActivePage(NULL) {
	}

	void	OnSize(UINT state, int cx, int cy){
		if( cx <= 0 || cy <= 0 )
			return;
		m_TabCtrl.MoveWindow(0,0,cx,cy);
		Resize();
	}

	void	OnNotify(WPARAM wParam, LPARAM lParam){
		notify.Dispach(wParam, lParam);
	}

	void	OnSelChange( LPNMHDR lpNmhdr ){
		int	item = m_TabCtrl.GetCurSel();
		SelectPage( item );
		Resize();
	}

public:
	void	Append( CWnd* pWnd, LPTSTR lpTitle ){
		m_pActivePage	= pWnd;
		pWnd->SetWindowText( lpTitle );
		m_Pages.push_back( pWnd );

		m_TabCtrl.InsertItem( m_TabCtrl.GetItemCount(), lpTitle );
	}

	void	SelectPage( int item ){
		m_TabCtrl.SetCurSel( item );
		m_pActivePage	= m_Pages.at( item );
		if( m_pActivePage ){
			std::vector<CWnd*>::iterator it = m_Pages.begin();
			for( int i=0; it!=m_Pages.end(); i++, it++ )
			{
				(*it)->ShowWindow( i == item ? SW_SHOW : SW_HIDE );
			}
		}
	}

	void	Resize( void ){
		CWnd*	pWnd	= GetActivePage();
		if( pWnd ){
			CRect	rc;
			GetWindowRect( &rc );
			m_TabCtrl.AdjustRect(FALSE, rc);
			CPoint	pt1, pt2;
			pt1		= rc.TopLeft();
			pt2		= rc.BottomRight();
			m_TabCtrl.ScreenToClient( &pt1 );
			m_TabCtrl.ScreenToClient( &pt2 );
			pWnd->MoveWindow( pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y );
		//	GetWindowRect( &rc );
		//	RedrawWindow(m_hWnd, &rc, NULL, RDW_FRAME|RDW_ALLCHILDREN);
		}
	}

	CWnd*	GetActivePage( void ) const {
		return(m_pActivePage);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public ICommandHandler<CMainForm,CForm> {
protected:
	enum {
		MAX_COL				= 256	,
		MAX_ROW				= 256	,

		ID_MENU				= 100	,
		ID_FILE						,
		ID_FILE_OPEN				,
		ID_FILE_SAVE				,
		ID_FILE_EXIT				,
		ID_HELP						,
		ID_HELP_ABOUT				,

		IDC					= 1000	,
		IDC_LISTVIEW				,
		IDC_NT_HDRS					,
		IDC_SECTION_HEADER			,
		IDC_IMPORT_ENTRY			,
		IDC_EXPORT_ENTRY			,
	};

	CTabPanel				m_wndTabPanel;

	CListView				m_wndDump;
	CListView				m_wndHdrs;
	CListView				m_wndSecHdr;
	CListView				m_wndImpEnt;
	CListView				m_wndExpEnt;

	CImageFile				m_ImageFile;

	void	InitMenu( void ){
		CPopupMenu	menuFile;
		menuFile.Create();
		menuFile.Insert(_T("Open"), ID_FILE_OPEN);
		menuFile.Insert(_T("Save"), ID_FILE_SAVE);
		menuFile.Insert(NULL, 0);
		menuFile.Insert(_T("Exit"), ID_FILE_EXIT);

		CPopupMenu	menuHelp;
		menuHelp.Create();
		menuHelp.Insert(_T("About"), ID_HELP_ABOUT);

		CMenu	menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File"), ID_FILE, (HMENU)menuFile);
		menuMain.Insert(_T("Help"), ID_HELP, (HMENU)menuHelp);

		SetMenu(m_hWnd, (HMENU)menuMain);
	}

	void	InitCtrl( void )
	{
		if( m_wndDump.Create(this, 0, 0, 0, 0, IDC_LISTVIEW) ){
			m_wndDump.SetExtendedLitViewStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT /*| LVS_EX_DOUBLEBUFFER*/ );
			m_wndDump.InsertColumn(0, _T("address"), 100);
			for( int i=1; i<=16; i++ ){
				m_wndDump.InsertColumn(i, _T(""), 30);
			}
			m_wndDump.InsertColumn(17, _T(""), 100);

			for( int i=18; i<MAX_COL; i++ ){
				m_wndDump.InsertColumn(i, _T(""), 30);
			}
			for( int i=0; i<MAX_ROW; i++ ){
				m_wndDump.InsertItem(i, _T(""));
			}
		}

		if( m_wndHdrs.Create(this, 0, 0, 0, 0, IDC_NT_HDRS) ){
			m_wndHdrs.SetExtendedLitViewStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT /*| LVS_EX_DOUBLEBUFFER*/ );
			m_wndHdrs.InsertColumn(0, _T("struct"), 200);
			m_wndHdrs.InsertColumn(1, _T("type"));
			m_wndHdrs.InsertColumn(2, _T("member"), 200);
			m_wndHdrs.InsertColumn(3, _T("value"));
			m_wndHdrs.InsertColumn(4, _T(""), 300);
			for( int i=5; i<MAX_COL; i++ ){
				m_wndHdrs.InsertColumn(i, _T(""), 30);
			}
			for( int i=0; i<MAX_ROW; i++ ){
				m_wndHdrs.InsertItem(i, _T(""));
			}
		}

		if( m_wndSecHdr.Create(this, 0, 0, 0, 0, IDC_SECTION_HEADER) ){
			m_wndSecHdr.SetExtendedLitViewStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT /*| LVS_EX_DOUBLEBUFFER*/ );
			m_wndSecHdr.InsertColumn(0, _T("Name"));
			m_wndSecHdr.InsertColumn(1, _T("Misc(PhysicalAddress/VirtualSize)"));
			m_wndSecHdr.InsertColumn(2, _T("VirtualAddress"));
			m_wndSecHdr.InsertColumn(3, _T("SizeOfRawData"));
			m_wndSecHdr.InsertColumn(4, _T("PointerToRawData"));
			m_wndSecHdr.InsertColumn(5, _T("PointerToRelocations"));
			m_wndSecHdr.InsertColumn(6, _T("PointerToLinenumbers"));
			m_wndSecHdr.InsertColumn(7, _T("NumberOfRelocations"));
			m_wndSecHdr.InsertColumn(8, _T("NumberOfLinenumbers"));
			m_wndSecHdr.InsertColumn(9, _T("Characteristics"));
			for( int i=10; i<MAX_COL; i++ ){
				m_wndSecHdr.InsertColumn(i, _T(""), 30);
			}
			for( int i=0; i<MAX_ROW; i++ ){
				m_wndSecHdr.InsertItem(i, _T(""));
			}
		}

		if( m_wndImpEnt.Create(this, 0, 0, 0, 0, IDC_IMPORT_ENTRY) ){
			m_wndImpEnt.SetExtendedLitViewStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT /*| LVS_EX_DOUBLEBUFFER*/ );
			m_wndImpEnt.InsertColumn(0, _T("Dll"));
			m_wndImpEnt.InsertColumn(1, _T("RVA"));
			m_wndImpEnt.InsertColumn(2, _T("HINT"));
			m_wndImpEnt.InsertColumn(3, _T("NAME"), 300);
			for( int i=4; i<MAX_COL; i++ ){
				m_wndImpEnt.InsertColumn(i, _T(""), 30);
			}
			for( int i=0; i<MAX_ROW; i++ ){
				m_wndImpEnt.InsertItem(i, _T(""));
			}
		}

		if( m_wndExpEnt.Create(this, 0, 0, 0, 0, IDC_EXPORT_ENTRY) ){
			m_wndExpEnt.SetExtendedLitViewStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT /*| LVS_EX_DOUBLEBUFFER*/ );
			for( int i=0; i<MAX_COL; i++ ){
				m_wndExpEnt.InsertColumn(i, _T(""), 30);
			}
			for( int i=0; i<MAX_ROW; i++ ){
				m_wndExpEnt.InsertItem(i, _T(""));
			}
		}

		if( m_wndTabPanel.Create(this,0,0,0,0) ){
			m_wndTabPanel.Append( &m_wndDump	, _T("DUMP"));
			m_wndTabPanel.Append( &m_wndHdrs	, _T("HEADERS"));
			m_wndTabPanel.Append( &m_wndSecHdr	, _T("IMAGE_SECTION_HEADER"));
			m_wndTabPanel.Append( &m_wndImpEnt	, _T("IMPORT_ENTRY"));
			m_wndTabPanel.Append( &m_wndExpEnt	, _T("EXPORT_ENTRY"));

			m_wndTabPanel.SelectPage( 0 );
		}
	}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		InitMenu();
		InitCtrl();

		cmd.Initialize(this);
		cmd.Register(ID_FILE_OPEN, &CMainForm::OnFileOpen);
		cmd.Register(ID_FILE_SAVE, &CMainForm::OnFileSave);
		cmd.Register(ID_FILE_EXIT, &CMainForm::OnFileExit);

		LoadImageFile(NULL);

		SetFontChildren();

		return TRUE;
	}

	void	SetWindowCaption( LPTSTR lpFilename ){
		TString	str;
		str.Format(_T("ImageFile: %s\0"), lpFilename);
		SetWindowText(str);
	}

	BOOL	LoadImageFile( LPTSTR lpFilename )
	{
		if( lpFilename == NULL )
		{
			HMODULE	hModule	= ::GetModuleHandle(NULL);
			if( hModule )
			{
				IMAGE_DOS_HEADER*	pDosHdr	= (IMAGE_DOS_HEADER*)hModule;
				IMAGE_NT_HEADERS*	pNtHdrs	= (IMAGE_NT_HEADERS*)(hModule + pDosHdr->e_lfanew);
			}
		}
		else
		{
			m_wndImpEnt.DeleteAllItems();


			int			item	= 0;

			AString		str;

			if( m_ImageFile.Load( lpFilename ) )
			{
				UpdateHeaders();
				UpdateSectionHeader();
				UpdateExport();
				DumpImageFile();

				CImageFile::DllNames	dllnames	= m_ImageFile.GetImportDllNames();
				for( unsigned i=0; i<dllnames.size(); i++ )
				{
					CImageFile::SYMBOLS	symbols	= m_ImageFile.GetImportSymbols( dllnames[ i ] );

					for( unsigned j=0; j<symbols.size(); j++ )
					{
						if( symbols[ j ].import_by_name )
						{
							SetImportEntry(	item++,	dllnames[ i ]
											,	symbols[ j ].iat_entry_rva
											,	symbols[ j ].hint
											,	symbols[ j ].name
											);
						}
						else
						{
						}
					}
				}

			}
		}

		return TRUE;
	}
	/**********************************************************************************
	 *
	 */
//	void	Append( const char* pText ){
//		AString	str(pText);
//		Append( str );
//	}
	/**********************************************************************************
	 *
	 */
//	void	Append( const AString& str ){
//		TCHAR	buf[MAX_PATH];
//		TString::A2W( &str.at(0), str.length() + 1, buf, MAX_PATH );
//
//		int	idx	= m_wndDump.GetItemCount();
//		m_wndDump.InsertItem( idx, buf );
//	}
	/**********************************************************************************
	 *
	 */
	void		DumpImageFile( void )
	{
		m_wndDump.DeleteAllItems();

		TString		str;
		BYTE*			pData	= m_ImageFile.GetLoadBase();

		unsigned		item	= 0;
		unsigned		offset	= 0;
		const unsigned	siz		= m_ImageFile.GetImageSize();
		while( offset < siz )
		{
			int	cnt	= (siz - offset) > 16 ? 16 : (siz - offset);

			str.Format( _T("%08X\0"), offset );
			m_wndDump.InsertItem( item, str );

			for( int i=0; i<cnt; i++ ){
				str.Format( _T("%02X\0"), pData[ offset + i ] );
				m_wndDump.SetItem( item, i+1, str );
			}

			str.clear();
			for( int i=0; i<cnt; i++ )
			{
				if( isgraph( pData[ offset + i] ) ){
					str.AppendFormat( _T("%c"), (pData[ offset + i ]) );
				}else{
					str.AppendFormat( _T(" ") );
				}
			}
			m_wndDump.SetItem( item, 17, str );

			offset += 16;
			item	+= 1;
		}
	}
	/**********************************************************************************
	 *
	 */
	void		UpdateHeaders( void )
	{
		AString		str;
		int	item	= 0;

		SetHeaders(item++, "IMAGE_DOS_HEADER"	, str.Format("%08X\0", (LPBYTE)m_ImageFile.GetDosHeader()-m_ImageFile.GetLoadBase())	);
		SetHeaders(item++, NULL	, "WORD", "e_magic"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_magic)		, "Magic number" );
		SetHeaders(item++, NULL	, "WORD", "e_cblp"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_cblp)		, "Bytes on last page of file" );
		SetHeaders(item++, NULL	, "WORD", "e_cp"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_cp)		, "Pages in file" );
		SetHeaders(item++, NULL	, "WORD", "e_crlc"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_crlc)		, "Relocations" );
		SetHeaders(item++, NULL	, "WORD", "e_cparhdr"	, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_cparhdr)	, "Size of header in paragraphs" );
		SetHeaders(item++, NULL	, "WORD", "e_minalloc"	, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_minalloc)	, "Minimum extra paragraphs needed" );
		SetHeaders(item++, NULL	, "WORD", "e_maxalloc"	, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_maxalloc)	, "Maximum extra paragraphs needed" );
		SetHeaders(item++, NULL	, "WORD", "e_ss"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_ss)		, "Initial (relative) SS value" );
		SetHeaders(item++, NULL	, "WORD", "e_sp"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_sp)		, "Initial SP value" );
		SetHeaders(item++, NULL	, "WORD", "e_csum"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_csum)		, "Checksum" );
		SetHeaders(item++, NULL	, "WORD", "e_ip"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_ip)		, "Initial IP value" );
		SetHeaders(item++, NULL	, "WORD", "e_cs"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_cs)		, "Initial (relative) CS value" );
		SetHeaders(item++, NULL	, "WORD", "e_lfarlc"	, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_lfarlc)	, "File address of relocation table" );
		SetHeaders(item++, NULL	, "WORD", "e_ovno"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_ovno)		, "Overlay number" );
		SetHeaders(item++, NULL	, "WORD", "e_res[4]"	, "-"															, "Reserved words" );
		SetHeaders(item++, NULL	, "WORD", "e_oemid"		, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_oemid)		, "OEM identifier (for e_oeminfo)" );
		SetHeaders(item++, NULL	, "WORD", "e_oeminfo"	, str.Format("%04X\0", m_ImageFile.GetDosHeader()->e_oeminfo)	, "OEM information; e_oemid specific" );
		SetHeaders(item++, NULL	, "WORD", "e_res2[10]"	, "-"															, "Reserved words" );
		SetHeaders(item++, NULL	, "WORD", "e_lfanew"	, str.Format("%d\0", m_ImageFile.GetDosHeader()->e_lfanew)		, "File address of new exe header" );

		SetHeaders(item++, "IMAGE_NT_HEADERS"	, str.Format("%08X\0", (LPBYTE)m_ImageFile.GetNtHeaders()-m_ImageFile.GetLoadBase())	);
		SetHeaders(item++, NULL	, "DWORD"	, "Signature"					, str.Format("%08X\0", m_ImageFile.GetNtHeaders()->Signature)	);

		SetHeaders(item++, "IMAGE_FILE_HEADER"	, str.Format("%08X\0", (LPBYTE)m_ImageFile.GetFileHeader()-m_ImageFile.GetLoadBase())	);
		SetHeaders(item++, NULL	, "WORD"	, "Machine"						, str.Format("%04X\0", m_ImageFile.GetFileHeader()->Machine)				);
		SetHeaders(item++, NULL	, "WORD"	, "NumberOfSections"			, str.Format("%u\0", m_ImageFile.GetFileHeader()->NumberOfSections)		);
		SetHeaders(item++, NULL	, "DWORD"	, "TimeDateStamp"				, str.Format("%08X\0", m_ImageFile.GetFileHeader()->TimeDateStamp)			);
		SetHeaders(item++, NULL	, "DWORD"	, "PointerToSymbolTable"		, str.Format("%08X\0", m_ImageFile.GetFileHeader()->PointerToSymbolTable)	);
		SetHeaders(item++, NULL	, "DWORD"	, "NumberOfSymbols"				, str.Format("%u\0", m_ImageFile.GetFileHeader()->NumberOfSymbols)		);
		SetHeaders(item++, NULL	, "WORD"	, "SizeOfOptionalHeader"		, str.Format("%u\0", m_ImageFile.GetFileHeader()->SizeOfOptionalHeader)	);
		SetHeaders(item++, NULL	, "WORD"	, "Characteristics"				, str.Format("%04X\0", m_ImageFile.GetFileHeader()->Characteristics)		);

		SetHeaders(item++, "IMAGE_OPTIONAL_HEADER"	, str.Format("%08X\0", (LPBYTE)m_ImageFile.GetOptionalHeader()-m_ImageFile.GetLoadBase())	);
		SetHeaders(item++, NULL	, "WORD"	, "Magic"						, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->Magic)						);
		SetHeaders(item++, NULL	, "BYTE"	, "MajorLinkerVersion"			, str.Format("%02X\0", m_ImageFile.GetOptionalHeader()->MajorLinkerVersion)			);
		SetHeaders(item++, NULL	, "BYTE"	, "MinorLinkerVersion"			, str.Format("%02X\0", m_ImageFile.GetOptionalHeader()->MinorLinkerVersion)			);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfCode"					, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfCode)					);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfInitializedData"		, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfInitializedData)		);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfUninitializedData"		, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfUninitializedData)	);
		SetHeaders(item++, NULL	, "DWORD"	, "AddressOfEntryPoint"			, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->AddressOfEntryPoint)		);
		SetHeaders(item++, NULL	, "DWORD"	, "BaseOfCode"					, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->BaseOfCode)					);
		SetHeaders(item++, NULL	, "DWORD"	, "BaseOfData"					, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->BaseOfData)					);

		SetHeaders(item++, NULL	, "DWORD"	, "ImageBase"					, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->ImageBase)					);
		SetHeaders(item++, NULL	, "DWORD"	, "SectionAlignment"			, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->SectionAlignment)			);
		SetHeaders(item++, NULL	, "DWORD"	, "FileAlignment"				, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->FileAlignment)				);
		SetHeaders(item++, NULL	, "WORD"	, "MajorOperatingSystemVersion"	, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->MajorOperatingSystemVersion));
		SetHeaders(item++, NULL	, "WORD"	, "MinorOperatingSystemVersion"	, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->MinorOperatingSystemVersion));
		SetHeaders(item++, NULL	, "WORD"	, "MajorImageVersion"			, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->MajorImageVersion)			);
		SetHeaders(item++, NULL	, "WORD"	, "MinorImageVersion"			, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->MinorImageVersion)			);
		SetHeaders(item++, NULL	, "WORD"	, "MajorSubsystemVersion"		, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->MajorSubsystemVersion)		);
		SetHeaders(item++, NULL	, "WORD"	, "MinorSubsystemVersion"		, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->MinorSubsystemVersion)		);
		SetHeaders(item++, NULL	, "DWORD"	, "Win32VersionValue"			, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->Win32VersionValue)			);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfImage"					, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfImage)				);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfHeaders"				, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfHeaders)				);
		SetHeaders(item++, NULL	, "DWORD"	, "CheckSum"					, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->CheckSum)					);
		SetHeaders(item++, NULL	, "WORD"	, "Subsystem"					, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->Subsystem)					);
		SetHeaders(item++, NULL	, "WORD"	, "DllCharacteristics"			, str.Format("%04X\0", m_ImageFile.GetOptionalHeader()->DllCharacteristics)			);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfStackReserve"			, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfStackReserve)			);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfStackCommit"			, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfStackCommit)			);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfHeapReserve"			, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfHeapReserve)			);
		SetHeaders(item++, NULL	, "DWORD"	, "SizeOfHeapCommit"			, str.Format("%u\0", m_ImageFile.GetOptionalHeader()->SizeOfHeapCommit)			);
		SetHeaders(item++, NULL	, "DWORD"	, "LoaderFlags"					, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->LoaderFlags)				);

		static const char*	strOutline[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] = {
			"Export Table"		,
			"Import Table"		,
			"Resource Table"	,
			"Exception Table"	,
			"Certificate Table"	,
			"Base relocation table"	,
			"Debugging information starting address and size."			,
			"Architecture-specific data address and size."	,
			"Global pointer register relative virtual address."	,
			"TLS(thread local storage) table"		,
			"Load configuration table"	,
			"Bound Import table"	,
			"IAT(import address table)"			,
			"Delay Import Descriptor"	,
			"Reserved"			,
			"Reserved"			,
		};

		SetHeaders(item++, "IMAGE_DATA_DIRECTORY");
		for( int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++ ){
			SetHeaders(item++, str.Format("DataDirectory[%d]: %s\0", i, strOutline[ i ])	, str.Format("%08X\0", (LPBYTE)&m_ImageFile.GetOptionalHeader()->DataDirectory[ i ]-m_ImageFile.GetLoadBase())	);
			SetHeaders(item++, NULL	, "DWORD"	, "VirtualAddress"				, str.Format("%08X\0", m_ImageFile.GetOptionalHeader()->DataDirectory[ i ].VirtualAddress)	);
			SetHeaders(item++, NULL	, "DWORD"	, "Size"						, str.Format("%u\0"	, m_ImageFile.GetOptionalHeader()->DataDirectory[ i ].Size)			);
		}

		LPBYTE	pBase	= m_ImageFile.GetLoadBase();

		IMAGE_IMPORT_DESCRIPTOR* pIAT	= (IMAGE_IMPORT_DESCRIPTOR*)m_ImageFile.GetDirEntryData( IMAGE_DIRECTORY_ENTRY_IAT );
		if( pIAT )
		{
			IMAGE_IMPORT_DESCRIPTOR*	pDesc	= pIAT;

			while( pDesc->Characteristics )
			{
				if( pDesc->Name )
				{
					IMAGE_THUNK_DATA*	pThunk	= (IMAGE_THUNK_DATA*)(pBase + pDesc->OriginalFirstThunk);
				}
				pDesc++;
			}
		}
	}
	/**********************************************************************************
	 *
	 */
	void		SetHeaders( int item, const char* str1, const char* str2 = NULL, const char* str3 = NULL, const char* str4 = NULL, const char* str5 = NULL )
	{
		TCHAR	buf[MAX_PATH];
		AString	 str;

		if( str1 ){
			str.Format( "%s\0", str1 );	TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
			m_wndHdrs.SetItem( item, 0, buf );
			buf[ 0 ] = 0;
		}

		if( str2 ){
			str.Format( "%s\0", str2 );	TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
			m_wndHdrs.SetItem( item, 1, buf );
			buf[ 0 ] = 0;
		}

		if( str3 ){
			str.Format( "%s\0", str3 );	TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
			m_wndHdrs.SetItem( item, 2, buf );
			buf[ 0 ] = 0;
		}

		if( str4 ){
			str.Format( "%s\0", str4 );	TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
			m_wndHdrs.SetItem( item, 3, buf );
			buf[ 0 ] = 0;
		}

		if( str5 ){
			str.Format( "%s\0", str5 );	TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
			m_wndHdrs.SetItem( item, 4, buf );
			buf[ 0 ] = 0;
		}
	}
	/**********************************************************************************
	 *
	 */
	void		UpdateSectionHeader( void )
	{
		TCHAR		buf[MAX_PATH];
		AString		str;

		IMAGE_SECTION_HEADER*	pSecHdr	= NULL;
		int	cnt	= m_ImageFile.GetFileHeader()->NumberOfSections;
		for( int i=0; i<cnt; i++ )
		{
			pSecHdr	= m_ImageFile.GetSectionHeader( i );
			if( pSecHdr )
			{
				int	col	= 0;
				str.Format("%s\0", pSecHdr->Name);							TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->Misc.VirtualSize);			TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->VirtualAddress);				TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->SizeOfRawData);				TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->PointerToRawData);			TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->PointerToRelocations);		TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->PointerToLinenumbers);		TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->NumberOfRelocations);			TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->NumberOfLinenumbers);			TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);

				str.Format("%08X\0", pSecHdr->Characteristics);				TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
				m_wndSecHdr.SetItem(i, col++, buf);
			}
		}
	}
	/**********************************************************************************
	 *
	 */
	void		SetImportEntry( int item, const char* pDll, DWORD dwRVA, WORD hint, const char* pSymbol )
	{
		TCHAR	buf[MAX_PATH];
		AString	 str;

		str.Format("%s\0",pDll);
		TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
		m_wndImpEnt.InsertItem( item, buf );

		str.Format("0x%08X\0",dwRVA);
		TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
		m_wndImpEnt.SetItem( item, 1, buf );

		str.Format("%04d\0",hint);
		TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
		m_wndImpEnt.SetItem( item, 2, buf );

		str.Format("%s\0",pSymbol);
		TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
		m_wndImpEnt.SetItem( item, 3, buf );

		str.Format("%d\0",item);
		TString::A2W( &str.at(0), str.length()+1, buf, MAX_PATH );
		m_wndImpEnt.SetItem( item, 4, buf );
	}
	/**********************************************************************************
	 *
	 */
	void	UpdateExport( void )
	{
		TString		str;
		AString		astr;

		CImageFile::ExportSymbols	symbols	= m_ImageFile.GetExportSymbols();
		if( symbols.size() )
		{
			for( size_t i=0; i<symbols.size(); i++ )
			{
				CImageFile::EXPORT_SYMBOL&	sym	= symbols.at(i);
				str.Format(_T("%-5d"), sym.ordinal);
				m_wndExpEnt.SetItem( i, 0, str );

				str.Format(_T("%08X"), sym.symbol_rva);
				m_wndExpEnt.SetItem( i, 1, str );

				astr	= sym.name;
				TString::A2W( astr, str );
				m_wndExpEnt.SetItem( i, 2, str );

				if( sym.forwarded_to[ 0 ] )
				{
					TString::A2W( sym.forwarded_to, strlen(sym.forwarded_to)+1, &str.at(0), (int)str.size() );
				//	str.Format(_T("%*s %s"), 16, _T(""), sym.forwarded_to);
					m_wndExpEnt.SetItem( i, 3, str );
				}
			}
		}
	}
	/**********************************************************************************
	 *
	 */
	void	OnSize(UINT state, int cx, int cy){
		if( cx <= 0 || cy <= 0 )
			return;
		m_wndTabPanel.MoveWindow(0,0,cx,cy);
	}
public:
	void	OnFileOpen( void ){
		CImageOpenDlg		dlg;
		if( dlg.DoModal(m_hWnd) ){
			TString	str;
			dlg.GetFileName(str);

			SetWindowCaption( (LPTSTR)str );

#if 0
			CHAR		buf[MAX_PATH];
			TString::W2A( &str.at(0), str.length()+1, buf, MAX_PATH );

			PLOADED_IMAGE	pLoadImage	= ::ImageLoad(buf,NULL);
			if( pLoadImage )
			{
				LPBYTE	pBase	= (LPBYTE)pLoadImage->MappedAddress;
				ULONG	uSize	= 0;
				PIMAGE_IMPORT_DESCRIPTOR	pImpDesc	= 
					(PIMAGE_IMPORT_DESCRIPTOR)::ImageDirectoryEntryToData(pBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &uSize);


				TRACE("%u/%u:%u\n", sizeof(IMAGE_IMPORT_DESCRIPTOR), uSize, uSize/sizeof(IMAGE_IMPORT_DESCRIPTOR));

				PIMAGE_IMPORT_DESCRIPTOR	p	= pImpDesc;
				int	cnt	= 1;
				while( p->Name )
				{
					TRACE("[%d]%s\n", cnt, (PSTR)(pBase + p->Name));
					p++;
					cnt++;
				}

				::ImageUnload( pLoadImage );
			}
#endif			

			LoadImageFile( (LPTSTR)str );
		}
	}

	void	OnFileSave( void ){
	}

	void	OnFileExit( void ){
		DestroyWindow();
	}
};
///**********************************************************************************
// *
// *
// *
// */
//typedef int	(WINAPI* FUNC)(HWND, const char*, const char*, int);
//
//extern "C" {
//	static FUNC		pMessageBox	= NULL;
//
//	int WINAPI	MsgBox( HWND hwnd, const char* pMsg, const char* pCaption, int nType ){
//		return pMessageBox(hwnd, pMsg, "Hook", nType);
//	}
//}
//
//void	EntryHook( void )
//{
//	MessageBoxA(NULL, "Before", "Entry", MB_OK);
//
//	pMessageBox	= (FUNC)::GetProcAddress(::GetModuleHandleA("user32.dll"), "MessageBoxA");
//
//	CImageFile	image( ::GetModuleHandleA(NULL) );
//	image.HookImportSymbol( pMessageBox, &MsgBox );
//
//	MessageBoxA(NULL, "After", "Entry", MB_OK);
//}
/**********************************************************************************
 *
 *
 *
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

//	EntryHook();

	CMainForm	Form;
	if( Form.Create(_T("ImageFile"), 0, 0, 100, 100, 600, 300) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
}
