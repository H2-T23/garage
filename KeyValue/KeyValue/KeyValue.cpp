// KeyValue.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "KeyValue.h"

#include "Inet.h"
#include "Thread.h"
#include "IOCP.h"

#include "ISocket.h"
#include "IEventHandler.h"

#include "KeyValueClient.h"
#include "KeyValueServerForm.h"

using namespace INET;
using namespace MT;


class CKeyValueProtocol : public IEventHandler {
public:
	CBuffer				m_RecvBuffer;
	CBuffer				m_SendBuffer;


	void	Done( std::string& str ){
		char	strCmd[256], strKey[256], strValue[256];

		str.append( "\0" );
		sscanf( str.c_str(), "%s %s %s\n", strCmd, strKey, strValue );
		TRACE( str.c_str() );

		if( str.compare(0, 3, "Set") == 0 )
		{
			CKeyValue::Instance().Set( strKey, strValue );
		}
		else 
		if( str.compare(0, 3, "Get") == 0 )
		{
			std::string	str( CKeyValue::Instance().Get( strKey ) );
			str.append( "\n" );
			m_SendBuffer.Set( (LPBYTE)str.c_str(), str.length() );
		}
	}

	int		OnRead( LPVOID lpParam ){

		CBuffer*	pBuff	= (CBuffer*)(lpParam);
		//if( pBuff )
		//{
		//	BYTE* p = pBuff->At( 0 );
		//	while( *p ){
		//		TRACE("%c ",(char)*p);
		//		p++;
		//	}
		//	TRACE("\n");
		//}

		m_RecvBuffer.Append( *pBuff );


		int	iStart	= 0;
		int iEnd	= m_RecvBuffer.Find( (BYTE)'\n', iStart );
		if( iEnd < 0 )
		{
			return 0;
		}
		else
		{
			std::string	strTotal( (char*)m_RecvBuffer.At( 0 ) );
			m_RecvBuffer.Clear();

			int	len	= strTotal.length();

			std::string	str;
			do{
				int	sz	= (iEnd + 1) - iStart;
				str.resize( sz );
				memcpy( &str.at( 0 ), &strTotal.at( iStart ), sz );

				Done( str );

				iStart = iEnd + 1;
				len -= sz;
				iEnd	= strTotal.find( '\n', iStart );
			}while( iEnd > 0 );

			if( len > 0 )
			{
				int next	= strTotal.length() - len;
				m_RecvBuffer.Set( (LPBYTE)&strTotal.at( next ), len );
			}
		
			return 1;
		}

		//std::string		str( (char*)m_RecvBuffer.At( 0 ) );

		//int idx	= str.find( '\n', 0 );
		//if( idx > 0 )
		//{
		//	char	strCmd[256], strKey[256], strValue[256];

		//	str.append( "\0" );
		//	sscanf( str.c_str(), "%s %s %s\n", strCmd, strKey, strValue );
		//	TRACE( str.c_str() );

		//	if( str.compare(0, 3, "Set") == 0 )
		//	{
		//		CKeyValue::Instance().Set( strKey, strValue );
		//	}
		//	else 
		//	if( str.compare(0, 3, "Get") == 0 )
		//	{
		//		std::string	str( CKeyValue::Instance().Get( strKey ) );
		//		str.append( "\n" );
		//		m_SendBuffer.Set( (LPBYTE)str.c_str(), str.length() );
		//	}

		//	m_RecvBuffer.Clear();

		//	return 1;
		//}

		//return 0;
	}

	int		OnWrite( LPVOID lpParam ){
		CBuffer* pBuff = (CBuffer*)lpParam;
		if( pBuff )
		{
			if( m_SendBuffer.Size() > 0 )
			{
				pBuff->Set( m_SendBuffer );
			}

			m_SendBuffer.Clear();
			return 1;
		}

		return 0;
	}

	void	OnClose( void ){
	}
};

class CSession : public IEventHandler {
public:
	OVERLAPPED			m_ol;
	WSABUF				m_wsaBuf;
	CBuffer				m_Buffer;
	CSocket*			m_pSocket;
	IEventHandler*		m_pProtocol;
	int					m_op;

	CSession( SOCKET s )
		: m_pSocket(new CSocket(s)), m_pProtocol(new CKeyValueProtocol())
	{
		unsigned	nSize	= 0xFFFF;
		m_Buffer.Resize( nSize );
	}

	~CSession( void ){
		OnClose();
	}

	void		OnEvent( int operation ){
		switch( operation ){
		case OP_CONNECT:
			OnConnect( NULL );
			Recv();
			break;

		case OP_READ:
			if( m_pProtocol->OnRead( &m_Buffer ) > 0 )
			{
				m_Buffer.Clear();
				m_pProtocol->OnWrite( &m_Buffer );
				
				if( m_Buffer.Size() > 0 )
				{
					Send();
				}
				else
				{
					m_Buffer.Clear();

					unsigned	nSize	= 0xFFFF;
					m_Buffer.Resize( nSize );

					Recv();
				}
			}
			else
			{
				Recv();
			}
			break;

		case OP_WRITE:
			if( m_pProtocol->OnWrite( &m_Buffer ) > 0 )
			{
				m_Buffer.Clear();

				unsigned	nSize	= 0xFFFF;
				m_Buffer.Resize( nSize );

				Recv();
			}
			else
			{
				Send();
			}
			break;
		}
	}

