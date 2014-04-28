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
	int							m_nSectionHeaderCount;

public:
	BYTE*						GetBase( void ) const				{ return(m_pBase);							}
	IMAGE_DOS_HEADER*			GetDosHeader( void ) const			{ return(m_pDosHeader);						}
	IMAGE_NT_HEADERS*			GetNtHeader( void ) const			{ return(m_pNtHeaders);						}
	IMAGE_FILE_HEADER*			GetFileHeader( void ) const			{ return(&m_pNtHeaders->FileHeader);		}
	IMAGE_OPTIONAL_HEADER32*	GetOptionalHeader( void ) const		{ return(&m_pNtHeaders->OptionalHeader);	}
	IMAGE_SECTION_HEADER*		GetSectionHeader( int nIdx ) const	{ return(&m_pSectionTable[ nIdx ]);			}
	int							GetSectionHeaderCount( void ) const	{ return(m_nSectionHeaderCount);			}

public:
	CPEFile( void ) 
		: m_pBase(NULL)
		, m_pDosHeader(NULL)
		, m_pNtHeaders(NULL)
		, m_pSectionTable(NULL)
	{
	}

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
	bool	LoadImageFile( const WCHAR* pImageFile )
	{
		std::vector<BYTE>	Buffer;

		CReadOnlyFile	file( pImageFile );
		if( file.IsOpen() )
		{
			DWORD	dwSize	= 0;
			dwSize	= file.GetFileSize();

			Buffer.resize( dwSize );

			file.Read( &Buffer.at(0), dwSize );
			file.Close();
		}

		BYTE*	pData	= &Buffer.at(0);
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
			m_pDosHeader			= reinterpret_cast<IMAGE_DOS_HEADER*>( m_pBase );
			m_pNtHeaders			= reinterpret_cast<IMAGE_NT_HEADERS*>( m_pBase + m_pDosHeader->e_lfanew );
			m_pSectionTable			= reinterpret_cast<IMAGE_SECTION_HEADER*>( m_pNtHeaders + 1 );
			m_nSectionHeaderCount	= m_pNtHeaders->FileHeader.NumberOfSections;
			return(true);
		}

		return(false);
	}

	void	UnloadImageFile( void ){
		if( m_pBase ){
			VirtualFree( m_pBase, 0, MEM_RELEASE );
			m_pBase	= NULL;
		}

		m_pBase			= NULL;
		m_pDosHeader	= NULL;
		m_pNtHeaders	= NULL;
		m_pSectionTable	= NULL;
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
	//CPEFileForm( void ) : m_strPEFilename(_T("")) {
	//}

	//CPEFileForm( LPCTSTR lpPEFilename ) : m_strPEFilename(lpPEFilename) {
	//}

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

	CPEFile			m_PEFile;

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
			m_wndList1.InsertColumn(0, _T(""));
			m_wndList1.InsertColumn(1, _T(""), 200);
			m_wndList1.InsertColumn(2, _T(""), 200);
			m_wndList1.InsertColumn(3, _T(""), 200);
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
		str.Format(_T("PE File : %s"), szBuf);
		SetWindowText(str);

	//	m_wndList1.InsertItem( 0 , szBuf );
		AppendDOSHeader(NULL);
		AppendNTHeaders(NULL);

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
		CExeOpenDlg	dlg;
		if( dlg.DoModal( m_hWnd ) )
		{
			TString	str;
			dlg.GetFileName( str );

		//	str = ( _T("..\\Debug\\HelloWorld.exe"));
			m_PEFile.LoadImageFile( str );
			MappingHeader();
		}
	}	

	void	OnExit( void ){
		DestroyWindow();
	}

	void	MappingHeader( void ){
		m_wndList1.DeleteAllItems();

		AppendDOSHeader( m_PEFile.GetDosHeader() );
		AppendNTHeaders( m_PEFile.GetNtHeader() );
		AppendSectoinHeader( m_PEFile.GetSectionHeader( 0 ), m_PEFile.GetSectionHeaderCount() );
	}

	void	AppendDOSHeader( IMAGE_DOS_HEADER* pDosHeader ){
		AppendList( _T("IMAGE_DOS_HEADER") );
		if( pDosHeader ){
			AppendList(	_T("WORD")	,	_T("e_magic")		,	_T("Magic number")						,	pDosHeader->e_magic				);
			AppendList(	_T("WORD")	,	_T("e_cblp")		,	_T("Bytes on last page of file")		,	pDosHeader->e_cblp				);
			AppendList(	_T("WORD")	,	_T("e_cp")			,	_T("Pages in file")						,	pDosHeader->e_cp				);
			AppendList(	_T("WORD")	,	_T("e_crlc")		,	_T("Relocations")						,	pDosHeader->e_crlc				);
			AppendList(	_T("WORD")	,	_T("e_cparhdr")		,	_T("Size of header in paragraphs")		,	pDosHeader->e_cparhdr			);
			AppendList(	_T("WORD")	,	_T("e_minalloc")	,	_T("Minimum extra paragraphs needed")	,	pDosHeader->e_minalloc			);
			AppendList(	_T("WORD")	,	_T("e_maxalloc")	,	_T("Maximum extra paragraphs needed")	,	pDosHeader->e_maxalloc			);
			AppendList(	_T("WORD")	,	_T("e_ss")			,	_T("Initial (relative) SS value")		,	pDosHeader->e_ss				);
			AppendList(	_T("WORD")	,	_T("e_sp")			,	_T("Initial SP value")					,	pDosHeader->e_sp				);
			AppendList(	_T("WORD")	,	_T("e_csum")		,	_T("Checksum")							,	pDosHeader->e_csum				);
			AppendList(	_T("WORD")	,	_T("e_ip")			,	_T("Initial IP value")					,	pDosHeader->e_ip				);
			AppendList(	_T("WORD")	,	_T("e_cs")			,	_T("Initial (relative) CS value")		,	pDosHeader->e_cs				);
			AppendList(	_T("WORD")	,	_T("e_lfarlc")		,	_T("File address of relocation table")	,	pDosHeader->e_lfarlc			);
			AppendList(	_T("WORD")	,	_T("e_ovno")		,	_T("Overlay number")					,	pDosHeader->e_ovno				);
			AppendList(	_T("WORD")	,	_T("e_res[0]")		,	_T("Reserved words")					,	pDosHeader->e_res[0]			);
			AppendList(	_T("WORD")	,	_T("e_res[1]")		,	_T("Reserved words")					,	pDosHeader->e_res[1]			);
			AppendList(	_T("WORD")	,	_T("e_res[2]")		,	_T("Reserved words")					,	pDosHeader->e_res[2]			);
			AppendList(	_T("WORD")	,	_T("e_res[3]")		,	_T("Reserved words")					,	pDosHeader->e_res[3]			);
			AppendList(	_T("WORD")	,	_T("e_oemid")		,	_T("OEM identifier (for e_oeminfo)")	,	pDosHeader->e_oemid				);
			AppendList(	_T("WORD")	,	_T("e_oeminfo")		,	_T("OEM information; e_oemid specific")	,	pDosHeader->e_oeminfo			);
			AppendList(	_T("WORD")	,	_T("e_res2[10]")	,	_T("Reserved words")					,	pDosHeader->e_res2[0]			);
			AppendList(	_T("LONG")	,	_T("e_lfanew")		,	_T("File address of new exe header")	,	pDosHeader->e_lfanew			);
		}else{
			AppendList(	_T("WORD")	,	_T("e_magic\0")		,	_T("Magic number")							);
			AppendList(	_T("WORD")	,	_T("e_cblp\0")		,	_T("Bytes on last page of file")			);
			AppendList(	_T("WORD")	,	_T("e_cp")			,	_T("Pages in file")							);
			AppendList(	_T("WORD")	,	_T("e_crlc")		,	_T("Relocations")							);
			AppendList(	_T("WORD")	,	_T("e_cparhdr")		,	_T("Size of header in paragraphs")			);
			AppendList(	_T("WORD")	,	_T("e_minalloc")	,	_T("Minimum extra paragraphs needed")		);
			AppendList(	_T("WORD")	,	_T("e_maxalloc")	,	_T("Maximum extra paragraphs needed")		);
			AppendList(	_T("WORD")	,	_T("e_ss")			,	_T("Initial (relative) SS value")			);
			AppendList(	_T("WORD")	,	_T("e_sp")			,	_T("Initial SP value")						);
			AppendList(	_T("WORD")	,	_T("e_csum")		,	_T("Checksum")								);
			AppendList(	_T("WORD")	,	_T("e_ip")			,	_T("Initial IP value")						);
			AppendList(	_T("WORD")	,	_T("e_cs")			,	_T("Initial (relative) CS value")			);
			AppendList(	_T("WORD")	,	_T("e_lfarlc")		,	_T("File address of relocation table")		);
			AppendList(	_T("WORD")	,	_T("e_ovno")		,	_T("Overlay number")						);
			AppendList(	_T("WORD")	,	_T("e_res[4]")		,	_T("Reserved words")						);
			AppendList(	_T("WORD")	,	_T("e_oemid")		,	_T("OEM identifier (for e_oeminfo)")		);
			AppendList(	_T("WORD")	,	_T("e_oeminfo")		,	_T("OEM information; e_oemid specific")		);
			AppendList(	_T("WORD")	,	_T("e_res2[10]")	,	_T("Reserved words")						);
			AppendList(	_T("LONG")	,	_T("e_lfanew")		,	_T("File address of new exe header")		);
		}
	}

	void	AppendNTHeaders( IMAGE_NT_HEADERS* pNtHeader ){
		AppendList( _T("IMAGE_NT_HEADERS") );

		if( pNtHeader )
		{
			AppendList( _T("DWORD"), _T("Signature"), _T(""), pNtHeader->Signature );
			AppendFileHeader( &pNtHeader->FileHeader );
			AppendOptionalHeader( &pNtHeader->OptionalHeader );
		}
		else
		{
			AppendList( _T("DWORD"), _T("Signature"), _T("") );
			AppendFileHeader( NULL );
			AppendOptionalHeader( NULL );
		}
	}

	void	AppendFileHeader( IMAGE_FILE_HEADER* pFileHeader ){
		AppendList( _T("IMAGE_FILE_HEADER") );
		if( pFileHeader ){
			AppendList( _T("WORD")	, _T("Machine")					, _T("")	, pFileHeader->Machine );
			AppendList( _T("WORD")	, _T("NumberOfSections")		, _T("")	, pFileHeader->NumberOfSections );
			AppendList( _T("DWORD")	, _T("TimeDateStamp")			, _T("")	, pFileHeader->TimeDateStamp );
			AppendList( _T("DWORD")	, _T("PointerToSymbolTable")	, _T("")	, pFileHeader->PointerToSymbolTable );
			AppendList( _T("DWORD")	, _T("NumberOfSymbols")			, _T("")	, pFileHeader->NumberOfSymbols );
			AppendList( _T("WORD")	, _T("SizeOfOptionalHeader")	, _T("")	, pFileHeader->SizeOfOptionalHeader );
			AppendList( _T("WORD")	, _T("Characteristics")			, _T("")	, pFileHeader->Characteristics );
		}else{
			AppendList( _T("WORD")	, _T("Machine")					, _T("")	);
			AppendList( _T("WORD")	, _T("NumberOfSections")		, _T("")	);
			AppendList( _T("DWORD")	, _T("TimeDateStamp")			, _T("")	);
			AppendList( _T("DWORD")	, _T("PointerToSymbolTable")	, _T("")	);
			AppendList( _T("DWORD")	, _T("NumberOfSymbols")			, _T("")	);
			AppendList( _T("WORD")	, _T("SizeOfOptionalHeader")	, _T("")	);
			AppendList( _T("WORD")	, _T("Characteristics")			, _T("")	);
		}
	}

	void	AppendOptionalHeader( IMAGE_OPTIONAL_HEADER32* pOptHeader ){
		AppendList( _T("IMAGE_OPTIONAL_HEADER32") );
		if( pOptHeader ){
			AppendList( _T("WORD")		, _T("Magic")						, _T("")	, pOptHeader->Magic			);
			AppendList( _T("WORD")		, _T("MajorLinkerVersion")			, _T("")	, pOptHeader->MajorLinkerVersion			);
			AppendList( _T("WORD")		, _T("MinorLinkerVersion")			, _T("")	, pOptHeader->MinorLinkerVersion			);
			AppendList( _T("DWORD")		, _T("SizeOfCode")					, _T("")	, pOptHeader->SizeOfCode			);
			AppendList( _T("DWORD")		, _T("SizeOfInitializedData")		, _T("")	, pOptHeader->SizeOfInitializedData			);
			AppendList( _T("DWORD")		, _T("SizeOfUninitializedData")		, _T("")	, pOptHeader->SizeOfUninitializedData			);
			AppendList( _T("DWORD")		, _T("AddressOfEntryPoint")			, _T("")	, pOptHeader->AddressOfEntryPoint			);
			AppendList( _T("DWORD")		, _T("BaseOfCode")					, _T("")	, pOptHeader->BaseOfCode			);
			AppendList( _T("DWORD")		, _T("BaseOfData")					, _T("")	, pOptHeader->BaseOfData			);
			AppendList( _T("DWORD")		, _T("ImageBase")					, _T("")	, pOptHeader->ImageBase				);
			AppendList( _T("DWORD")		, _T("SectionAlignment")			, _T("")	, pOptHeader->SectionAlignment			);
			AppendList( _T("DWORD")		, _T("FileAlignment")				, _T("")	, pOptHeader->FileAlignment				);
			AppendList( _T("WORD")		, _T("MajorOperatingSystemVersion")	, _T("")	, pOptHeader->MajorOperatingSystemVersion			);
			AppendList( _T("WORD")		, _T("MinorOperatingSystemVersion")	, _T("")	, pOptHeader->MinorOperatingSystemVersion			);
			AppendList( _T("WORD")		, _T("MajorImageVersion")			, _T("")	, pOptHeader->MajorImageVersion			);
			AppendList( _T("WORD")		, _T("MinorImageVersion")			, _T("")	, pOptHeader->MinorImageVersion			);
			AppendList( _T("WORD")		, _T("MajorSubsystemVersion")		, _T("")	, pOptHeader->MajorSubsystemVersion		);
			AppendList( _T("WORD")		, _T("MinorSubsystemVersion")		, _T("")	, pOptHeader->MinorSubsystemVersion		);
			AppendList( _T("DWORD")		, _T("Win32VersionValue")			, _T("")	, pOptHeader->Win32VersionValue			);
			AppendList( _T("DWORD")		, _T("SizeOfImage")					, _T("")	, pOptHeader->SizeOfImage				);
			AppendList( _T("DWORD")		, _T("SizeOfHeaders")				, _T("")	, pOptHeader->SizeOfHeaders				);
			AppendList( _T("DWORD")		, _T("CheckSum")					, _T("")	, pOptHeader->CheckSum					);
			AppendList( _T("WORD")		, _T("Subsystem")					, _T("")	, pOptHeader->Subsystem					);
			AppendList( _T("WORD")		, _T("DllCharacteristics")			, _T("")	, pOptHeader->DllCharacteristics		);
			AppendList( _T("DWORD")		, _T("SizeOfStackReserve")			, _T("")	, pOptHeader->SizeOfStackReserve		);
			AppendList( _T("DWORD")		, _T("SizeOfStackCommit")			, _T("")	, pOptHeader->SizeOfStackCommit			);
			AppendList( _T("DWORD")		, _T("SizeOfHeapReserve")			, _T("")	, pOptHeader->SizeOfHeapReserve			);
			AppendList( _T("DWORD")		, _T("SizeOfHeapCommit")			, _T("")	, pOptHeader->SizeOfHeapCommit			);
			AppendList( _T("DWORD")		, _T("LoaderFlags")					, _T("")	, pOptHeader->LoaderFlags				);
			AppendList( _T("DWORD")		, _T("NumberOfRvaAndSizes")			, _T("")	, pOptHeader->NumberOfRvaAndSizes		);

			AppendDataDirctory( pOptHeader->DataDirectory );
		}else{
			AppendList( _T("WORD")		, _T("Magic")						, _T("") );
			AppendList( _T("WORD")		, _T("MajorLinkerVersion")			, _T("") );
			AppendList( _T("WORD")		, _T("MinorLinkerVersion")			, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfCode")					, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfInitializedData")		, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfUninitializedData")		, _T("") );
			AppendList( _T("DWORD")		, _T("AddressOfEntryPoint")			, _T("") );
			AppendList( _T("DWORD")		, _T("BaseOfCode")					, _T("") );
			AppendList( _T("DWORD")		, _T("BaseOfData")					, _T("") );
			AppendList( _T("DWORD")		, _T("ImageBase")					, _T("") );
			AppendList( _T("DWORD")		, _T("SectionAlignment")			, _T("") );
			AppendList( _T("DWORD")		, _T("FileAlignment")				, _T("") );
			AppendList( _T("WORD")		, _T("MajorOperatingSystemVersion")	, _T("") );
			AppendList( _T("WORD")		, _T("MinorOperatingSystemVersion")	, _T("") );
			AppendList( _T("WORD")		, _T("MajorImageVersion")			, _T("") );
			AppendList( _T("WORD")		, _T("MinorImageVersion")			, _T("") );
			AppendList( _T("WORD")		, _T("MajorSubsystemVersion")		, _T("") );
			AppendList( _T("WORD")		, _T("MinorSubsystemVersion")		, _T("") );
			AppendList( _T("DWORD")		, _T("Win32VersionValue")			, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfImage")					, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfHeaders")				, _T("") );
			AppendList( _T("DWORD")		, _T("CheckSum")					, _T("") );
			AppendList( _T("WORD")		, _T("Subsystem")					, _T("") );
			AppendList( _T("WORD")		, _T("DllCharacteristics")			, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfStackReserve")			, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfStackCommit")			, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfHeapReserve")			, _T("") );
			AppendList( _T("DWORD")		, _T("SizeOfHeapCommit")			, _T("") );
			AppendList( _T("DWORD")		, _T("LoaderFlags")					, _T("") );
			AppendList( _T("DWORD")		, _T("NumberOfRvaAndSizes")			, _T("") );
		}
	}

	void	AppendDataDirctory( IMAGE_DATA_DIRECTORY* pDataDirectory ){
		static TCHAR*	strOutline[] = {
			_T("Export Table")	,
			_T("Import Table")	,
			_T("Resource Table")	,
			_T("Exception Table")	,
			_T("Certificate Table")	,
			_T("Base Relocation Table")	,
			_T("Debug")	,
			_T("Architecture")	,
			_T("Global Ptr")	,
			_T("TLS Table")	,
			_T("Load Config Table")	,
			_T("Bound Import")	,
			_T("IAT")	,
			_T("Delay Import Descriptor")	,
			_T("Reserved")	,
		};
		TString	str;
		for( int i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++ )
		{
			str.Format( _T("IMAGE_DATA_DIRECTORY[%d]"), i );
			AppendList( str, strOutline[ i ], _T("") );

			if( pDataDirectory ){
				AppendList( _T("DWORD")		, _T("VirtualAddress")		, _T("")	, pDataDirectory->VirtualAddress	);
				AppendList( _T("DWORD")		, _T("Size")				, _T("")	, pDataDirectory->Size				);
			}else{
				AppendList( _T("DWORD")		, _T("VirtualAddress")		, _T("")	);
				AppendList( _T("DWORD")		, _T("Size")				, _T("")	);
			}

			pDataDirectory++;
		}
	}

	void	AppendSectoinHeader( IMAGE_SECTION_HEADER* pSectionHeader, int nCount ){
		TString	str;
		for( int i=0; i<nCount; i++ )
		{
			str.Format( _T("IMAGE_SECTION_HEADER[%d]"), i );
			AppendList( str );
			if( pSectionHeader ){
				AppendList( _T("BYTE")		, _T("Name")						, _T("")			, (CHAR*)(pSectionHeader->Name), IMAGE_SIZEOF_SHORT_NAME );
				AppendList( _T("DWORD")		, _T("VirtualSize")					, _T("")			, pSectionHeader->Misc.VirtualSize		);
				AppendList( _T("DWORD")		, _T("VirtualAddress")				, _T("")			, pSectionHeader->VirtualAddress		);
				AppendList( _T("DWORD")		, _T("SizeOfRawData")				, _T("")			, pSectionHeader->SizeOfRawData			);
				AppendList( _T("DWORD")		, _T("PointerToRawData")			, _T("")			, pSectionHeader->PointerToRawData		);
				AppendList( _T("DWORD")		, _T("PointerToRelocations")		, _T("")			, pSectionHeader->PointerToRelocations	);
				AppendList( _T("DWORD")		, _T("PointerToLinenumbers")		, _T("")			, pSectionHeader->PointerToLinenumbers	);
				AppendList( _T("WORD")		, _T("NumberOfRelocations")			, _T("")			, pSectionHeader->NumberOfRelocations	);
				AppendList( _T("WORD")		, _T("NumberOfLinenumbers")			, _T("")			, pSectionHeader->NumberOfLinenumbers	);
				AppendList( _T("DWORD")		, _T("Characteristics")				, _T("")			, pSectionHeader->Characteristics		);
			}
			pSectionHeader++;
		}
	}

	void	AppendList( LPTSTR pText ){
		int	item	= m_wndList1.GetItemCount();
		m_wndList1.InsertItem( item, pText );
	}

	void	AppendList( LPTSTR pType, LPTSTR pMember, LPTSTR pSummary ){
		int	item	= m_wndList1.GetItemCount();
		m_wndList1.InsertItem( item, pType );
		m_wndList1.SetItem( item, 1, pMember );
		m_wndList1.SetItem( item, 2, pSummary );
	}

	void	AppendList( LPTSTR pType, LPTSTR pMember, LPTSTR pSummary, LPTSTR pValue ){
		int	item	= m_wndList1.GetItemCount();
		m_wndList1.InsertItem( item, pType );
		m_wndList1.SetItem( item, 1, pMember );
		m_wndList1.SetItem( item, 2, pSummary );
		m_wndList1.SetItem( item, 3, pValue );
	}

	void	AppendList( LPTSTR pType, LPTSTR pMember, LPTSTR pSummary, BYTE&	bValue ){
		TString	str;
		str.Format(_T("0x%02X\0"), bValue);
		AppendList(pType, pMember, pSummary, (LPTSTR)str);
	}

	void	AppendList( LPTSTR pType, LPTSTR pMember, LPTSTR pSummary, WORD&	wValue ){
		TString	str;
		str.Format(_T("0x%04X\0"), wValue);
		AppendList(pType, pMember, pSummary, (LPTSTR)str);
	}

	void	AppendList( LPTSTR pType, LPTSTR pMember, LPTSTR pSummary, DWORD& dwValue ){
		TString	str;
		str.Format(_T("0x%08X\0"), dwValue);
		AppendList(pType, pMember, pSummary, (LPTSTR)str);
	}

	void	AppendList( LPTSTR pType, LPTSTR pMember, LPTSTR pSummary, LONG& lValue ){
		TString	str;
		str.Format(_T("%d\0"), lValue);
		AppendList(pType, pMember, pSummary, (LPTSTR)str);
	}

	void	AppendList( LPTSTR pType, LPTSTR pMember, LPTSTR pSummary, const CHAR* pValue, int nMax ){
		WCHAR	buf[MAX_PATH];
		ZeroMemory(buf, sizeof(buf));
		TString::A2W( pValue, nMax, buf, _countof(buf));
		AppendList(pType, pMember, pSummary, buf);
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

	CPEFileUIThread( void ) : m_dwPID(NULL) {
	}

	CPEFileUIThread( DWORD dwPID ) : m_dwPID(dwPID) {
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
