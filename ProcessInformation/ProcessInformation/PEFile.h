#pragma once

#include "ProcessInformation.h"
#include "FileOpenDlg.h"
#include "UseToolkit.h"

#include "File.h"

/**********************************************************************************
 *
 *
 *
 */
class CPEFile {
protected:
	BYTE*						m_pBase;
	IMAGE_DOS_HEADER*			m_pDosHeader;
	IMAGE_NT_HEADERS*			m_pNtHeaders;
	IMAGE_SECTION_HEADER*		m_pSectionTable;

public:
	BYTE*						GetBase( void ) const				{ return(m_pBase);							}
	IMAGE_DOS_HEADER*			GetDosHeader( void ) const			{ return(m_pDosHeader);						}
	IMAGE_NT_HEADERS*			GetNtHeader( void ) const			{ return(m_pNtHeaders);						}
	IMAGE_FILE_HEADER*			GetFileHeader( void ) const			{ return(&m_pNtHeaders->FileHeader);		}
	IMAGE_OPTIONAL_HEADER32*	GetOptionalHeader( void ) const		{ return(&m_pNtHeaders->OptionalHeader);	}
	IMAGE_SECTION_HEADER*		GetSectionHeader( int nIdx ) const	{ return(&m_pSectionTable[ nIdx ]);			}

public:
	explicit CPEFile( const CHAR* pImageFile )
		: m_pBase(NULL)
		, m_pDosHeader(NULL)
		, m_pNtHeaders(NULL)
		, m_pSectionTable(NULL)
	{
	}

	~CPEFile( void ){
		UnloadImageFile();
	}

public:
	bool	LoadImageFile( const CHAR* pImageFile ){

		BYTE*	pData	= NULL;
		if( pData == NULL ){
			return(false);
		}

		IMAGE_DOS_HEADER*		pDosHeader		= (IMAGE_DOS_HEADER*)( pData );
		if( pDosHeader ){
			if( (pDosHeader->e_magic != *(WORD*)("MZ")) || (pDosHeader->e_lfanew == 0) ){
				return(false);
			}
		}

		IMAGE_NT_HEADERS*		pNtHeaders		= (IMAGE_NT_HEADERS*)( &pData[ pDosHeader->e_lfanew ] );
		if( pNtHeaders ){
			if( (pNtHeaders->Signature != *(DWORD*)("PE\0\0")) || (pNtHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) ){
				return(false);
			}
		}

		IMAGE_SECTION_HEADER*	pSectionTable	= (IMAGE_SECTION_HEADER*)( pNtHeaders + 1 );
		if( pSectionTable ){

			BYTE*	pVMem	= (BYTE*)VirtualAlloc( NULL, pNtHeaders->OptionalHeader.SizeOfImage, MEM_COMMIT, PAGE_READWRITE );
			if( pVMem == NULL ){
				return(false);
			}

			memcpy( pVMem, pData, pNtHeaders->OptionalHeader.SizeOfHeaders );

			for( unsigned i=0; i<pNtHeaders->FileHeader.NumberOfSections; i++ )
			{
				if( pSectionTable[ i ].PointerToRawData )
				{
					memcpy( &pVMem[ pSectionTable[ i ].VirtualAddress ], &pData[ pSectionTable[ i ].PointerToRawData ], pSectionTable[ i ].SizeOfRawData );
				}
			}

			m_pBase	= pVMem;
		}

		if( m_pBase )
		{
			m_pDosHeader	= reinterpret_cast<IMAGE_DOS_HEADER*>( m_pBase );
			m_pNtHeaders	= reinterpret_cast<IMAGE_NT_HEADERS*>( m_pBase + m_pDosHeader->e_lfanew );
			m_pSectionTable	= reinterpret_cast<IMAGE_SECTION_HEADER*>( m_pNtHeaders + 1 );

			return(true);
		}

		return(false);
	}

	void	UnloadImageFile( void ){
		if( m_pBase ){
			VirtualFree( m_pBase, 0, MEM_RELEASE );
			m_pBase	= NULL;
		}
	}

public:
	BYTE*	GetDirEntryData( int idx ) const {
		if( m_pBase ){
			if( (0 <= idx) && (idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) 
			&&	(GetOptionalHeader()->DataDirectory[ idx ].VirtualAddress)
			&&	(GetOptionalHeader()->DataDirectory[ idx ].Size)			)
			{
				return(m_pBase + GetOptionalHeader()->DataDirectory[ idx ].VirtualAddress);
			}
		}
		return(NULL);
	}

	DWORD	GetDirEntryDataSize( int idx ) const {
		if( m_pBase ){
			if( (0 <= idx) && (idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) )
			{
				return(GetOptionalHeader()->DataDirectory[ idx ].Size);
			}
		}
		return(0);
	}

	IMAGE_IMPORT_DESCRIPTOR*	GetImportDir( void ) const {
		return (IMAGE_IMPORT_DESCRIPTOR*)GetDirEntryData( IMAGE_DIRECTORY_ENTRY_IMPORT );
	}

	void	GetImportDllNames( std::vector<const char*>& vec ) const {
		vec.clear();

		IMAGE_IMPORT_DESCRIPTOR*	pImpDesc	= GetImportDir();
		if( pImpDesc ){
			for( int i=0; pImpDesc[ i ].FirstThunk; i++ ){
				vec.push_back( (const char*)(pImpDesc[ i ].Name) );
			}
		}
	}

