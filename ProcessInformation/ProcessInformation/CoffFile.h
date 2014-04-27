#pragma once

/*
	coff file format.
	-----------------------------
	|	coff file header		|
	-----------------------------
	|	section header 1		|
	|			:				|
	|	section header n		|
	-----------------------------
	|	section deta 1			|
	|			:				|
	|	section data n			|
	-----------------------------
	|	symbol infomation 1		|
	|	:						|
	|	symbol infomation n		|
	-----------------------------
	|	long symbol name 		|
	-----------------------------

	coff file header: IMAGE_FILE_HEADER structure
	|	Machine					| WORD			|
			IMAGE_FILE_MACHINE_I386
			IMAGE_FILE_MACHINE_UNKNOWN

	|	NumberOfSections		| WORD			|
			IMAGE_FILE_MACHINE_UNKNOWN + IMPORT_OBJECT_HDR_SIG2

	|	TimeDataStamp			| DWROD			|

	|	PointerToSymbolTable	| DWROD			|
			シンボルテーブルのオフセット

	|	NumberOfSymbols			| DWROD			|
	|	SizeOfOptionalHeader	| WORD			|
	|	Characteristics			| WORD			|


	section table : IMAGE_SECTION_HEADER structure
	|	Name					| BYTE			|

	|	Misc					| DWORD(UNION)	|
			PhisicalAddress
			VirtualSize

	|	VirtualAddress			| DWORD			|
	|	SizeOfRawData			| DWORD			|
		セクションデータのサイズ

	|	PointerToRelacations	| DWORD			|
	|	PointerTolineunmbers	| DWORD			|
	|	NumberOfRelocations		| DWORD			|
	|	NumberOfLinenumbers		| WORD			|
	|	Chracteristics			| DWORD			|


 */

#include "FileOpenDlg.h"

#include "UseToolkit.h"
#include "Thread.h"
using namespace MT;



/**********************************************************************************
 *
 *
 *
 */
class CCoff {
public:
	LPBYTE					m_pData;
	DWORD					m_dwSize;
	IMAGE_FILE_HEADER*		pImageFileHaader;
	IMAGE_SECTION_HEADER*	pImageSectionHeader;


	void	Bind( LPBYTE lpByte, DWORD dwSize ){
		m_pData	= lpByte;

		pImageFileHaader		= (IMAGE_FILE_HEADER*)m_pData;
		pImageSectionHeader		= (IMAGE_SECTION_HEADER*)((BYTE*)pImageFileHaader + sizeof(IMAGE_FILE_HEADER));
	
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CCoffForm : public CForm {
protected:
	enum {
		ID_MENU			= 100	,
		ID_FILE					,
		ID_FILE_OPEN			,
		ID_FILE_EXIT			,

		IDC				= 1000	,
		IDC_LISTVIEW			,
	};

	CListView			m_wndList;

	TCommandHandler<CCoffForm>	cmd;

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
		int	w	= lpCreateStruct->cx;
		int	h	= lpCreateStruct->cy;
		m_wndList.Create(this, 0, 0, w, h, IDC_LISTVIEW);

		InitMenu();


		cmd.Initialize(this);
		cmd.Register(ID_FILE_OPEN, &CCoffForm::OnOpen);
		cmd.Register(ID_FILE_EXIT, &CCoffForm::OnClose);

		return TRUE;
	}

	void	OnSize(UINT, int cx, int cy){
		if( cx < 0 || cy < 0 )
			return;

		m_wndList.MoveWindow(0,0,cx,cy);
	}

public:
	void	OnCommand(UINT nID, HWND hWndCtrl, UINT nCodeNotify){
		cmd.Dispach(nID, nCodeNotify);
	}

	void	OnOpen( void ){
		CObjOpenDlg	dlg;
		dlg.DoModal( m_hWnd );
	}

	void	OnClose( void ){
		DestroyWindow();
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CCoffUIThread : public IRunnable {
public:
	void	run( void ){
		CCoffForm	Form;
		if( Form.Create(_T("Coff"), 0, 0, 100, 100, 300, 300) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}	
	}
};
