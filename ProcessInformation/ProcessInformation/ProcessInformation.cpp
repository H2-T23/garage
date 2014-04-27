// ProcessInformation.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ProcessInformation.h"

#include "UseToolkit.h"

#include "Thread.h"
using namespace MT;

#include <tlhelp32.h>

#include "PEFile.h"
#include "CoffFile.h"

//#define	PEFILE
#ifdef PEFILE
#include "LoadFile.h"
#include "PEFile.h"
#endif

/**********************************************************************************
 *
 *
 *
 */
typedef std::vector<PROCESSENTRY32>		VecProcessEntry;
typedef std::auto_ptr<VecProcessEntry>	PVecProcessEntry;

PVecProcessEntry	CreateProcessShapShot(void){
	PVecProcessEntry p( new VecProcessEntry() );

	HANDLE	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapShot != INVALID_HANDLE_VALUE){
		PROCESSENTRY32 entry = { sizeof(PROCESSENTRY32) };
		BOOL bProcess = Process32First(hSnapShot, &entry);
		for( ; bProcess; bProcess=Process32Next(hSnapShot,&entry) ){
			p->push_back( entry );
		}
		CloseHandle( hSnapShot );
	}
	return(p);
}
/**********************************************************************************
 *
 *
 *
 */
typedef std::vector<MODULEENTRY32>		VecModuleEntry;
typedef std::auto_ptr<VecModuleEntry>	PVecModuleEntry;

PVecModuleEntry		CreateModuleSnapShot(DWORD dwPID){
	PVecModuleEntry	p( new VecModuleEntry() );

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPID);
	if(hSnapShot != INVALID_HANDLE_VALUE){
		MODULEENTRY32 entry = { sizeof(MODULEENTRY32) };
		BOOL bModule = Module32First(hSnapShot, &entry);
		for( ; bModule; bModule=Module32Next(hSnapShot,&entry) ){
			p->push_back( entry );
		}
		CloseHandle(hSnapShot);
	}
	return(p);
}
/**********************************************************************************
 *
 *
 *
 */
typedef std::vector<THREADENTRY32>		VecThreadEntry;
typedef std::auto_ptr<VecThreadEntry>	PVecThreadEntry;

PVecThreadEntry		CreateThreadSnapShot(DWORD dwPID){
	PVecThreadEntry	p( new VecThreadEntry() );

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if(hSnapShot != INVALID_HANDLE_VALUE){
		THREADENTRY32 entry = { sizeof(THREADENTRY32) };
		BOOL bModule = Thread32First(hSnapShot, &entry);
		for( ; bModule; bModule=Thread32Next(hSnapShot,&entry) ){
			if( dwPID == entry.th32OwnerProcessID ){
				p->push_back( entry );
			}
		}
		CloseHandle(hSnapShot);
	}
	return(p);
}
/**********************************************************************************
 *
 *
 *
 */
