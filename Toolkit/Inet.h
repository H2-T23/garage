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
	 *
	 *
	 *
	 */
	class CSocket : public THandle<CSocket,SOCKET,INVALID_SOCKET> {
	public:
		CSocket( void ) : SUPER() {}
		CSocket( const CSocket& sock ) : SUPER(sock) {}
		/*explicit*/ CSocket( SOCKET sock ) : SUPER(sock) {}

		inline bool	IsValid( void ) const {
			return(GetHandle()!=INVALID_SOCKET);
		}

		virtual BOOL	Create( int nAf, int nType, int nProtocol ){
			if( IsValid() ){
				return FALSE;
			}

			m_handle = socket( nAf, nType, nProtocol );

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
			return ::bind(m_handle, pName, nNameLen);
		}

		int		Listen( int nBackLog ){
			return ::listen(m_handle, nBackLog);
		}

		SOCKET	Accept( sockaddr* pAddr, int& nAddrLen ){
			return ::accept(m_handle, pAddr, &nAddrLen);
		}

		int		Ioctl( long lCmd, ULONG& argp ){
			return ::ioctlsocket(m_handle, lCmd, &argp);
		}
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CListenSocket : public CSocket, public TNonCopyable<CListenSocket> {
	public:
		const int		m_AF;
		const int		m_TYPE;
		SOCKADDR_IN		Addr;

		CListenSocket( void ) : CSocket(), m_AF(AF_INET), m_TYPE(SOCK_STREAM) {}

		BOOL		Create( int nPort ){
			if( !CSocket::Create(m_AF, m_TYPE, 0) ){
				return FALSE;
			}

			Addr.sin_family				= m_AF;
			Addr.sin_port				= htons(nPort);
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

		CSocket		Accept( SOCKADDR* pAddr, int nLen ){
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