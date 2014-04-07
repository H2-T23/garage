// ExampleMultiplexing.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <math.h>
#include <sys/timeb.h>

#include "ExampleMultiplexing.h"

#include "Inet.h"
#include "Buffer.h"

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
template<class CLASS>
class TCopyable {
public:
	TCopyable( void ) {
	}
	TCopyable( const CLASS& obj ){
		Copy( obj );
	}
	CLASS&	operator=( const CLASS& obj ){
		Copy( obj );
		return(*this);
	}

	bool	operator==( CLASS& obj ) const {
		return IsEqule(obj);
	}

	bool	operator!=( CLASS& obj ) const {
		return !IsEqule(obj);
	}

protected:
	virtual void	Copy( const CLASS& obj ) = 0;
	virtual bool	IsEqule( CLASS& obj ) const { return false; }; 
};
 */
/**********************************************************************************
 *
 *
 *
template<class CLASS,typename HANDLE_TYPE, HANDLE_TYPE INIT_VALUE = 0>
class THandle : public TCopyable<CLASS> {
public:
	typedef THandle<CLASS,HANDLE_TYPE,INIT_VALUE>	SUPER;

	HANDLE_TYPE			m_hHandle;

	THandle( void ) : m_hHandle(INIT_VALUE) {}

	THandle( HANDLE_TYPE handle ) : m_hHandle(handel) {}

	THandle( const THandle& obj ){
		Copy(obj);
	}

	THandle( const CLASS& obj ){
		Copy(obj);
	}

	THandle&	operator=( const THandle& obj ){
		Copy(obj);
		return(*this);
	}

	CLASS&		operator=( const CLASS& obj ){
		Copy(obj);
		return(*this);
	}

	const HANDLE_TYPE&		GetHandle( void ) const {
		return(m_hHandle);
	}
	
	inline bool			IsInvalid( void ) const {
		return(INIT_VALUE==m_hHandle);
	}

	inline bool			IsValid( void ) const {
		return(!IsInvalid());
	}

	virtual void		Close( void ){
		if( !IsInvalid() )
		//	::CloseHandle( m_hHandle );
			CloseHandle( m_hHandle );
		m_hHandle = INIT_VALUE;
	}

protected:
	virtual void	Copy( const THandle& obj ){
		if( this != &obj ){
			m_hHandle	= obj.m_hHandle;
		}
	}
	virtual void	Copy( const CLASS& obj ){
		if( this != &obj ){
			m_hHandle	= obj.m_hHandle;
		}
	}
	virtual bool	IsEqule( CLASS& obj ) const {
		return(m_hHandle == obj.m_hHandle);
	}

	virtual void	CloseHandle( HANDLE_TYPE handle ) = 0;
};
 */
