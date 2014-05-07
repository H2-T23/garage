// DumpView.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "DumpView.h"

#include "UseToolkit.h"
#include "FileOpenDlg.h"
#include "Thread.h"

using namespace MT;


#define	MAX_ITEM		0xFFFFFF

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, class BASE>
class ICommandHandler : public BASE {
protected:
	TCommandHandler<CLASS>	cmd;

public:
	void	OnCommand(UINT nID, HWND hWndCtrl, UINT nCodeNotify){
		cmd.Dispach(nID, nCodeNotify);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CDumpFileThread : public IRunnable {
protected:
	TString			m_strFilename;
	CListView&		m_wndList;

	typedef std::vector<BYTE>	BUFFER;

	DWORD	LoadFileData( LPTSTR lpFilename, BUFFER& buf ){
		DWORD	dwRet	= 0;
		CReadOnlyFile	file(lpFilename);
		if( file.IsOpen() ){
			DWORD dwSize = file.GetFileSize();
			if( dwSize > 0 )
			{
				buf.resize( dwSize );
				if( file.Read(&buf.at(0), dwSize) == dwSize ){
					dwRet	= dwSize;
				}
			}
			file.Close();
			return dwRet;
		}
		return dwRet;
	}

	void	DumpFile( LPTSTR lpFilename ){
		BUFFER	buf;
		if( LoadFileData(lpFilename, buf) )
		{
			m_wndList.DeleteAllItems();
			
			TString	str;	

			int	item		= 0;
			unsigned idx	= 0;
			unsigned siz	= buf.size();

			while( idx < siz )
			{
				int	loop	= 16;
				if( int(siz - idx) < 16 ){
					loop	= int(siz - idx);
				}

				str.Format(_T("%08X"), idx);
				m_wndList.InsertItem(item, (LPTSTR)str);

				for( int i=0; i<loop; i++ ){
					str.Format(_T("%02X"), buf.at(idx+i));
					m_wndList.SetItem(item, i+1, (LPTSTR)str);
				}

				str.clear();
				for( int i=0; i<loop; i++ ){
					if( ::isgraph(int(buf.at(idx+i))) )
						str.AppendFormat(_T("%c"), buf.at(idx+i));
					else
						str.AppendFormat(_T(" "));
				}
				m_wndList.SetItem(item, 17, (LPTSTR)str);
			
				idx += 16;
				item += 1;
			}
		}
	}

public:
	CDumpFileThread( LPTSTR lpFilename, CListView& listview ) 
	: m_strFilename(lpFilename), m_wndList(listview) {
	}

	void	run( void ){
		DumpFile( m_strFilename );
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public ICommandHandler<CMainForm, CForm> {
public:
	enum {
		ID_MENU				= 100	,
		ID_FILE						,
		ID_FILE_OPEN				,
		ID_FILE_SAVE				,
		ID_FILE_EXIT				,
		ID_HELP						,
		ID_HELP_ABOUT				,

		IDC					= 1000	,
		IDC_LIST_VIEW				,
	};
protected:
	CListView			m_wndList;

	void		InitMenu( void ){
		CPopupMenu	menuFile;
		menuFile.Create();
		menuFile.Insert(_T("Open"), ID_FILE_OPEN);
		menuFile.Insert(_T("Save"), ID_FILE_SAVE);
		menuFile.Insert(NULL, NULL);
		menuFile.Insert(_T("Exit"), ID_FILE_EXIT);

		CPopupMenu	menuHelp;
		menuHelp.Create();
		menuHelp.Insert(_T("About")	, ID_HELP_ABOUT);
		
		CMenu	menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File")	, ID_FILE	, (HMENU)menuFile);
		menuMain.Insert(_T("Help")	, ID_HELP	, (HMENU)menuHelp);
		
		SetMenu(m_hWnd, (HMENU)menuMain);
	}

	BOOL		OnCreate( LPCREATESTRUCT lpCreateStruct ){
		InitMenu();

		if( m_wndList.Create(this,0,0,0,0,IDC_LIST_VIEW) ){
			m_wndList.SetExtendedLitViewStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

			int	item	= 0;
			m_wndList.InsertColumn(item,_T("address"));
			for( int i=0; i<16; i++ ){
				item++;
				m_wndList.InsertColumn(item,_T(""),30);
			}
			item++;
			m_wndList.InsertColumn(item,_T("text"));
		}

		cmd.Initialize(this);
		cmd.Register(ID_FILE_OPEN, &CMainForm::OnFileOpen);
		cmd.Register(ID_FILE_SAVE, &CMainForm::OnFileSave);
		cmd.Register(ID_FILE_EXIT, &CMainForm::OnFileExit);
		cmd.Register(ID_HELP_ABOUT, &CMainForm::OnHelpAbout);
		return TRUE;
	}

	void	OnSize(UINT state, int cx, int cy){
		if( cx < 0 || cy < 0 )
			return;

		m_wndList.MoveWindow(0,0,cx,cy);
	}

	void	OnFileOpen( void ){
		CFileOpenDlg	dlg;
		if( dlg.DoModal(m_hWnd) ){
			TString	str;
			dlg.GetFileName(str);

			SetWindowText(str);

			/*CThread* pThread = */new CThread( new CDumpFileThread(str, m_wndList), true );
			//if( pThread ){
			//	pThread->Start();
			//}
		}
	}

	void	OnFileSave( void ){
		CFileSaveDlg	dlg;
		if( dlg.DoModal(m_hWnd) ){
			TString	str;
			dlg.GetFileName(str);

			SaveDumpText(str);
		}
	}

	void	OnFileExit( void ){
		DestroyWindow();
	}

	void	OnHelpAbout( void ){
		MessageBox(m_hWnd,_T("DumpView"),_T("About"),MB_OK);
	}

	void	SaveDumpText( LPTSTR lpFilename ){
		CTxtFile	file(lpFilename);
		if( file.IsOpen() )
		{
			TCHAR	buf[256];
			TString	str;
			int	item	= 0;
			int	cnt		= m_wndList.GetItemCount();
			for( ; item<cnt; item++ )
			{
				m_wndList.GetItemText(item, 0, buf, _countof(buf));
				str.assign(buf);
				str.Append(_T("\t"));

				for( int i=0; i<16; i++ ){
					m_wndList.GetItemText(item, i+1, buf, _countof(buf));
					str.AppendFormat(_T("%s "), buf);
				}
				str.Append(_T("\t"));

				m_wndList.GetItemText(item, 17, buf, _countof(buf));
				str.AppendFormat(_T("%s\n\0"), buf);

				file.WriteString(str);
			}

			file.Close();
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


	CMainForm	Form;
	if( Form.Create(_T("DumpView"),0,0,100,100,600,300) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		Form.MessageLoop();
	}
	return 0;
}