	template<typename T>
	T	GetData( DWORD dw ){
		return (T)(m_pBase + dw);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CPEFileForm : public TSplitter<CForm, CVerticalSplitter> {
public:
	CPEFileForm( void ) : m_strPEFilename(_T("")) {
	}

	CPEFileForm( LPCTSTR lpPEFilename ) : m_strPEFilename(lpPEFilename) {
	}

	CPEFileForm( DWORD dwPID ) : m_dwPID(dwPID) {
	}

protected:
	enum {
		ID_MENU			= 100	,
		ID_FILE					,
		ID_FILE_OPEN			,
		ID_FILE_EXIT			,

		IDC				= 1000	,
		IDC_LISTVIEW			,
	};

	TString			m_strPEFilename;
	DWORD			m_dwPID;

	CListView		m_wndList1;
	CListView		m_wndListSymbol;

	TCommandHandler<CPEFileForm>	cmd;

	void	InitMenu( void ){
		CPopupMenu	menuFile;
		menuFile.Create();
		menuFile.Insert(_T("Open"), ID_FILE_OPEN);
		menuFile.Insert(_T("Exit"), ID_FILE_EXIT);

		CMenu	menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File"), ID_FILE, (HMENU)menuFile);
		
		SetMenu(m_hWnd, (HMENU)menuMain);
	}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		InitMenu();

		int	w	= lpCreateStruct->cx;
		int	h	= lpCreateStruct->cy;

		if( m_wndList1.Create(this, 0, 0, w, h, 1001) ){
			m_wndList1.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT );
			m_wndList1.InsertColumn(0, _T(""), 200);
		}

		if( m_wndListSymbol.Create(this, 0, 0, w, h, 1002) ){
			m_wndListSymbol.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT );
			m_wndListSymbol.InsertColumn(0, _T("DLL"));
			m_wndListSymbol.InsertColumn(1, _T("RVA"));
			m_wndListSymbol.InsertColumn(2, _T("Symbol"));
			m_wndListSymbol.InsertColumn(3, _T("hint"));
			m_wndListSymbol.InsertColumn(4, _T("Ordinal"));
		}

		cmd.Initialize(this);
		cmd.Register(ID_FILE_OPEN, &CPEFileForm::OnOpen);
		cmd.Register(ID_FILE_EXIT, &CPEFileForm::OnExit);

		m_Splitter.Append( &m_wndList1 );
		m_Splitter.Append( &m_wndListSymbol );
		m_Splitter.Bind( this );

		CProcessInfo	psinfo(m_dwPID);
		TCHAR	szBuf[MAX_PATH];
		psinfo.GetProcessImageFileNameEx(szBuf, MAX_PATH);

		TString	str;
		GetWindowText(str);
		str.Format(_T("PE File : %s"), szBuf);
		SetWindowText(str);

		m_wndList1.InsertItem( 0 , szBuf );

#ifdef PEFILE
		AString		str;
		WCHAR		szBuf[256];

		PEFile		pe((LPTSTR)m_strPEFilename);
		std::vector<const char*>	dlls	= pe.GetImportDllNames();

		unsigned item = 0;
		for( unsigned i=0; i<dlls.size(); i++ )
		{
			std::vector<ImportSymbol>	symbols	= pe.GetImportSymbols( dlls[ i ] );

			for( unsigned j=0; j<symbols.size(); j++ )
			{
				str.Format( "%s\0", dlls[ i ] );
				TString::A2W( &str.at(0), str.Length()+1, szBuf, _countof(szBuf) );
				m_wndListSymbol.InsertItem( item, szBuf );

				str.Format( "0x%08X", symbols[ j ].iat_entry_rva );
				TString::A2W( &str.at(0), str.Length()+1, szBuf, _countof(szBuf) );
				m_wndListSymbol.SetItem( item, 1, szBuf );

				if( symbols[ j ].import_by_name )
				{
					str.Format( "%s\0", symbols[ j ].name );
					TString::A2W( &str.at(0), str.Length()+1, szBuf, _countof(szBuf) );
					m_wndListSymbol.SetItem( item, 2, szBuf );
					
					str.Format( "%04d", symbols[ j ].hint );
					TString::A2W( &str.at(0), str.Length()+1, szBuf, _countof(szBuf) );
					m_wndListSymbol.SetItem( item, 3, szBuf );
				}
				else
				{
					str.Format( "%d\0", symbols[ j ].ordinal );
					TString::A2W( &str.at(0), str.Length()+1, szBuf, _countof(szBuf) );
					m_wndListSymbol.SetItem( item, 4, szBuf );
				}


				item++;
			}
		}
#endif
		return TRUE;
	}

	void	OnCommand(UINT nID, HWND, UINT nNotifyCode){
		cmd.Dispach(nID, nNotifyCode);
	}

	void	OnOpen( void ){
	//	CExeOpenDlg	dlg;
	//	if( dlg.DoModal( m_hWnd ) )
		{
	//		TString	str;
	//		dlg.GetFileName( str );

			TString	str( _T("C:\\Users\\jupiter\\Garage\\ProcessInformation\\Debug\\HelloWorld.exe"));
			Load( str );
		}
	}	

	void	OnExit( void ){
		DestroyWindow();
	}


	void	Load( TString& strFileName ){
		TRACE( strFileName );

		CFile	file( strFileName );
		if( file.IsOpen() )
		{
			DWORD	dwSize	= 0;
			file.GetFileSize( &dwSize );

			LARGE_INTEGER	filesize;
			::GetFileSizeEx( (HANDLE)file, &filesize );

			std::vector<BYTE>	buff;
			buff.resize( dwSize );

			file.Read( &buff.at(0), dwSize );

		}
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CPEFileUIThread : public IRunnable {
public:
	DWORD		m_dwPID;

	CPEFileUIThread( void ){
	}

	CPEFileUIThread( DWORD dwPID ) : m_dwPID(dwPID){
	}


	void	run( void ){
		CPEFileForm	Form(m_dwPID);
		if( Form.Create(_T("PE File"), 0, 0, 100, 100, 300, 300) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};