namespace Multipliexing 
{
	/**********************************************************************************
	 *
	 *
	 *
	class CSocket : public THandle<CSocket,SOCKET,INVALID_SOCKET>
	{
	public:
		SOCKET&				m_sock;
		CSocket( void ) : SUPER(), m_sock(m_hHandle) {
		}

		void	CloseHandle( SOCKET sock ){
			::closesocket( sock );
		}
	};
	 */
	using namespace INET;
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class SESSIONS;
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class IProtocol	{
	public:
		virtual DWORD		HandleEvent( DWORD dwMask )	= 0;
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CSession : public IProtocol {
		friend class SESSIONS;
	public:
		CSocket			m_sock;
		DWORD			m_mask;
		IProtocol*		m_pProtocol;
		bool			m_bDeleteOnClose;
		bool			m_bKeepOpen;

		CPacket			m_packet;

	public:
	private:
		CSession( const CSocket&	sock
				, const DWORD		dwMask
				, IProtocol*		pProtocol		= NULL
				, const bool		bDeleteOnClose	= false 
				, const bool		bKeepOpen		= false
				)
			: m_sock(sock)
			, m_mask(dwMask)
			, m_pProtocol(pProtocol) 
			, m_bDeleteOnClose(bDeleteOnClose)
			, m_bKeepOpen(bKeepOpen)
		{
		}

	public:
		virtual ~CSession( void ){}

	public:
		CSocket			GetSocket( void ) const						{ return(m_sock); }
		void			SetSocket( const CSocket& sock )			{ m_sock = sock; }

		bool			GetKeepOpen( void ) const 					{ return(m_bKeepOpen); }
		void			SetKeepOepn( const bool bKeepOpen = true )	{ m_bKeepOpen = bKeepOpen; }

		DWORD&			Mask( void )								{ return(m_mask); }
		DWORD&			Mask( const DWORD dwMask )					{ return(m_mask=dwMask); }

		virtual void			Close( void ){
			if( !m_sock.IsInvalid() ){
				m_sock.Close();
			}

			if( m_bDeleteOnClose ){
				m_bDeleteOnClose	= false;
				delete this;
			}
		}

	//	virtual unsigned		HandleEvent( unsigned eventType )	= 0;
		DWORD					HandleEvent( DWORD eventType ) {
			if( m_pProtocol )
				return m_pProtocol->HandleEvent( eventType );
			return 0;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class SESSIONS {
	private:
		SESSIONS( void );
		SESSIONS( const SESSIONS& );
		SESSIONS& operator=( const SESSIONS& );

		typedef std::map<SOCKET, CSession*>		MAP;
		typedef	std::pair<SOCKET, CSession*>	PAIR;
		typedef MAP::iterator					ITERATOR;

		static MAP				m_Sessions;
		static ITERATOR			m_It;

	public:
		static size_t			Size( void ){
			return m_Sessions.size();
		}

	public:
		static CSession*		New( const CSocket& sock, DWORD dwMask, IProtocol* pProtocol = NULL ){
			CSession*	pSession	= new CSession(sock, dwMask, pProtocol);
			if( pSession ){
				m_Sessions.insert( PAIR(sock.GetHandle(), pSession) );
				return pSession;
			}
			return NULL;
		}

		static void				Delete( CSocket& sock ){
			CSession* pSession = Find( sock );
			if( pSession ){
				m_Sessions.erase( sock.GetHandle() );
				delete pSession;
			}
		}

		static void				Delete( CSession& session ){
			SESSIONS::Delete( session.m_sock );
		}

		static void				DeleteAll( void ){
		//	std::remove( m_Sessions.begin(), m_Sessions.end() );
		}

		static CSession*		Find( const CSocket& sock ){
			ITERATOR	it	= m_Sessions.find( sock.GetHandle() );
			if( it != m_Sessions.end() ){
				return(it->second);
			}
			return NULL;
		}

	public:
		static CSession*		Begin( void ){
			m_It	= m_Sessions.begin();
			return(m_It->second);
		}

		static CSession*		Next( void ){
			m_It++;
			if( hasNext() ){
				return(m_It->second);
			}
			return NULL;
		}

		static bool				hasNext( void ){
			return(m_It!=m_Sessions.end());
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	SESSIONS::MAP		SESSIONS::m_Sessions;
	SESSIONS::ITERATOR	SESSIONS::m_It	= SESSIONS::m_Sessions.begin();
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CMultipliexing {
	protected:
		typedef std::list<CSession>		SessionList;

		double				m_endTime;
		bool				m_doClear;
		bool				m_inWork;

		double		GetTime( void ) const {
		#define	timeb _timeb
		#define	ftime _ftime
			struct timeb	tbuff;
			ftime(&tbuff);
			return ((double) tbuff.time + ((double)tbuff.millitm / 1000.0) + ((double) tbuff.timezone * 60));
		#undef	timeb
		#undef	ftime
		}

	public:
		enum EventType { ReadableEvent = 1, WritableEvent, Exception = 4, };

	public:
		CMultipliexing( void ){
			m_endTime	= -1.0;
			m_doClear	= false;
			m_inWork	= false;
		}

		~CMultipliexing( void ){
		}

		void		Dispatch( double timeout ){
			m_endTime	= (timeout < 0.0) ? -1.0 : (GetTime() + timeout);
			m_doClear	= false;
			m_inWork	= true;

			while( SESSIONS::Size() > 0 )
			{
				fd_set	inFd, outFd, excFd;
				FD_ZERO(&inFd);
				FD_ZERO(&outFd);
				FD_ZERO(&excFd);

				int	maxFd	= -1;

				CSession* pSession	= NULL;

				pSession	= SESSIONS::Begin();
				while( SESSIONS::hasNext() )
				{
					int	fd	= int(pSession->GetSocket().GetHandle());

					if( pSession->Mask() & ReadableEvent )
						FD_SET(fd, &inFd);

					if( pSession->Mask() & WritableEvent )
						FD_SET(fd, &outFd);

					if( pSession->Mask() & Exception	 )
						FD_SET(fd, &excFd);

					if( pSession->Mask() && (fd > maxFd) )
						maxFd	= fd;

					pSession	= SESSIONS::Next();
				}


				int	nEvents;
				if( timeout < 0.0 )
				{
					nEvents		= select(maxFd+1, &inFd, &outFd, &excFd, NULL);
				}
				else
				{
					struct timeval	tv;
					tv.tv_sec	= (int)floor( timeout );
					tv.tv_usec	= (int)floor( 1000000.0 * (timeout - floor(timeout)) ) % 1000000;
					nEvents		= select(maxFd+1, &inFd, &outFd, &excFd, &tv);
				}

				if( 0 > nEvents )
				{
					m_inWork	= false;
					return;
				}

				pSession	= SESSIONS::Begin();
				while( SESSIONS::hasNext() )
				{
					int	fd	= int(pSession->GetSocket().GetHandle());
					DWORD	dwMask	= (DWORD)(-1);

					if( fd <= maxFd )
					{
						if( FD_ISSET(fd, &inFd) )
							dwMask	&=	pSession->HandleEvent( ReadableEvent );

						if( FD_ISSET(fd, &outFd) )
							dwMask	&=	pSession->HandleEvent( WritableEvent );

						if( FD_ISSET(fd, &excFd) )
							dwMask	&=	pSession->HandleEvent( Exception );

						if( !dwMask )
						{
							SESSIONS::Delete( *pSession );
						}
						else
						{
							pSession->Mask( dwMask );
						}
					}

					pSession	= SESSIONS::Next();
				}


				// 
				if( m_doClear )
				{
				/*
					SessionList	closeList = m_SessionList;
					m_SessionList.clear();

					SessionList::iterator	it = closeList.begin();
					for( ; it!=closeList.end(); it++ ){
						it->GetSocket().Close();
					}
				*/
					SESSIONS::DeleteAll();

					m_doClear	= false;
				}


			}

			m_inWork	= false;
		}

		void		Exit( void ){
			m_endTime	= 0.0;
		}
		
		void		Clear( void ){
			if( m_inWork )
			{
				m_doClear	= true;
			}
			else
			{
			/*
				SessionList	closeList = m_SessionList;
				m_SessionList.clear();
				
				SessionList::iterator	it = closeList.begin();
				for( ; it!=closeList.end(); it++ ){
					it->GetSocket().Close();
				}
			*/
				SESSIONS::DeleteAll();
			}
		}
	};

#if 1
	class CServer : public IProtocol {
	public:
		class Connection : public IProtocol {
		public:
			DWORD		HandleEvent( DWORD dwMask )	{
				return 0;
			}
		};

		CMultipliexing		m_Multipliexing;
		CListenSocket		m_ListenSock;

		bool		Setup( void ){
			if( m_ListenSock.Create( 50001 ) ){
				SESSIONS::New( m_ListenSock, 0xFF, this );
				return true;
			}
			return false;
		}

		bool		Shutdown( void ){
			m_Multipliexing.Clear();
			return true;
		}

		void		Dispatch( double timeout ){
			m_Multipliexing.Dispatch( timeout );
		}

		DWORD		HandleEvent( DWORD mask ){
			SOCKADDR	SockAddr = {0};
			int	nLen				= sizeof(SOCKADDR);
			CSocket	sock	= m_ListenSock.Accept(SockAddr, nLen);

			CSession* pSession	= SESSIONS::New( sock, mask, new Connection );
			if( pSession )
			{
				SESSIONS::Delete( *pSession );
			}
			return 0;
		}
	};

#endif
}// end of "Multipliexing".
/**********************************************************************************
 *
 *
 *
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	unsigned char	buf[128] = {0};
	std::vector<unsigned char>		pack(32,(char)0xFF);
	for( int i=0; i<pack.size(); i++ ){
		pack[ i ] = char(i+1);
	}
#if 0
	char*	p	= &pack.at( 0 );
	int	idx		= pack.size();
	int	nSize	= (pack.size() + 128);
	pack.resize( nSize, (char)0xFF );
	p	= &pack.at( idx );
	memcpy( p, buf, 128 );
#else
	CBuffer		buffer(pack);
	buffer.Append( buf, 128 );
#endif	

	INET::WinInet::Startup();

	Multipliexing::CServer	server;
	if( server.Setup() ){
		server.Dispatch(-1);
		server.Shutdown();
	}

	INET::WinInet::Cleanup();

	return (int)0;
}