	void		OnEvent( void ){
		CSession::OnEvent( m_op );
	}

	void		OnClose( void ){
		if( m_pProtocol ){
			m_pProtocol->OnClose();
			delete m_pProtocol;
			m_pProtocol	= NULL;
		}

		if( m_pSocket ){
			m_pSocket->Close();
			delete m_pSocket;
			m_pSocket	= NULL;
		}
	}

	BOOL		Recv( void ){
		DWORD	dwFlags	= 0;

		m_wsaBuf.buf	= (char*)m_Buffer.At( 0 );
		m_wsaBuf.len	= m_Buffer.Size();

		memset(&m_ol, 0, sizeof(WSAOVERLAPPED));
		m_op	= OP_READ;

		if( m_pSocket->WSARecv(&m_wsaBuf, 1, NULL, &dwFlags, &m_ol, NULL) == SOCKET_ERROR )
		{
			if( ::WSAGetLastError() == WSA_IO_PENDING )
			{
				return TRUE;
			}
			return FALSE;
		}
		return TRUE;
	}

	BOOL		Send( void ){
		DWORD	dwFlags	= 0;

		m_wsaBuf.buf	= (char*)m_Buffer.At( 0 );
		m_wsaBuf.len	= m_Buffer.Size();

		memset(&m_ol, 0, sizeof(WSAOVERLAPPED));
		m_op	= OP_WRITE;

		if( m_pSocket->WSASend(&m_wsaBuf, 1, NULL, dwFlags, &m_ol, NULL) == SOCKET_ERROR )
		{
			if( ::WSAGetLastError() == WSA_IO_PENDING )
			{
				return TRUE;
			}
			return FALSE;
		}
		return TRUE;
	}
};

class CSessionManager {
public:
	typedef std::map<DWORD, CSession*>		MAP;
	typedef std::pair<DWORD, CSession*>		PAIR;

private:
	static MAP		m_map;

public:
	static LPVOID		New( DWORD dwKey, LPVOID lpParam ){
		CSession* pSession = new CSession( *(SOCKET*)lpParam );
		if( pSession )
		{
			m_map.insert( PAIR(dwKey, pSession) );
			return (LPVOID)pSession;
		}
		return NULL;
	}

	static void			Delete( DWORD dwKey ){
		MAP::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			CSession*	pSession = it->second;
			m_map.erase( it );

			if( pSession )
				delete pSession;
		}
	}

	static CSession*	Find( DWORD dwKey ){
		MAP::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			return it->second;
		}
		return NULL;
	}
};

CSessionManager::MAP		CSessionManager::m_map;


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
			CSession*			pSession			= NULL;
			BOOL	bReturn	= m_pIOCP->GetQueuedCompletionStatus( &dwBytesTransfered, (LPDWORD)&pSession, &lpOverlapped, INFINITE );
			if( pSession == NULL ){
				break;
			}

			if( (bReturn == FALSE) || ((bReturn == TRUE) && (dwBytesTransfered == 0))	)
			{
				CSessionManager::Delete( (DWORD)pSession->m_pSocket->m_hHandle );
				continue;
			}

			pSession->OnEvent();
		}
	}
};

class CIOCPServer	: public IRunnable {
public:
	CEvent				m_evntShutdown;

	CListenSocket		m_sockListen;

	CIOCP				m_IOCP;

	CThread*			m_pThreads[ 4 ];

	void	run( void )
	{
		m_evntShutdown.m_hObject	= ::CreateEvent(NULL, TRUE, FALSE, NULL);

		m_IOCP.Create();

		for( int i=0; i<4; i++ ){
			m_pThreads[ i ]	= new CThread( new CIOCPWorker(&m_IOCP, &m_evntShutdown) );
			m_pThreads[ i ]->Start();
		}


		if( m_sockListen.Create( 54321 ) )
		{
			while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_evntShutdown.m_hObject, 0) )
			{
				SOCKADDR_IN		addr;
				int	len	= sizeof(addr);
				CSocket	sock	= m_sockListen.Accept( (LPSOCKADDR)&addr, len );
				if( sock.IsValid() )
				{
					SOCKET	s	= sock.Detach();
					CSession*	pSession	= (CSession*)CSessionManager::New( (DWORD)s, (LPVOID)&s );
					if( pSession )
					{
						m_IOCP.IoCompletionPort( (HANDLE)s, (ULONG_PTR)pSession );

						pSession->OnEvent( IEventHandler::OP_CONNECT );
					}
				}
			}
		}

		for( int i=0; i<4; i++ ){
			m_pThreads[ i ]->exit();
		}
	}
};



class CKeyValueClient	: public IRunnable {
protected:
	void		run( void )
	{
		CClientForm	Form;
		if( Form.Create(_T("Client"), 0, 0, 100, 100, 600, 600) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
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

	INET::WinInet::Startup();

	CThread*	pKeyValue	= new CThread( new CKeyValueServerFormThread() );
	if( pKeyValue )
	{
		pKeyValue->Start();
	}


	CThread*	pThreadServer	= new CThread( new CIOCPServer() );
	if( pThreadServer )
	{
		pThreadServer->Start();

		Sleep( 500 );

		CThread*	pThreadClient	= new CThread( new CKeyValueClient() );
		pThreadClient->Start();
		pThreadServer->Wait();
	}

	INET::WinInet::Cleanup();
}
