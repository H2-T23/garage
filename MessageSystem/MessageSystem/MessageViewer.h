#pragma once

#include "MessagePool.h"

#include "Form.h"
#include "ComponentWnd.h"
#include "Handler.h"

#include "Thread.h"

#include "File.h"

using namespace MT;

class CMessageViewerForm : public CForm {
protected:
	enum{
		ID_MENU				= 100	,
		ID_FILE						,
		ID_FILE_SAVE				,
		ID_FILE_EXIT				,
		ID_EDIT						,
		ID_EDIT_CLEAR				,
		ID_TIMER					,
		ID_TIMER_START				,
		ID_TIMER_STOP				,
		ID_HELP						,
		ID_HELP_ABOUT				,

		IDC					= 1001	,
		IDC_LISTVIEW				,

		ID_TIMER_EVENT		= 2000	,
	};

	bool				m_bTimerActive;

	CMenu				m_menuMain;

	CListView			m_wndList;

	TCommandHandler<CMessageViewerForm>		cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){

		if( m_menuMain.Create() )
		{
			CPopupMenu			m_menuFile;
			m_menuFile.Create();
			m_menuFile.Insert(_T("Save(&S)"), ID_FILE_SAVE);
			m_menuFile.Insert(NULL, 0);
			m_menuFile.Insert(_T("Exit(&X)"), ID_FILE_EXIT);

			CPopupMenu		m_menuEdit;
			m_menuEdit.Create();
			m_menuEdit.Insert(_T("Clear(&C)"), ID_EDIT_CLEAR);

			CPopupMenu		m_menuTimer;
			m_menuTimer.Create();
			m_menuTimer.Insert(_T("Start")	, ID_TIMER_START);
			m_menuTimer.Insert(_T("Stop")	, ID_TIMER_STOP);

			CPopupMenu			m_menuHelp;
			m_menuHelp.Create();
			m_menuHelp.Insert(_T("About(&A)"), ID_HELP_ABOUT);

			m_menuMain.Insert(_T("File(&F)")	, ID_FILE, (HMENU)m_menuFile);
			m_menuMain.Insert(_T("Edit(&E)")	, ID_EDIT, (HMENU)m_menuEdit);
			m_menuMain.Insert(_T("Timer(&C)")	, ID_TIMER, (HMENU)m_menuTimer);
			m_menuMain.Insert(_T("Help(&H)")	, ID_HELP, (HMENU)m_menuHelp);

			SetMenu(m_hWnd, (HMENU)m_menuMain);
		}

		if( m_wndList.Create(this, 0, 0, 0, 0, IDC_LISTVIEW) ){
			m_wndList.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT );
			m_wndList.InsertColumn( 0, _T("ID") );
			m_wndList.InsertColumn( 1, _T("Time") );
			m_wndList.InsertColumn( 2, _T("Subject") );
		}


		cmd.Initialize(this);
		cmd.Register(ID_FILE_SAVE, &CMessageViewerForm::OnSave);
		cmd.Register(ID_FILE_EXIT, &CMessageViewerForm::OnExit);
		cmd.Register(ID_TIMER_START, &CMessageViewerForm::OnTimerStart);
		cmd.Register(ID_TIMER_STOP, &CMessageViewerForm::OnTimerStop);
		cmd.Register(ID_EDIT_CLEAR, &CMessageViewerForm::OnClear);

		OnTimerStart();

		SetFontChildren();
		return TRUE;
	}

	void	OnSize( UINT, int cx, int cy ){
		if( cx > 0 && cy > 0 )
		{
			m_wndList.MoveWindow( 0, 0, cx, cy );
		}
	}

	void	OnTimer( UINT nID ){
		if( nID == ID_TIMER_EVENT )
		{
			CMessagePool::ARRAY	ary(0);
			CMessagePool::Instance().GetSnapshot( ary );

			TString	str;

			int idx = m_wndList.GetItemCount();

			int cnt = ary.size();
			for( int i=0; i<cnt; i++, idx++ )
			{
				str.Format(_T("%d\0"), ary[ i ].ID);
				m_wndList.InsertItem( idx, (LPTSTR)str );

				struct tm* pTm	= localtime( &ary[ i ].tm );
				if( pTm )
				{
					str.Format(_T("%02d/%02d/%02d %02d:%02d:%02d\0")
						,	pTm->tm_year + 1900, pTm->tm_mday + 1, pTm->tm_mday
						,	pTm->tm_hour, pTm->tm_min, pTm->tm_sec	
						);
					m_wndList.SetItem( idx, 1, (LPTSTR)str );
				}
				else
				{
					m_wndList.SetItem( idx, 1, _T("******") );
				}

				str.resize( 256 );
				TString::A2W( ary[ i ].subject, 256, &str.at( 0 ), 256 );
				m_wndList.SetItem( idx, 2, (LPTSTR)str );
			}

			if( cnt > 0 )
			{
				m_wndList.EnsureVisible( idx - 1 );
			}
		}
	}

	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);
	}

	void	OnSave( void ){
		bool	bActive	= m_bTimerActive;
		if( bActive ){
			OnTimerStop();
		}

		CTxtFile	file(_T("msg.txt"));
		if( file.IsOpen() )
		{
			TCHAR	buf[256];
			TString	str;
			int idx = m_wndList.GetItemCount();
			for( int i=0; i<idx; i++ )
			{
				m_wndList.GetItemText( i, 0, buf, _countof(buf) );
				str	= buf;

				m_wndList.GetItemText( i, 1, buf, _countof(buf) );
				str.AppendFormat(_T(":%s"), buf);

				m_wndList.GetItemText( i, 2, buf, _countof(buf) );
				str.AppendFormat(_T(":%s\0"), buf);

				int	nLen	= str.Length();
				if( nLen > 0 )
				{
					if( str.at( nLen-1 ) != _T('\n') ){
						str.Append( _T("\n") );
					}

					file.WriteString( str );
				}
			}

			file.Close();
		}

		if( bActive ){
			OnTimerStart();
		}
	}

	void	OnExit( void ){
		DestroyWindow();
	}

	void	OnClear( void ){
		bool	bActive	= m_bTimerActive;
		if( bActive ){
			OnTimerStop();
		}

		m_wndList.DeleteAllItems();

		if( bActive ){
			OnTimerStart();
		}
	}

	void	OnTimerStart( void ){
		m_bTimerActive	= true;
		SetTimer( ID_TIMER_EVENT, 100 );
	}

	void	OnTimerStop( void ){
		m_bTimerActive	= false;
		KillTimer( ID_TIMER_EVENT );
	}
};

class CMessageViewerThread : public IRunnable {
public:
	void	run( void ){
		CMessageViewerForm	Form;
		if( Form.Create(_T("MessageViewer"), 0, 0, 100, 100, 600, 600) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};