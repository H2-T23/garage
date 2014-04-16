#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>

#include "Generic.h"
#include "TString.h"

namespace INET {
	using namespace GENERIC;

	/**********************************************************************************
	 *
	 *
	 *
	 */
	namespace WinInet {
		bool	Startup( WORD wMajor = 2, WORD wMinor = 2 ){
			WORD	wVersion	= MAKEWORD(wMajor,wMinor);
			WSADATA	wsaData;
			if( ::WSAStartup(wVersion, &wsaData) != 0 )
				return false;
			return true;
		}

		int		Cleanup( void ){
			return ::WSACleanup();
		}
	}
	/**********************************************************************************
	 *	ADDRINFO
	 *	    int                 ai_flags;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
	 *	    int                 ai_family;      // PF_xxx
	 *	    int                 ai_socktype;    // SOCK_xxx
	 *	    int                 ai_protocol;    // 0 or IPPROTO_xxx for IPv4 and IPv6
	 *	    size_t              ai_addrlen;     // Length of ai_addr
	 *	    char *              ai_canonname;   // Canonical name for nodename
	 *	    __field_bcount(ai_addrlen) struct sockaddr *   ai_addr;        // Binary address
	 *	    struct addrinfo *   ai_next;        // Next structure in linked list
	 */
	class CAddressInfo : public ADDRINFO {
	protected:
		LPADDRINFO			m_lpAddrInfo;

		inline bool	IsValid( void ) const {
			return(m_lpAddrInfo!=NULL);
		}

		void	Free( void ){
			if( IsValid() )
				freeaddrinfo( m_lpAddrInfo );
			m_lpAddrInfo	= NULL;
		}

	public:
		CAddressInfo( void ) : m_lpAddrInfo(NULL) {
			ZeroMemory(this, sizeof(ADDRINFO));
		}

		CAddressInfo( int family, int socktype, int protocol, int flags = 0 ) : m_lpAddrInfo(NULL) {
			ZeroMemory(this, sizeof(ADDRINFO));
			Set(family, socktype, protocol, flags);
		}

		~CAddressInfo( void ) {
			Free();
		}

		void		Set( int family, int socktype, int protocol, int flags ){
			ai_family		= family;
			ai_socktype		= socktype;
			ai_protocol		= protocol;
			ai_flags		= flags;
		}

		void			Flags( int flags ) { ai_flags = flags; }
		void			Family( int family ){ ai_family = family; }
		void			SockType( int socktype ){ ai_socktype = socktype; }
		void			Protocol( int protocol ){ ai_protocol = protocol; }

		int				Flags( void ){ return(m_lpAddrInfo->ai_flags); }
		int				Family( void ){ return(m_lpAddrInfo->ai_family); }
		int				SockType( void ){ return(m_lpAddrInfo->ai_socktype); }
		int				Protocol( void ){ return(m_lpAddrInfo->ai_protocol); }
		LPSOCKADDR		SockAddr( void ){ return(m_lpAddrInfo->ai_addr); }
		int				AddrLen( void ){ return(m_lpAddrInfo->ai_addrlen); }
		LPADDRINFO		AddrInfo( void ){ return(m_lpAddrInfo); }

	public:
		BOOL	GetAddressInfo( LPCSTR lpszHostname, LPCSTR lpszPort = NULL ){
			Free();
			return(::getaddrinfo(lpszHostname, lpszPort, (LPADDRINFO)this, &m_lpAddrInfo) == 0 ? TRUE : FALSE);
		}

		BOOL	GetAddressInfo( LPCSTR lpszHostname, int nPort ){
			AString		str;
			str.Format("%d", nPort);

			LPCSTR	lpPort	= (LPCSTR)str;
			return GetAddressInfo(lpszHostname, lpPort);
		}

