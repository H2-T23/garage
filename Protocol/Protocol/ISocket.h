#pragma once

/**********************************************************************************
 */
class ISocket {
public:
	class SocketOption {
	public:
		int		nAf, nType, nProtocol;

		SocketOption( int af = 0, int type = 0, int protocol = 0 )
			: nAf(af), nType(type), nProtocol(protocol) {
		}
	};

public:
	SOCKET				m_sock;
	SocketOption		m_sockOption;

	int					Af( void ) const		{ return(m_sockOption.nAf); }
	int					Type( void ) const		{ return(m_sockOption.nType); }
	int					Protocol( void ) const	{ return(m_sockOption.nProtocol); }

public:
	ISocket( void ) : m_sock(INVALID_SOCKET) {}

public:
	inline bool			IsInvalid( void ) const {
		return(m_sock==INVALID_SOCKET);
	}
	inline bool			IsValid( void ) const {
		return !IsInvalid();
	}

public:
	virtual bool		Create( SocketOption& Option )				= 0;
	virtual int			Close( void )								= 0;
	virtual int			Shutdown( void )							= 0;

	virtual int			Recv( CBuffer* pBuffer )					= 0;
	virtual int			Send( CBuffer* pBuffer )					= 0;

	virtual int			Accept( LPSOCKADDR lpAddr, int& nLen )		= 0;
};