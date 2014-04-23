#pragma once

#include "Form.h"
#include "ComponentWnd.h"
#include "Handler.h"

#include "Thread.h"

#include "SessionProtocol.h"
#include "IOCP.h"

#include "MessagePool.h"

#include "trace.h"


using namespace MT;


class CMessageReciverProtocol : public IEventHandler {
public:
	CBuffer			m_Buffer;


	int		OnRead( LPVOID lpParam ){
		CBuffer*	pBuffer = (CBuffer*)lpParam;
		if( pBuffer ){
			CMessageHeader* pMsg = (CMessageHeader*)pBuffer->At(0);
			if( pMsg ){
				CMessagePool::Instance().Append( pMsg );
				TRACE( pMsg->subject );
				return 1;
			}
		}
		return 0;
	}

	int		OnWrite( LPVOID lpParam ){
		return 0;
	}

	void	OnClose( void ){
	}
};


class CMessageReciverWorker : public IRunnable {
protected:
	CIOCP*		m_pIOCP;
	CEvent*		m_pEventShutdown;

public:
	CMessageReciverWorker( CIOCP* pIOCP, CEvent* pEvent ) 
	 : m_pIOCP(pIOCP), m_pEventShutdown(pEvent) {
	}

	void	run( void ){
		while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_pEventShutdown->m_hObject, 0) )
		{
			LPOVERLAPPED		lpOverlapped		= NULL;
			DWORD				dwBytesTransfered	= 0;
			CSession*			pSession			= NULL;
			BOOL	bReturn	= m_pIOCP->GetQueuedCompletionStatus( &dwBytesTransfered, (LPDWORD)&pSession, &lpOverlapped, INFINITE );
			if( pSession == NULL ){
				break;
			}

			if( (bReturn == FALSE) || ((bReturn == TRUE) && (dwBytesTransfered == 0))	)
			{
				CSessionManager::Instance().Delete( (DWORD)pSession->m_pSocket->m_hHandle );
				continue;
			}

			pSession->OnEvent( dwBytesTransfered );
		}

		TRACE("ExitThread: %d\n", ::GetCurrentThreadId());
	}
};

class CMessageReciver : public IRunnable {
public:
	enum {
		NUM_THREADS		= 4	,
	};

	CIOCP				m_IOCP;
	CEvent				m_EventShutdown;
	CListenSocket		m_sockListen;

	CThread*			m_pThreads[NUM_THREADS];

	void	run( void ){
		m_EventShutdown.m_hObject	= ::CreateEvent(NULL, TRUE, FALSE, NULL);

		m_IOCP.Create();

		for( int i=0; i<NUM_THREADS; i++ ){
			m_pThreads[ i ]	= new CThread( new CMessageReciverWorker(&m_IOCP, &m_EventShutdown) );
			m_pThreads[ i ]->Start();
		}


		if( m_sockListen.Create( 5511 ) )
		{
			while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_EventShutdown.m_hObject, 0) )
			{
				SOCKADDR_IN		addr;
				int	len	= sizeof(addr);
				CSocket	sock	= m_sockListen.Accept( (LPSOCKADDR)&addr, len );
				if( sock.IsValid() )
				{
					SOCKET	s	= sock.Detach();

					CSession*	pSession = CSessionManager::Instance().New<CMessageReciverProtocol>( (DWORD)s, (LPVOID)&s );
					if( pSession )
					{
						m_IOCP.IoCompletionPort( (HANDLE)s, (ULONG_PTR)pSession );

						pSession->OnEvent( OP_CONNECT );
					}
				}
			}
		}

		HANDLE	hThreads[NUM_THREADS] = {NULL};
		for( int i=0; i<NUM_THREADS; i++ ){
			hThreads[ i ]	= m_pThreads[ i ]->m_hThread;
		}

		::WaitForMultipleObjects(NUM_THREADS, hThreads, TRUE, INFINITE);
	}

	void	exit( void ){
		m_EventShutdown.SetEvent();
	}
};

class CMessageReciverForm : public CForm {
protected:
	enum {
		IDC_BTN_START		= 1001	,
		IDC_BTN_STOP				,
	};

	CButton		m_btnStart, m_btnStop;

	CThread*	m_pThread;

	TCommandHandler<CMessageReciverForm>	cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){

		m_btnStart.Create(this, 10, 10, 80, 25, IDC_BTN_START, _T("START"));
		m_btnStop.Create(this, 10, 40, 80, 25, IDC_BTN_STOP, _T("STOP"));

		cmd.Initialize( this );
		cmd.Register( IDC_BTN_START, &CMessageReciverForm::OnBtnStart );
		cmd.Register( IDC_BTN_STOP, &CMessageReciverForm::OnBtnStop );

		m_pThread	= new CThread( new CMessageReciver() );

		SetFontChildren();
		return TRUE;
	}

	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);
	}

	void	OnBtnStart( void ){
		if( m_pThread ){
			m_pThread->Start();
		}
	}

	void	OnBtnStop( void ){
		if( m_pThread ){
			m_pThread->Terminate();
		}
	}

};

class CMessageReciverUIThread : public IRunnable {
public:
	void	run( void ){
		CMessageReciverForm	Form;
		if( Form.Create(_T(""), 0, 0, 100, 100, 300, 300) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};