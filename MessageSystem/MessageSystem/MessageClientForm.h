#pragma once

#include "MessageStructure.h"
#include "MessageClient.h"

#include "Form.h"
#include "ComponentWnd.h"
#include "Handler.h"
#include "Thread.h"

using namespace MT;

class CMessageClientForm : public CForm {
protected:
	enum {
		IDC_BTN_SEND		= 1001	,
		IDC_BTN_CONNECT				,
		IDC_EDT_SENDMSG				,
		IDC_BTN_CLOSE				,
	};

	CButton				m_btnConnect;
	CButton				m_btnSend;
	CButton				m_btnClose;
	CEdit				m_edtSendMsg;

	CMessageClient		m_client;

	TCommandHandler<CMessageClientForm>		cmd;


	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_btnConnect.Create(this,10, 10, 80, 25, IDC_BTN_CONNECT, _T("CONNECT"));
		m_btnSend.Create(this, 10, 40, 80, 25, IDC_BTN_SEND, _T("SEND"));
		m_edtSendMsg.Create(this, 100, 40, 320, 25, IDC_EDT_SENDMSG, _T(""));
		m_btnClose.Create(this, 10, 70, 80, 25, IDC_BTN_CLOSE, _T("CLOSE"));

		cmd.Initialize( this );
		cmd.Register( IDC_BTN_CONNECT, &CMessageClientForm::OnBtnConnect );
		cmd.Register( IDC_BTN_SEND, &CMessageClientForm::OnBtnSend );
		cmd.Register( IDC_BTN_CLOSE, &CMessageClientForm::OnBtnClose );

		m_btnConnect.Enable( TRUE );
		m_btnSend.Enable( FALSE );
		m_btnClose.Enable( FALSE );

		SetFontChildren();
		return TRUE;
	}

	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);
	}

	void	OnBtnConnect( void ){
		if( !m_client.IsConnected() )
		{
			if( m_client.Connect("127.0.0.1", 5511) )
			{
				m_btnConnect.Enable( FALSE );
				m_btnSend.Enable( TRUE );
				m_btnClose.Enable( TRUE );
			}
		}
	}

	void	OnBtnSend( void ){
		TCHAR	lpBuf[256];
		if( m_edtSendMsg.GetTextLength() > 0 ){
			m_edtSendMsg.GetText( lpBuf, _countof(lpBuf) );
		}

		if( m_client.IsConnected() ){
			m_client.Send( lpBuf );
		}
	}

	void	OnBtnClose( void ){
		if( m_client.IsConnected() ){
			m_client.Close();

			m_btnConnect.Enable( TRUE );
			m_btnSend.Enable( FALSE );
			m_btnClose.Enable( FALSE );
		}
	}
};

class CMessageClientUIThread : public IRunnable {
public:
	void	run( void ){
		CMessageClientForm	Form;
		if( Form.Create(_T(""), 0, 0, 100, 200, 480, 200) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};