class CModuleForm : public CForm {
public:
	CModuleForm( void ) : CForm(), m_dwPID(0) {}
	CModuleForm( DWORD dwPID ) : CForm(), m_dwPID(dwPID) {}

protected:
	DWORD				m_dwPID;
	CListView			m_wndList;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( m_wndList.Create(this, 0, 0, lpCreateStruct->cx, lpCreateStruct->cy, 1001) ){
			m_wndList.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT );
			m_wndList.InsertColumn(0, _T("Module"));
			m_wndList.InsertColumn(1, _T("Base Address"));
			m_wndList.InsertColumn(2, _T("Size of Image"));
		}

		if( m_dwPID )
		{
			TCHAR	szBuf[MAX_PATH];
			CProcessInfo	psinfo(m_dwPID);
			psinfo.GetProcessImageFileNameEx(szBuf, _countof(szBuf));

			if( psinfo.IsValid() )
			{
				TString		str;
				TCHAR		szName[256];
				DWORD		dwSize	= 0;
				HMODULE		modules[100];
				::EnumProcessModules( (HANDLE)psinfo, modules, sizeof(modules), &dwSize );

				MODULEINFO	mi	= {0};

				for( int i=0; i<(dwSize/sizeof(HMODULE)); i++ ){
					::GetModuleBaseName( (HANDLE)psinfo, modules[ i ], szName, sizeof(szName) );
					::GetModuleInformation( (HANDLE)psinfo, modules[ i ], &mi, sizeof(mi) );

					str.Format(_T("%s\0"), szName );
					m_wndList.InsertItem( i , (LPTSTR)str );

					str.Format(_T("0x%X\0"), mi.lpBaseOfDll );
					m_wndList.SetItem( i, 1, (LPTSTR)str );

					str.Format(_T("0x%X\0"), mi.SizeOfImage );
					m_wndList.SetItem( i, 2, (LPTSTR)str );
				}
			}
		}

		return TRUE;
	}

	void	OnSize(UINT, int cx, int cy){
		if( cx < 0 || cy < 0 )
			return;

		m_wndList.MoveWindow(0, 0, cx, cy);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CModuleUIThread : public IRunnable {
public:
	DWORD	m_dwPID;

	CModuleUIThread( DWORD dwPID ) : m_dwPID(dwPID) {
	}

	void		run( void ){
		CModuleForm	Form(m_dwPID);
		if( Form.Create(_T("Module"), 0, 0, 100, 100, 300, 300) ){
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
enum {
	ID_MENU				= 100	,
	ID_FILE						,
	ID_FILE_SNAPSHOT			,
	ID_FILE_PEFILE				,
	ID_FILE_COFF				,
	ID_FILE_EXIT				,
	ID_HELP						,
	ID_HELP_ABOUT				,

	IDC					= 1000	,
	IDC_LIST_PROCESS			,
	IDC_LIST_MODULE				,
	IDC_LIST_THREAD				,

	WM_USER_MESSAGE				= WM_USER	,
	WM_SNAPSHOT_MODULE						,
	WM_SNAPSHOT_THREAD						,
};
/**********************************************************************************
 *
 *
 *
 */
class CModuleAndThreadPanel : public TSplitter<CPanel, CHorizontalSplitter> {
protected:
	CListView			m_wndListModule;
	CListView			m_wndListThread;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( m_wndListModule.Create(this,0,0,0,0,IDC_LIST_MODULE) ){
			m_wndListModule.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT );
			m_wndListModule.InsertColumn(0,_T("Module Name"));
			m_wndListModule.InsertColumn(1,_T("Executable"));
			m_wndListModule.InsertColumn(2,_T("Process ID"));
			m_wndListModule.InsertColumn(3,_T("Module ID"));
			m_wndListModule.InsertColumn(4,_T("Ref Count(G)"));
			m_wndListModule.InsertColumn(5,_T("Ref Count(P)"));
			m_wndListModule.InsertColumn(6,_T("Base Address"));
			m_wndListModule.InsertColumn(7,_T("Base Size"));
			m_wndListModule.InsertColumn(8,_T("Handle"));
		}

		if( m_wndListThread.Create(this,0,0,0,0,IDC_LIST_THREAD) ){
			m_wndListThread.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT );
			m_wndListThread.InsertColumn(0,_T("Thread ID"));
			m_wndListThread.InsertColumn(1,_T("Owner Process ID"));
			m_wndListThread.InsertColumn(2,_T("Usage"));
			m_wndListThread.InsertColumn(3,_T("Base Priority"));
			m_wndListThread.InsertColumn(4,_T("Delta Priority"));
			m_wndListThread.InsertColumn(5,_T("Flags"));
		}

		m_Splitter.Append( &m_wndListModule );
		m_Splitter.Append( &m_wndListThread );
		m_Splitter.Bind( this );
		return TRUE;
	}

public:
	void		SnapshotModule( DWORD dwPID ){
		m_wndListModule.DeleteAllItems();

		PVecModuleEntry	pModuleList = CreateModuleSnapShot( dwPID );
		int siz	= pModuleList->size();
		if( siz > 0 )
		{
			TString	str;
			for( int i=0; i<siz; i++ )
			{
				str.Format(_T("%s\0"), (*pModuleList)[ i ].szModule );
				m_wndListModule.InsertItem( i, (LPTSTR)str );

				str.Format(_T("%s\0"), (*pModuleList)[ i ].szExePath );
				m_wndListModule.SetItem( i, 1,  (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pModuleList)[ i ].th32ProcessID );
				m_wndListModule.SetItem( i, 2,  (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pModuleList)[ i ].th32ModuleID );
				m_wndListModule.SetItem( i, 3,  (LPTSTR)str );

				str.Format(_T("%d\0"), (*pModuleList)[ i ].GlblcntUsage );
				m_wndListModule.SetItem( i, 4,  (LPTSTR)str );

				str.Format(_T("%d\0"), (*pModuleList)[ i ].ProccntUsage );
				m_wndListModule.SetItem( i, 5,  (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pModuleList)[ i ].modBaseAddr );
				m_wndListModule.SetItem( i, 6,  (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pModuleList)[ i ].modBaseSize );
				m_wndListModule.SetItem( i, 7,  (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pModuleList)[ i ].hModule );
				m_wndListModule.SetItem( i, 8,  (LPTSTR)str );
			}
		}
	}

	void		SnapshotThread( DWORD dwPID ){
		m_wndListThread.DeleteAllItems();

		PVecThreadEntry pThreadList	= CreateThreadSnapShot( dwPID );
		int	siz	= pThreadList->size();
		if( siz > 0 )
		{
			TString	str;
			for( int i=0; i<siz; i++ )
			{
				str.Format(_T("0x%X\0"), (*pThreadList)[ i ].th32ThreadID );
				m_wndListThread.InsertItem( i, (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pThreadList)[ i ].th32OwnerProcessID );
				m_wndListThread.SetItem( i, 1, (LPTSTR)str );

				str.Format(_T("%d\0"), (*pThreadList)[ i ].cntUsage );
				m_wndListThread.SetItem( i, 2, (LPTSTR)str );

				str.Format(_T("%d\0"), (*pThreadList)[ i ].tpBasePri );
				m_wndListThread.SetItem( i, 3, (LPTSTR)str );

				str.Format(_T("%d\0"), (*pThreadList)[ i ].tpDeltaPri );
				m_wndListThread.SetItem( i, 4, (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pThreadList)[ i ].dwFlags );
				m_wndListThread.SetItem( i, 5, (LPTSTR)str );
			}
		}
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CProcessPanel : public CPanel {
protected:
	CListView			m_wndListProcess;

	TNotifyHandler<CProcessPanel>	notify;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( m_wndListProcess.Create(this,0,0,0,0,IDC_LIST_PROCESS) ){
			m_wndListProcess.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT );
			m_wndListProcess.InsertColumn(0,_T("Process Name"),200);
			m_wndListProcess.InsertColumn(1,_T("Process ID"));
			m_wndListProcess.InsertColumn(2,_T("Thread Count"));
			m_wndListProcess.InsertColumn(3,_T("Parent Process ID"));
			m_wndListProcess.InsertColumn(4,_T("Priority base"));
			m_wndListProcess.InsertColumn(5,_T("Usage"));
			m_wndListProcess.InsertColumn(6,_T("Flags"));
		}

		notify.Initialize(this);
		notify.Register(NM_DBLCLK, IDC_LIST_PROCESS, &CProcessPanel::OnDblClkListProcess);

		return TRUE;
	}

	void	OnSize(UINT state, int cx, int cy){
		if( cx > 0 && cy > 0 ){
			m_wndListProcess.MoveWindow(0,0,cx,cy);
		}
	}

public:
	void	OnNotify(WPARAM wParam, LPARAM lParam){
		notify.Dispach(wParam, lParam);
	}

	void	OnDblClkListProcess( LPNMHDR lpNmhdr ){
		LPNMLISTVIEW lpListView = (LPNMLISTVIEW)lpNmhdr;
		if( lpListView )
		{
			LV_HITTESTINFO	hit	= {0};
			DWORD	dwPos	= ::GetMessagePos();
			hit.pt.x	= GET_X_LPARAM(dwPos);
			hit.pt.y	= GET_Y_LPARAM(dwPos);
			::MapWindowPoints(HWND_DESKTOP, lpNmhdr->hwndFrom, &hit.pt, 1 );
			
			m_wndListProcess.HitTest( &hit );
			if( hit.flags & LVHT_ONITEM )
			{
				LVITEM	item = {0};
				item.mask	= LVIF_PARAM;
				item.iItem	= hit.iItem;
				m_wndListProcess.GetItem( item );
				TRACE(_T("PID: 0x%X\n"), (DWORD)item.lParam );

				::PostMessage(GetParent(), WM_SNAPSHOT_MODULE, 0, item.lParam );
				::PostMessage(GetParent(), WM_SNAPSHOT_THREAD, 0, item.lParam );

				CThread* pThread = new CThread( new CModuleUIThread(item.lParam) );
				if( pThread ){
					pThread->Start();
				}

				pThread	= new CThread( new CPEFileUIThread(item.lParam) );
				if( pThread ){
					pThread->Start();
				}

			}
		}
	}

public:
	void	SnapshotProcess( void ){
		PVecProcessEntry	pProcessList = CreateProcessShapShot();
		int siz  = pProcessList->size();
		if( siz > 0 )
		{
			m_wndListProcess.DeleteAllItems();

			TString	str;
			for( int i=0; i<siz; i++ )
			{
				DWORD	dwPID	= (*pProcessList)[ i ].th32ProcessID;

				str.Format(_T("%s\0"), (*pProcessList)[ i ].szExeFile );
				m_wndListProcess.InsertItem( i, (LPTSTR)str );

				str.Format(_T("0x%X\0"), dwPID );
				m_wndListProcess.SetItem( i, 1, (LPTSTR)str );

				str.Format(_T("%d\0"), (*pProcessList)[ i ].cntThreads );
				m_wndListProcess.SetItem( i, 2, (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pProcessList)[ i ].th32ParentProcessID );
				m_wndListProcess.SetItem( i, 3, (LPTSTR)str );

				str.Format(_T("%d\0"), (*pProcessList)[ i ].pcPriClassBase );
				m_wndListProcess.SetItem( i, 4, (LPTSTR)str );

				str.Format(_T("%d\0"), (*pProcessList)[ i ].cntUsage );
				m_wndListProcess.SetItem( i, 5, (LPTSTR)str );

				str.Format(_T("0x%X\0"), (*pProcessList)[ i ].dwFlags );
				m_wndListProcess.SetItem( i, 6, (LPTSTR)str );

				LVITEM	item = {0};
				item.mask	= LVIF_PARAM;
				item.lParam	= dwPID;
				item.iItem	= i;
				m_wndListProcess.SetItem( item );
			}
		}
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public TSplitter<CForm, CVerticalSplitter> {
public:
protected:
	CProcessPanel				m_wndProcess;
	CModuleAndThreadPanel		m_wndModuleAndThread;
	
	TCommandHandler<CMainForm>	cmd;
	TMessageHandler<CMainForm>	msg;

	void	InitMenu( void ){
		CPopupMenu		menuFile;
		menuFile.Create();
		menuFile.Insert(_T("Snapshot"), ID_FILE_SNAPSHOT);
		menuFile.Insert(_T("PEFile"), ID_FILE_PEFILE);
		menuFile.Insert(_T("Coff"), ID_FILE_COFF);
		menuFile.Insert( NULL, 0 );
		menuFile.Insert(_T("Exit"), ID_FILE_EXIT);

		CPopupMenu		menuHelp;
		menuHelp.Create();
		menuHelp.Insert(_T("About"), ID_HELP_ABOUT);

		CMenu			menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File"), ID_FILE, (HMENU)menuFile);
		menuMain.Insert(_T("Help"), ID_HELP, (HMENU)menuHelp);

		SetMenu(m_hWnd, (HMENU)menuMain);
	}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		InitMenu();

		m_wndProcess.Create(this,0,0,0,0);
		m_wndModuleAndThread.Create(this,0,0,0,0);

		cmd.Initialize(this);
		cmd.Register(ID_FILE_SNAPSHOT, &CMainForm::OnSnapshot);
		cmd.Register(ID_FILE_PEFILE, &CMainForm::OnPEFile);
		cmd.Register(ID_FILE_COFF, &CMainForm::OnCoff);

		msg.Initialize(this);
		msg.Register(WM_SNAPSHOT_MODULE, &CMainForm::OnSnapshotModule);
		msg.Register(WM_SNAPSHOT_THREAD, &CMainForm::OnSnapshotThread);

		m_Splitter.Append( &m_wndProcess );
		m_Splitter.Append( &m_wndModuleAndThread );
		m_Splitter.Bind( this );

		SetFontChildren();
		return TRUE;
	}

public:
	void	OnCommand(UINT uID, HWND hWndCtrl, UINT nCodeNotify){
		cmd.Dispach(uID, nCodeNotify);
	}

	void	OnSnapshot( void ){
		m_wndProcess.SnapshotProcess();
	}

	void	OnPEFile( void ){
		CThread* pThread = new CThread( new CPEFileUIThread() );
		if( pThread ){
			pThread->Start();
		}		
	}

	void	OnCoff( void ){
		CThread* pThread = new CThread( new CCoffUIThread() );
		if( pThread ){
			pThread->Start();
		}
	}

public:
	void	OnWndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam ){
		msg.Dispach(uMsg, wParam, lParam);
	}

	LRESULT	OnSnapshotModule( WPARAM wParam, LPARAM lParam ){
		m_wndModuleAndThread.SnapshotModule( (DWORD)lParam );
		return 0;
	}

	LRESULT	OnSnapshotThread( WPARAM wParam, LPARAM lParam ){
		m_wndModuleAndThread.SnapshotThread( (DWORD)lParam );
		return 0;
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

	CMainForm	Form;
	if( Form.Create(_T("ProcessInformation"), 0, 0, 100, 100, 600, 600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
