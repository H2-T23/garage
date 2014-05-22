// DLLInjection.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "DLLInjection.h"

#include "UseToolkit.h"

#include "SystemInfo.h"
//#pragma comment(lib,"SystemInfo.lib")

/**********************************************************************************
 *
 *
 *
 */
class CDllInjection {
private:
	CDllInjection( void ){}

public:
	HANDLE		m_hProcess;

//	CDllInjection( HANDLE hProcess ) : m_hProcess(hProcess) {
//	}

	CDllInjection( DWORD dwPID ) : m_hProcess(NULL) {
		Open( dwPID );
	}

	~CDllInjection( void ){
		if( m_hProcess ){
			::CloseHandle( m_hProcess );
		}
	}

	HANDLE		Open( DWORD dwPID ){
		return m_hProcess	= ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	}

	void		Inject( const char* pDllName ){
		HMODULE					hKernel32		= ::GetModuleHandleA("Kernel32");
		LPTHREAD_START_ROUTINE	pLoadLibrary	= (LPTHREAD_START_ROUTINE)::GetProcAddress(hKernel32, "LoadLibraryA");
		if( pLoadLibrary )
		{
			DWORD	dwSize	= strlen(pDllName) + 1;
			LPVOID	pMem	= ::VirtualAllocEx( m_hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE );
			if( pMem )
			{
				if( ::WriteProcessMemory( m_hProcess, pMem, (LPCVOID)pDllName, dwSize, NULL ) )
				{
					HANDLE	hThread	= ::CreateRemoteThread( m_hProcess, NULL, 0, pLoadLibrary, pMem, 0, NULL );
					if( hThread )
					{
						::WaitForSingleObject( hThread, INFINITE );
						::CloseHandle( hThread );
					}
				}

				::VirtualFreeEx( m_hProcess, pMem, dwSize, MEM_RELEASE );
			}
		}
	}

