#pragma once

#include "MessageStructure.h"
#include "Inet.h"

using namespace INET;


class CMessageClient {
protected:
	CTCPSocket				m_sock;

	BOOL	MakeAddress( struct sockaddr_in& Addr, const CHAR* lpHostname, int nPort ){
		Addr.sin_family				= AF_INET;
		Addr.sin_port				= htons( nPort );
		Addr.sin_addr.s_addr		= inet_addr(lpHostname/* "127.0.0.1" */);

		if( Addr.sin_addr.S_un.S_addr < 0 )
		{
			struct hostent*	pHost = gethostbyname(lpHostname/* "127.0.0.1" */);
			if( pHost )
			{
				Addr.sin_addr.S_un.S_addr	= *(unsigned int*)pHost->h_addr_list[ 0 ];
			}
			else
			{
				return FALSE;
			}
		}
		return TRUE;
	}

public:
	inline BOOL	IsConnected( void ) const {
		return(m_sock.IsValid()?TRUE:FALSE);
	}

	BOOL	Connect( const WCHAR* lpHostname, int nPort ){
		CHAR	buf[256];
		TString	str(lpHostname);
		if( TString::W2A(lpHostname, str.Length() + 1, buf, _countof(buf)) ){
			return Connect(buf, nPort);
		}
		return FALSE;
	}

	BOOL	Connect( const CHAR* lpHostname, int nPort ){
		if( !IsConnected() )
		{
			if( m_sock.Create() )
			{
				struct sockaddr_in	server = {0};
				if( MakeAddress(server, lpHostname, nPort) )
				{
					if( m_sock.Connect((const sockaddr*)&server, sizeof(server)) == 0 )
					{
						return TRUE;
					}
					else
					{
						m_sock.Close();
					}
				}
			}
		}
		return FALSE;
	}

	BOOL	Send( const WCHAR* pMessage ){
		CHAR	buf[256];
		TString	str(pMessage);
		if( TString::W2A(pMessage, str.Length()+1, buf, _countof(buf)) ){
			return Send( buf );
		}
		return FALSE;
	}

	BOOL	Send( const CHAR* pMessage ){
		if( IsConnected() ){
			CMessageHeader	msg( ::GetCurrentThreadId() );
			msg.SetCurrentTime();
			strcpy(msg.subject, pMessage);

			if( m_sock.Send( (LPVOID)&msg, sizeof(msg) ) > 0 )
				return TRUE;
		}
		return FALSE;
	}

	void	Close( void ){
		if( IsConnected() ){
			m_sock.Shutdown( 1 );
			char buf[256];
			m_sock.Recv( buf, 256 );
			m_sock.Shutdown( 2 );
			m_sock.Close();
		}
	}
};
