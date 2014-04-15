// KeyValue.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "KeyValue.h"

#include "Inet.h"
#include "Thread.h"
#include "IOCP.h"

#include "ISocket.h"
#include "IEventHandler.h"

using namespace INET;
using namespace MT;

class CSession : public IEventHandler {
public:
	enum {
		OP_READ, OP_WRITE
	};

	CSocket*			m_pSocket;

	CSession( LPVOID lpParam ) : m_pSocket((CSocket*)lpParam) {}
};



class CSessionManager {
private:
	static std::map<DWORD, LPVOID>		m_map;

public:
	static LPVOID		New( DWORD dwKey, LPVOID lpParam ){
		CSession* pSession = new CSession( lpParam );
		if( pSession )
		{
			m_map.insert( std::pair<DWORD, LPVOID>(dwKey, (LPVOID)pSession) );
			return (LPVOID)pSession;
		}
		return NULL;
	}

	static void			Delete( DWORD dwKey ){
		std::map<DWORD, LPVOID>::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			LPVOID	lpParam = it->second;
			m_map.erase( it );

			if( lpParam )
				delete lpParam;
		}
	}

	static LPVOID		Find( DWORD dwKey ){
		std::map<DWORD, LPVOID>::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			return it->second;
		}
		return NULL;
	}
};

std::map<DWORD, LPVOID>		CSessionManager::m_map;


class CIOCPWorker	: public IRunnable {
public:
	CIOCP*			m_pIOCP;
	CEvent*			m_pEventShutdown;

	CIOCPWorker( CIOCP* pIOCP, CEvent* pEvent )
	 : m_pIOCP(pIOCP), m_pEventShutdown(pEvent) {
	}

	void	run( void ){
		while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_pEventShutdown->m_hObject, 0) )
		{
			LPOVERLAPPED		lpOverlapped		= NULL;
			DWORD				dwBytesTransfered	= 0;
			LPVOID				pSession			= NULL;
			BOOL	bReturn	= m_pIOCP->GetQueuedCompletionStatus( &dwBytesTransfered, (LPDWORD)&pSession, &lpOverlapped, INFINITE );
			if( pSession == NULL ){
				break;
			}

		}
	}
};

class CIOCPServer	: public IRunnable {
public:
	CEvent				m_evntShutdown;

	CListenSocket		m_sockListen;

	CIOCP				m_IOCP;

	void	run( void )
	{
		m_evntShutdown.m_hObject	= ::CreateEvent(NULL, TRUE, FALSE, NULL);

		m_IOCP.Create();

		m_sockListen.Create( 54321 );

		while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_evntShutdown.m_hObject, 0) )
		{
			int	len;
			SOCKADDR	addr;
			CSocket	sock	= m_sockListen.Accept( addr, len );
			if( sock.IsValid() )
			{
				SOCKET	s	= sock.Detach();
				m_IOCP.IoCompletionPort( (HANDLE)s, (ULONG_PTR)CSessionManager::New( (DWORD)s, (LPVOID)(new CSocket(s)) ) );
			}
		}
	}
};



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	CThread*	pThread	= new CThread( new CIOCPServer() );
	if( pThread )
	{
		pThread->Wait();
	}
}