	void		Eject( DWORD dwModuleAddress ){
		HMODULE					hKernel32		= ::GetModuleHandleA("Kernel32");
		LPTHREAD_START_ROUTINE	pFreeLibrary	= (LPTHREAD_START_ROUTINE)::GetProcAddress(hKernel32, "FreeLibrary");
		if( pFreeLibrary )
		{
			DWORD	dwSize	= sizeof(DWORD);
			LPVOID	pMem	= ::VirtualAllocEx( m_hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE );
			if( pMem )
			{
				if( ::WriteProcessMemory( m_hProcess, pMem, (LPCVOID)&dwModuleAddress, dwSize, NULL ) )
				{
					HANDLE	hThread	= ::CreateRemoteThread( m_hProcess, NULL, 0, pFreeLibrary, pMem, 0, NULL );
				//	HANDLE	hThread	= ::CreateRemoteThread( m_hProcess, NULL, 0, pFreeLibrary, (LPVOID)&dwModuleAddress, 0, NULL );
					if( hThread )
					{
						::WaitForSingleObject( hThread, INFINITE );
						::CloseHandle( hThread );
						hThread	= NULL;
					}
				}

				::VirtualFreeEx( m_hProcess, pMem, dwSize, MEM_RELEASE );
				pMem	= NULL;
				dwSize	= 0;
			}
		}
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CDllOpenDlg	: public CFileOpenDlg {
protected:
	virtual LPCTSTR			Filter( void ) const	{ return _T("Dll(*.dll)\0*.dll\0All files(*.*)\0*.*\0\0");	}
};
/**********************************************************************************
 *
 *
 *
 */
template<class CLASS,class SUPERCLASS>
class ICommandHandler : public SUPERCLASS {
protected:
	TCommandHandler<CLASS>	cmd;
public:
	void	OnCommand( UINT uID, HWND, UINT nCodeNotify ){
		cmd.Dispach(uID,nCodeNotify);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CInputForm : public ICommandHandler<CInputForm,CForm> {
public:
	enum {
		IDC					= 1000	,
		IDC_EDIT_PID				,
		IDC_EDIT_BASEADDR			,
		IDC_EDIT_PATH				,
		IDC_BUTTON_SELECT			,
		IDC_BUTTON_I				,
		IDC_BUTTON_E				,
	};

protected:
	CEdit				m_edtPID;
	CEdit				m_edtBaseAddr;
	CEdit				m_edtPath;
	CButton				m_btnSelect;
	CButton				m_btnI, m_btnE;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( m_edtPID.Create(this, 10, 10, 80, 20, IDC_EDIT_PID) ){
			TString	str;
			str.Format(_T("%X"), ::GetCurrentProcessId());

			m_edtPID.SetText( str );
		}

		if( m_edtBaseAddr.Create(this, 100, 10, 80, 20, IDC_EDIT_BASEADDR) ){
		}

		if( m_edtPath.Create(this, 10, 30, 250, 20, IDC_EDIT_PATH) ){
			m_edtPath.ModifyStyle( 0, ES_AUTOVSCROLL, TRUE );
			TCHAR	buf[MAX_PATH];
			ZeroMemory(buf, sizeof(buf));
			::GetSystemDirectory(buf, MAX_PATH);
			m_edtPath.SetWindowText(buf);
		}

		if( m_btnSelect.Create(this, 260, 30, 20, 20, IDC_BUTTON_SELECT, _T("...\0")) ){
		}

		if( m_btnI.Create(this, 10, 50, 80, 20, IDC_BUTTON_I, _T("Inject")) ){
		}

		if( m_btnE.Create(this, 100, 50, 80, 20, IDC_BUTTON_E, _T("Eject")) ){
		}

		cmd.Initialize(this);
		cmd.Register(IDC_BUTTON_SELECT	, &CInputForm::OnBtnSelect	);
		cmd.Register(IDC_BUTTON_I		, &CInputForm::OnBtnI		);
		cmd.Register(IDC_BUTTON_E		, &CInputForm::OnBtnE		);

		VERSION	= 0x00BB;
		CSystemInfo::Instance().SetVersion(VERSION);

		SetFontChildren();
		return TRUE;
	}

public:
	void	OnBtnSelect( void ){
		CDllOpenDlg		dlg;
		if( dlg.DoModal(m_hWnd) ){
			TString	str;
			dlg.GetFileName( str );

			m_edtPath.SetWindowText( str );
		}
	}
	
	void	OnBtnI( void ){
		TString		str, strPID, strPath;
		m_edtPath.GetText( strPath );
		m_edtPID.GetText( strPID );

		long	lPID	= _tcstol( (LPTSTR)strPID, NULL, 16 );

		str.Format(_T("%s : %s\0"), (LPTSTR)strPID, (LPTSTR)strPath);
		MessageBox(m_hWnd, str, _T("TARGET"), MB_OK);

		char	buf[MAX_PATH];
		TString::W2A( strPath, strPath.length()+1, buf, MAX_PATH );
		CDllInjection		inj( lPID );
		inj.Inject( buf );
	}
	
	void	OnBtnE( void ){
		TString		str, strPID, strBaseAddr;
		m_edtPID		.GetText( strPID );
		m_edtBaseAddr	.GetText( strBaseAddr );

		long	lPID	= _tcstol( (LPTSTR)strPID, NULL, 16 );
		long	lAddr	= _tcstol( (LPTSTR)strBaseAddr, NULL, 16 );

		str.Format(_T("%s : %s\0"), (LPTSTR)strPID, (LPTSTR)strBaseAddr);
		MessageBox(m_hWnd, str, _T("TARGET"), MB_OK);

		CDllInjection		inj( lPID );
		inj.Eject( lAddr );
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CInputFormThread : public MT::IRunnable {
public:
	void		run( void ){
		CInputForm	Form;
		if( Form.Create(_T(""), 0, 0, 700, 100, 300, 120) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};
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


	MT::CThread*	pThread = new MT::CThread( new CInputFormThread() );
	pThread->Start();
	pThread->Wait();
}

