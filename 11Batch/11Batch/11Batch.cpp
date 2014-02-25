// 11Batch.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "11Batch.h"

#include "UseToolkit.h"

#define	WM_APP_COMPLETED		(WM_APP+123)


class CMainForm : public CForm {
public:
	enum {
		IDC_LISTBOX		= 1001	,
		IDC_START_BTN			,
	};

public:
	CMainForm( void ) : m_nCurrentTask(0), m_pWorkItem(0) {}

protected:
	volatile LONG	m_nCurrentTask;
	PTP_WORK		m_pWorkItem;
	CButton			m_btnStart;
	CListBox		m_ListBox;
	
	TMessageHandler<CMainForm>	msg;
	TCommandHandler<CMainForm>	cmd;

	static
	void NTAPI	WorkerThread( PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work )
	{
		CMainForm*	pMainForm	= (CMainForm*)(Context);
		if( pMainForm ){
			pMainForm->TaskHandler();
		}
	}

	void	TaskHandler( void ){
		LONG	currentTask	= InterlockedIncrement( &m_nCurrentTask );

		TString	str;
		str.Format(_T("[%u] Task #%u is starting."), GetCurrentThreadId(), currentTask);
		AddMessage( (LPCTSTR)str );

		Sleep( currentTask * 1000 );

		str.Format(_T("[%u] Task #%u is done."), GetCurrentThreadId(), currentTask);
		AddMessage( (LPCTSTR)str );

		if( InterlockedDecrement(&m_nCurrentTask) == 0 ){
			PostMessage( WM_APP_COMPLETED, 0, (LPARAM)currentTask );
		}
	}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_pWorkItem	= CreateThreadpoolWork( WorkerThread, this, NULL );
		if( m_pWorkItem == NULL ){
			return FALSE;
		}

		m_btnStart.Create(this, 10, 10, 100, 25, IDC_START_BTN, _T("START"));
		m_btnStart.SetFont();

		m_ListBox.Create(this, 10, 30, 200, 200, IDC_LISTBOX);
		m_ListBox.SetFont();

		cmd.Initialize( this );
		cmd.Register( IDC_START_BTN, &CMainForm::OnStartBatch );

		msg.Initialize( this );
		msg.Register( WM_APP_COMPLETED, &CMainForm::OnCompleted );

		return TRUE;
	}

	void	OnDestroy( void ){
		CloseThreadpoolWork( m_pWorkItem );
	}

	void	OnWndMsg( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		msg.Dispach(uMsg, wParam, lParam);
	}

	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);
	}

	LRESULT	OnCompleted( WPARAM wParam, LPARAM lParam ){
		TString	str;
		str.Format(_T("___Task #%u was the last task of the batch____"), lParam);
		AddMessage( (LPCTSTR)str );

		m_btnStart.Enable( TRUE );

		return 0;
	}

	void	OnStartBatch( void ){
		m_btnStart.Enable( FALSE );

		AddMessage(_T("---- start a new batch ----"));

		SubmitThreadpoolWork( m_pWorkItem );
		SubmitThreadpoolWork( m_pWorkItem );
		SubmitThreadpoolWork( m_pWorkItem );
		SubmitThreadpoolWork( m_pWorkItem );

		AddMessage(_T("4 tasks are submitted."));
	}

	void	AddMessage( LPCTSTR szMsg ){
		int	idx = m_ListBox.AddString( szMsg );
		m_ListBox.SetCurSel( idx );
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CMainForm	Form;
	if( Form.Create(_T(""), 0, 0, 100, 100, 300, 300) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
