#pragma once

#include "Generic.h"
#include "Inet.h"
#include "IEventHandler.h"
#include "Buffer.h"

using namespace GENERIC;
using namespace INET;

/**********************************************************************************
 */
class CSession : public IEventHandler {
public:
	OVERLAPPED			m_ol;
	WSABUF				m_wsaBuf;
	CBuffer				m_Buffer;
	CSocket*			m_pSocket;
	IEventHandler*		m_pProtocol;
	int					m_op;

	CSession( CSocket* pSock, IEventHandler* pProtocol )
		: m_pSocket(pSock), m_pProtocol(pProtocol)
	{
		m_Buffer.Resize( 0x8000 );
	}

	~CSession( void ){
		OnClose();
	}

	void		OnEvent( int operation, DWORD dwSize = 0 ){
		switch( operation ){
		case OP_CONNECT:
			OnConnect( NULL );
			Recv();
			break;

		case OP_READ:
			m_Buffer.Resize( dwSize );

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
					m_Buffer.Resize( 0x8000 );

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
				m_Buffer.Resize( 0x8000 );

				Recv();
			}
			else
			{
				Send();
			}
			break;
		}
	}

	void		OnEvent( DWORD dwSize ){
		CSession::OnEvent( m_op, dwSize );
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
/**********************************************************************************
 */
class CSessionManager : public TSingleton<CSessionManager> {
public:
	typedef std::map<DWORD, CSession*>		MAP;
	typedef std::pair<DWORD, CSession*>		PAIR;

private:
	CCriticalSection		m_cs;
	MAP						m_map;

public:
	template<class PROTOCOL>
	CSession*	New( DWORD dwKey, LPVOID lpParam ){
		CSingleLock	lock(&m_cs);

		CSession* pSession = new CSession( new CSocket(*(SOCKET*)lpParam), new PROTOCOL );
		if( pSession )
		{
			m_map.insert( PAIR(dwKey, pSession) );
			return pSession;
		}
		return NULL;
	}

	void		Delete( DWORD dwKey ){
		CSingleLock	lock(&m_cs);

		MAP::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			CSession*	pSession = it->second;
			m_map.erase( it );

			if( pSession )
				delete pSession;
		}
	}

	CSession*	Find( DWORD dwKey ){
		CSingleLock	lock(&m_cs);

		MAP::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			return it->second;
		}
		return NULL;
	}
};
