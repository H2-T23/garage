#pragma once


#include "Form.h"
#include "ComponentWnd.h"
#include "Handler.h"

#include "Inet.h"

using namespace INET;


class ICommandHandler {
public:
	virtual void	OnCommand			( UINT uID, HWND hWndCtrl, UINT nCodeNotify ) = 0;
};

class CClientForm : public CForm, public ICommandHandler {
protected:
	enum {
		IDC_BTN_CONNECT	= 1001	,
		IDC_BTN_SEND			,
		IDC_BTN_RECV			,
		IDC_BTN_SHUTDOWN		,
		IDC_BTN_TEST1			,
		IDC_BTN_TEST2			,

		IDC_EDT_HOSTNAME		,
		IDC_EDT_SENDMSG			,
		IDC_EDT_RECVMSG			,
	};

	CButton			m_btnConnect;
	CButton			m_btnSend;
	CButton			m_btnRecv;
	CButton			m_btnShutdown;
	CButton			m_btnTest1;
	CButton			m_btnTest2;

	CEdit			m_edtHostName;
	CEdit			m_edtSendMsg;
	CEdit			m_edtRecvMsg;


	CTCPSocket		m_sock;

	TCommandHandler<CClientForm>	cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){

		m_btnConnect.Create(this, 10, 10, 80, 25, IDC_BTN_CONNECT, _T("CONNECT"));
		m_btnSend.Create(this, 10, 35, 80, 25, IDC_BTN_SEND, _T("SEND"));
		m_btnRecv.Create(this, 10, 60, 80, 25, IDC_BTN_RECV, _T("RECV"));
		m_btnShutdown.Create(this, 10, 85, 80, 25, IDC_BTN_SHUTDOWN, _T("SHUTDOWN"));
		m_btnTest1.Create(this, 10, 115, 80, 25, IDC_BTN_TEST1, _T("TEST1"));
		m_btnTest2.Create(this, 10, 140, 80, 25, IDC_BTN_TEST2, _T("TEST2"));
	
		m_edtHostName.Create(this, 100, 10, 320, 25, IDC_EDT_HOSTNAME, _T("127.0.0.1"));
		m_edtSendMsg.Create(this, 100, 35, 320, 25, IDC_EDT_SENDMSG, _T("Set Key Value"));
		m_edtRecvMsg.Create(this, 100, 60, 320, 25, IDC_EDT_RECVMSG, _T(""));


		cmd.Initialize( this );
		cmd.Register( IDC_BTN_CONNECT, &CClientForm::OnBtnConnect );
		cmd.Register( IDC_BTN_SEND, &CClientForm::OnBtnSend );
		cmd.Register( IDC_BTN_RECV, &CClientForm::OnBtnRecv );
		cmd.Register( IDC_BTN_SHUTDOWN, &CClientForm::OnBtnShutdown );
		cmd.Register( IDC_BTN_TEST1, &CClientForm::OnBtnTest1 );
		cmd.Register( IDC_BTN_TEST2, &CClientForm::OnBtnTest2 );

		SetFontChildren();
		return TRUE;
	}

	void	OnBtnConnect( void ){
		if( m_sock.IsValid() )
			return;
		
		char	lpHostname[256];
		TCHAR	lpBuf[256];
		if( m_edtHostName.GetTextLength() > 0 ){
			m_edtHostName.GetText( lpBuf, _countof(lpBuf) );
			TString::W2A( lpBuf, _countof(lpBuf), lpHostname, _countof(lpHostname) );
		}

		if( strlen(lpHostname) <= 0 )
		{
			return;
		}

		if( m_sock.Create() )
		{
			struct sockaddr_in	server;
			server.sin_family			= AF_INET;
			server.sin_port				= htons( 54321 );
			server.sin_addr.s_addr		= inet_addr(lpHostname/* "127.0.0.1" */);
			if( server.sin_addr.S_un.S_addr < 0 )
			{
				struct hostent*	pHost = gethostbyname(lpHostname/* "127.0.0.1" */);
				if( pHost )
				{
					server.sin_addr.S_un.S_addr	= *(unsigned int*)pHost->h_addr_list[ 0 ];
				}
				else
				{
					return;
				}
			}

			m_sock.Connect((const sockaddr*)&server, sizeof(server));
		}
	}

	void	OnBtnSend( void ){
		char	lpSendData[256];
		TCHAR	lpBuf[256];
		if( m_edtSendMsg.GetTextLength() > 0 ){
			m_edtSendMsg.GetText( lpBuf, _countof(lpBuf) );
			TString::W2A( lpBuf, _countof(lpBuf), lpSendData, _countof(lpSendData) );
		}

		std::string	str(lpSendData);

		if( str.length() <= 0 )
		{
			return;
		}

		if( m_sock.IsValid() )
		{
		//	std::string		str;
		//	str = "Set system.version 1.123\n";
			str.append( "\n\0" );
			m_sock.Send( (LPVOID)str.c_str(), str.length() );
		}
	}

	void	OnBtnRecv( void ){
		if( m_sock.IsValid() )
		{
			char	buf[256];
			m_sock.Recv( buf, 256 );

			std::string	str(buf);
			if( str.length() ){
				TCHAR	wbuf[256];
				TString::A2W(buf, 256, wbuf, 256);
				m_edtRecvMsg.SetText( wbuf );
			}
		}
	}

	void	OnBtnShutdown( void ){
		if( m_sock.IsValid() )
		{
			m_sock.Shutdown( 2 );
			m_sock.Close();
		}
	}

	void	OnBtnTest1( void ){
		if( m_sock.IsValid() )
		{
			char	buf[256];
			std::string	str;
			for( int i=0; i<3200; i++ )
			{
				sprintf( buf, "Set Key%d %d\n\0", i+1, i+1 );
			//	TRACE("Send: %s", buf);

				str = buf;
				m_sock.Send( (LPVOID)str.c_str(), str.length() );
			}
		}
	}

	void	OnBtnTest2( void ){
		if( m_sock.IsValid() )
		{
			char	buf[256];
			std::string	str;
			for( int i=0; i<3200; i++ )
			{
				sprintf( buf, "Get Key%d\n\0", i+1 );
				TRACE("Send: %s", buf);

				str = buf;
				m_sock.Send( (LPVOID)str.c_str(), str.length() );
				int nRet = m_sock.Recv( buf, 256 );
				if( nRet > 0 )
				{
					buf[nRet] = '\0';
					TRACE("Result: %s\n", buf);
				}
				else
				{
					TRACE("Result: ERR\n");
				}
			}
		}
	}

	void	OnCommand			( UINT uID, HWND hWndCtrl, UINT nCodeNotify ) {
		cmd.Dispach(uID, nCodeNotify);
	}
};