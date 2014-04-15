#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <mswsock.h>

#include <tlhelp32.h>
#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")


#include "Inet.h"
#include "Thread.h"
#include "Buffer.h"
#include "IOCP.h"




namespace ECHO_IOCP {
	/**********************************************************************************
	 *
	 *
	 *
	 */
	typedef	CBuffer			CPacket;
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CUniformRandom {
	public:
		void	Generate( int nSeed ){
			srand( nSeed );
		}

		int		GetNext( void ) const {
			return rand();
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CXorShiftRandom {
	protected:
		ULONG		x, y, z, w;

	public:
		CXorShiftRandom( void ) : x(123456789), y(362436069), z(521288629), w(88675123) {
		}

		ULONG		GetNext( void ){
			ULONG	u	= (x ^ (x << 11));
			x = y;
			y = z;
			z = w;
			w	= (w ^ (w >> 19)) ^ (u ^ (u >> 8));
			return w;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	enum {
		SERVER_ADDRESS	= INADDR_LOOPBACK,
		SERVICE_PORT	= 4000,
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CIocpState {
	public:
		enum OPERATION {
			OP_NONE			= 0	,
			OP_ACCEPT			,
			OP_READ				,
			OP_WRITE			,

			MAX_BUF			= 64	,
		};

		INET::CSocket		sock;
		OPERATION			operation;
		DWORD				length;
		char				buf[ MAX_BUF ];

		CPacket				m_packet;


		CIocpState( void ) : operation(OP_NONE), length(0) {}
		CIocpState( INET::CSocket& s ) : sock(s), operation(OP_NONE), length(0) {
		}

		virtual ~CIocpState( void ){
			sock.Close();
		}

		BOOL	Recv( WSAOVERLAPPED* pOverlapped )
		{
			DWORD	dwFlags	= 0;
			WSABUF	wsabuf	= { MAX_BUF, buf };

			memset(pOverlapped, 0, sizeof(WSAOVERLAPPED));
			operation	= OP_READ;

			if( sock.WSARecv(&wsabuf, 1, NULL, &dwFlags, pOverlapped, NULL) == SOCKET_ERROR )
			{
				if( ::WSAGetLastError() == WSA_IO_PENDING )
				{
					return TRUE;
				}
			}
			return FALSE;
		}

		BOOL	Send( WSAOVERLAPPED* pOverlapped )
		{
			WSABUF	wsabuf	= { length, buf };

			memset(pOverlapped, 0, sizeof(WSAOVERLAPPED));
			operation	= OP_WRITE;

			if( sock.WSASend(&wsabuf, 1, NULL, 0, pOverlapped, NULL) == SOCKET_ERROR )
			{
				if( WSAGetLastError() == WSA_IO_PENDING )
				{
					return TRUE;
				}
			}

			return FALSE;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	static 	GUID	GuidAcceptEx	= WSAID_ACCEPTEX;
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class WSAEx {
	private:
	public:
		static LPFN_ACCEPTEX	AcceptEx;

		static BOOL		Load( SOCKET sock ){
			DWORD	dwBytes	= 0;
			::WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, 
						&GuidAcceptEx, sizeof(GuidAcceptEx),
						&AcceptEx, sizeof(AcceptEx),
						&dwBytes, NULL, NULL);
			return TRUE;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CListener {
	protected:
		CListener( void );

	public:
		INET::CSocket			m_sock;
		CIocpState				m_state;
		WSAOVERLAPPED			m_overlapped;

		CIOCP*					m_pIOCP;

		LPFN_ACCEPTEX			pfAcceptEx;


		void		LoadAcceptExFunction( void ){
			DWORD	dwBytes;
			WSAIoctl( m_sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &pfAcceptEx, sizeof(pfAcceptEx), &dwBytes, NULL, NULL );
		}

	public:
		CListener( CIOCP* pIOCP ) : m_pIOCP(pIOCP), pfAcceptEx(NULL) {
			m_sock	= Create();
			
			m_state.sock		= 0;
			m_state.operation	= CIocpState::OP_ACCEPT;

			m_pIOCP->IoCompletionPort( (HANDLE)m_sock, (ULONG_PTR)&m_state );
		}

		INET::CSocket		Create( void ){
		//	INET::CSocket	sock(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			INET::CSocket	sock( ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED) );
			return sock;
		}

		BOOL		Listen( void ){
			if( m_sock.Listen(100) == SOCKET_ERROR )
			{
				return FALSE;
			}
			return TRUE;
		}

		BOOL		Bind( void ){
			struct sockaddr_in	sin;
			sin.sin_family			= AF_INET;
			sin.sin_addr.s_addr		= htonl( SERVER_ADDRESS );
			sin.sin_port			= htons( SERVICE_PORT );
		   
			if( m_sock.Bind((SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR )
			{
				return FALSE;
			}
			return TRUE;
		}

		BOOL		Accept( void ){
			INET::CSocket	acceptor	= Create();

			DWORD	dwExpected	= sizeof(struct sockaddr_in) + 16;
			
			m_state.sock	= acceptor;
			memset( &m_overlapped, 0, sizeof(WSAOVERLAPPED) );

			if( !pfAcceptEx( m_sock, acceptor, m_state.buf, 0, dwExpected, dwExpected, NULL, &m_overlapped ) )
			{
				if( WSAGetLastError() == ERROR_IO_PENDING )
				{
					return TRUE;
				}
			}
			return FALSE;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CIOCPControler : public MT::IThread {
	protected:
		CIOCP*			m_pIOCP;
		CListener*		m_pListener;

	public:
		CIOCPControler( void ) : m_pIOCP(NULL), m_pListener(NULL) {
			INET::WinInet::Startup();
		}

		~CIOCPControler( void ) {
			if( m_pIOCP )
				delete m_pIOCP;
			if( m_pListener )
				delete m_pListener;

			INET::WinInet::Cleanup();
		}

	protected:
		void	init( void )
		{
			m_pIOCP	= new CIOCP();
			m_pIOCP->Create();

			m_pListener = new CListener( m_pIOCP );
			m_pListener->Bind();
			m_pListener->Listen();
			m_pListener->LoadAcceptExFunction();
			m_pListener->Accept();
		}

		virtual unsigned		run( LPVOID lpParam )
		{
			init();

			DWORD			length;

			WSAOVERLAPPED*	pOverlapped		= NULL;
			CIocpState*		pState			= NULL;

			while( 1 )
			{
				BOOL	resultOK	= m_pIOCP->GetQueuedCompletionStatus( &length, (PULONG_PTR)&pState, &pOverlapped, INFINITE );
				switch( pState->operation ){
				case CIocpState::OP_ACCEPT:
					if( resultOK )
					{
						pState->sock.SetOption( SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_pListener->m_sock.m_hHandle, sizeof(m_pListener->m_sock.m_hHandle) );

						CIocpState*	pNewState = new CIocpState( pState->sock );

						m_pIOCP->IoCompletionPort( (HANDLE)pNewState->sock, (ULONG_PTR)pNewState );

						pNewState->Recv( new WSAOVERLAPPED );

						m_pListener->Accept();
					}
					else
					{
						pState->sock.Close();
						m_pListener->Accept();
					}
					break;

				case CIocpState::OP_READ:
					if( resultOK )
					{
						if( length > 0 )
						{
							DBG::TRACE(_T("* read operation completed, %d bytes read."), length);
							pState->length	= length;
							int	sz	= pState->m_packet.Append( (LPBYTE)pState->buf, length );
							if( sz >= 256 )
								pState->Send( pOverlapped );
							else
								pState->Recv( pOverlapped );
						}
						else
						{
							DBG::TRACE(_T("* connection closed by client."));
							delete pState;
							delete pOverlapped;
						}
					}
					else
					{
						DBG::TRACE(_T("* error %d in recv, assuming connection was reset by client."), GetLastError());
						delete pState;
						delete pOverlapped;
					}
					break;

				case CIocpState::OP_WRITE:
					if( resultOK )
					{
						if( length > 0 )
						{
							DBG::TRACE(_T("* write operation completed."));
							pState->Recv( pOverlapped );
						}
						else
						{
							DBG::TRACE(_T("* connection closed by client."));
							delete pState;
							delete pOverlapped;
						}
					}
					else
					{
						DBG::TRACE(_T("* error %d on send, assuming connection was reset."), GetLastError());
						delete pState;
						delete pOverlapped;
					}
					break;

				default:
					delete pState;
					delete pOverlapped;
					break;
				}
			}

			return 0;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	 class CEchoClient : public MT::IThread {
	 protected:
		 enum { BUFF_SIZE = 256 };
		 char					m_Buffer[ BUFF_SIZE ];
		 INET::CSocket			m_sock;

		 CXorShiftRandom		m_random;

		virtual unsigned		run( LPVOID lpParam ){
			INET::WinInet::Startup();

		//	m_random.Generate( ::GetTickCount() );

			if( m_sock.Create(AF_INET, SOCK_STREAM, 0) ){
				struct sockaddr_in	sin;
				sin.sin_family		= AF_INET;
				sin.sin_addr.s_addr	= htonl( SERVER_ADDRESS );
				sin.sin_port		= htons( SERVICE_PORT );

				if( m_sock.Connect( (struct sockaddr*)&sin, sizeof(sin) ) >= 0 ){
					for( int i=0; i<1000; i++ ){
					//	if( !sendBuffer( (m_random.GetNext() >> 16) ) )		break;
						if( !sendBuffer( i ) )		break;
						if( !recvBuffer() )		break;

						Sleep( 100 );
					}
				}
			}

			INET::WinInet::Cleanup();
			return 0;
		}

		void	ResetBuffer( void ){
		//	for( int i=0; i<BUFF_SIZE/2; i++ ){
		//		sprintf( &m_Buffer[ i ], "%02X", i );
		//	}
			memset( m_Buffer, int('F'), BUFF_SIZE );
		}

		bool	sendBuffer( int nCount ){
			char*	pBuf		= m_Buffer;
			int		pendingSize = BUFF_SIZE;

		//	memset( pBuf, 0, BUFF_SIZE );
			ResetBuffer();
			sprintf(pBuf, "%12d ******", nCount);
			m_Buffer[ strlen(pBuf) ]	= 'A';
			m_Buffer[ BUFF_SIZE - 1 ]	= '\0';

			while( pendingSize > 0 ){
				int	nRet = m_sock.Send( pBuf, BUFF_SIZE );
				if( nRet == 0 ){
					continue;
				}
				else
				if( nRet > 0 ){
					pendingSize -= nRet;
					pBuf		+= nRet;
				}
				else{
					return false;
				}
			}

			DBG::TRACE("Send:%s", m_Buffer);
			return true;
		}

		bool	recvBuffer( void ){
			char*	pBuf		= m_Buffer;
			int		pendingSize = BUFF_SIZE;

		//	memset( pBuf, 0, BUFF_SIZE );
			ResetBuffer();

			while( pendingSize > 0 ){
				int	nRet = m_sock.Recv( pBuf, BUFF_SIZE );
				if( nRet == 0 ){
					continue;
				}
				else
				if( nRet >  0 ){
					pendingSize	-= nRet;
					pBuf		+= nRet;
				}
				else{
					return false;
				}
			}

			DBG::TRACE("Recv:%s", m_Buffer);
			return true;
		}
	};
	/**********************************************************************************
	 *
	 *	int	::send(SOCKET s, const char* buf, int len, int flags);
	 *
	 */
	void	EntryOneModule( PCSTR lpModuleName, PROC pOldMethod, PROC pNewMethod, HMODULE hModule )
	{
		ULONG	uSize	= 0;
		PIMAGE_IMPORT_DESCRIPTOR	pImportDesc	= (PIMAGE_IMPORT_DESCRIPTOR)::ImageDirectoryEntryToData( hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &uSize );
		if( pImportDesc )
		{
			while( pImportDesc->Name )
			{
				PSTR	pModName	= (PSTR)( (BYTE*)hModule + pImportDesc->Name );
				DBG::TRACE("\t%s",pModName);
				if( ::lstrcmpiA(pModName, lpModuleName) == 0 )
				{
					break;
				}

				pImportDesc++;
			}

			if( pImportDesc->Name )
			{
				PIMAGE_THUNK_DATA	pThunk	= (PIMAGE_THUNK_DATA)( (BYTE*)hModule + pImportDesc->FirstThunk );

				while( pThunk->u1.Function )
				{
					PROC*	pMethod	= (PROC*)(&pThunk->u1.Function);
					if( *pMethod == pOldMethod )
					{
						DWORD	dwDummy	= 0;
						::VirtualProtect( pMethod, sizeof(pMethod), PAGE_EXECUTE_READWRITE, &dwDummy );
						::WriteProcessMemory( ::GetCurrentProcess(), pMethod, &pNewMethod, sizeof(pNewMethod), NULL );
						DBG::TRACE( "Replase.\n" );
						return;
					}

					pThunk++;
				}
			}
		}
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	void	EntryAllModule( PCSTR lpModuleName, PROC pOldMethod, PROC pNewMethod )
	{
		HANDLE	hSnapshot	= ::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetCurrentProcessId() );
		if( hSnapshot != INVALID_HANDLE_VALUE )
		{
			MODULEENTRY32	entry;
			entry.dwSize	= sizeof(entry);

			BOOL	bResult	= ::Module32First( hSnapshot, &entry );
			while( bResult )
			{
				DBG::TRACE( entry.szModule );
				EntryOneModule( lpModuleName, pOldMethod, pNewMethod, entry.hModule );
				bResult	= ::Module32Next( hSnapshot, &entry );
			}

			::CloseHandle( hSnapshot );
		}
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	typedef int	(WINAPI *HOOKSEND)(SOCKET,const char*,int,int);
	/**********************************************************************************
	 *
	 *
	 *
	 */
	int	WINAPI	HookSend( SOCKET s, const char* buf, int len, int flags )
	{
		PROC	pMethod	= ::GetProcAddress( GetModuleHandleA("ws2_32.dll"), "send" );
		if( pMethod )
		{
			return ((HOOKSEND)pMethod)( s, buf, len, flags );
		}
		return SOCKET_ERROR;
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	void		EntryHook( void )
	{
		PROC	pMethod	= NULL;

		pMethod	= ::GetProcAddress( GetModuleHandleA("ws2_32.dll"), "send" );
		if( pMethod )
		{
			EntryAllModule( "ws2_32.dll", pMethod, (PROC)HookSend );
		}
	}
}//