		int		ToString( TString& str ){
			str.Empty();
			if( !IsValid() )
				return 0;

			TCHAR	tmp[256];

			str.resize( 256 );
			LPTSTR lpResult		= (LPTSTR)str.data();
			int		szCapacity	= str.capacity();
			int		szMaxSize	= str.max_size();
			DWORD	dwSize		= 0;
			::WSAAddressToString( m_lpAddrInfo->ai_addr, (DWORD)m_lpAddrInfo->ai_addrlen, NULL, tmp, &dwSize );
			return str.Length();
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CSocket : public THandle<CSocket,SOCKET,INVALID_SOCKET> {
	public:
		SOCKET&		m_sock;
		CSocket( void ) : SUPER(), m_sock(SUPER::m_hHandle) {}
		CSocket( const CSocket& sock ) : SUPER(sock), m_sock(SUPER::m_hHandle) {}
		/*explicit*/ CSocket( SOCKET sock ) : SUPER(sock), m_sock(SUPER::m_hHandle) {}

		CSocket( int nAf, int nType, int nProtocol ) : SUPER(), m_sock(SUPER::m_hHandle) {
			Create(nAf, nType, nProtocol);
		}

		inline bool	IsInvalid( void ) const {
			return(m_sock==INVALID_SOCKET);
		}
		inline bool	IsValid( void ) const {
			return(m_sock!=INVALID_SOCKET);
		}

		virtual BOOL	Create( int nAf, int nType, int nProtocol ){
			if( IsValid() ){
				return FALSE;
			}

			m_hHandle = socket( nAf, nType, nProtocol );

			return(IsValid() ? TRUE : FALSE);
		}

		virtual void	Close(){
			if( IsValid() ){
				::closesocket( GetHandle() );
				SetHandle( INVALID_SOCKET );
			}
		}

	public:
		int		Bind( const sockaddr* pName, int nNameLen ){
			return ::bind(m_hHandle, pName, nNameLen);
		}

		int		Listen( int nBackLog ){
			return ::listen(m_hHandle, nBackLog);
		}

		SOCKET	Accept( sockaddr* pAddr, int& nAddrLen ){
			return ::accept(m_hHandle, pAddr, &nAddrLen);
		}

		int		Ioctl( long lCmd, ULONG& argp ){
			return ::ioctlsocket(m_hHandle, lCmd, &argp);
		}

		int		Connect( const sockaddr* lpName, int nNameLen ){
			return connect(m_hHandle, lpName, nNameLen);
		}

		int		Recv( LPVOID lpData, int nDataSize, int nFlags = 0 ){
			return ::recv(m_hHandle, (char*)lpData, nDataSize, nFlags );
		}

		int		Send( LPVOID lpData, int nDataSize, int nFlags = 0 ){
			return ::send(m_hHandle, (char*)lpData, nDataSize, nFlags);
		}

		int		AsyncSelect( HWND hWnd, UINT uMsg, LONG lEvent ){
			return ::WSAAsyncSelect(m_hHandle, hWnd, uMsg, lEvent);
		}

		int		Shutdown( int nHow ){
			return ::shutdown(m_hHandle, nHow);
		}

		int		SetOption( int nLevel, int nOptName, const char* pOptVal, int nOptLen ){
			return ::setsockopt(m_hHandle, nLevel, nOptName, pOptVal, nOptLen);
		}

		int		WSARecv( LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine ){
			return ::WSARecv( m_hHandle, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine );
		}

		int		WSASend( LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine ){
			return ::WSASend( m_hHandle, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine );
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CTCPSocket : public CSocket {
	public:
		BOOL		Create( void ){
			return CSocket::Create(AF_INET, SOCK_STREAM, 0);
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CListenSocket : public CSocket, public TNonCopyable<CListenSocket> {
	public:
		int				m_Af;
		int				m_Type;
		int				m_Protocol;
		int				m_Port;

		SOCKADDR_IN		Addr;

		CListenSocket( void )
			: CSocket(), m_Af(AF_INET), m_Type(SOCK_STREAM), m_Protocol(0), m_Port(0) {
		}

		BOOL	Create( int nAf, int nType, int nProtocol, int nPort ){
			m_Af		= nAf;
			m_Type		= nType;
			m_Protocol	= nProtocol;
			m_Port		= nPort;
			return CListenSocket::Create( m_Port );
		}

		BOOL	Create( int nPort ){
			m_Port	= nPort;

			if( !CSocket::Create(m_Af, m_Type, m_Protocol) ){
				return FALSE;
			}

			Addr.sin_family				= m_Af;
			Addr.sin_port				= htons(m_Port);
			Addr.sin_addr.S_un.S_addr	= INADDR_ANY;

			if( Bind((const sockaddr*)&Addr, sizeof(Addr)) == SOCKET_ERROR ){
				Close();
				return FALSE;
			}

			if( Listen(1) == SOCKET_ERROR ){
				Close();
				return FALSE;
			}

			return TRUE;
		}

		CSocket		Accept( LPSOCKADDR pAddr, int& nLen ){
			CSocket	sock	= CSocket::Accept(pAddr, nLen);
			return sock;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CAddInfo {
	private:
		CAddInfo( const CAddInfo& );
		CAddInfo&	operator=( const CAddInfo& );

	protected:
		LPADDRINFO		m_lpAddrInfo;

		inline BOOL		IsValid( void ) const {
			return(m_lpAddrInfo!=NULL);
		}

	public:
		CAddInfo( void ) : m_lpAddrInfo(NULL) {
		}

		BOOL	Get( const CHAR* lpHost, const CHAR* lpService, int nAF = AF_INET, int nType = SOCK_STREAM, int nFlags = AI_PASSIVE ){
			Free();

			ADDRINFO		AddrInfo;
			ZeroMemory(&AddrInfo, sizeof(AddrInfo));
			AddrInfo.ai_family			= nAF;
			AddrInfo.ai_socktype		= nType;
			AddrInfo.ai_flags			= nFlags;

			if( ::getaddrinfo(lpHost, lpService, &AddrInfo, &m_lpAddrInfo) != 0 ){
				return FALSE;
			}
			return TRUE;
		}

		void	Free( void ){
			if( !IsValid() )
				return;

			::freeaddrinfo( m_lpAddrInfo );
			m_lpAddrInfo	= NULL;
		}

		TString		IPAddress( void ){
			TString str;
			str.empty();

			if( !IsValid() )
				return str;

			struct in_addr	addr;
			addr.S_un	= ((struct sockaddr_in*)(m_lpAddrInfo->ai_addr))->sin_addr.S_un;

			str.Format( _T("%s"), inet_ntoa( addr ));
			return str;
		}
	};
}