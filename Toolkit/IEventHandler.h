#pragma once

/**********************************************************************************
 */
class IEventHandler {
public:
	enum EnumType {
		OP_READ	= 0, OP_WRITE, OP_ACCEPT, OP_CLOSE, OP_CONNECT
	};

	virtual int		Preform( EnumType nType, CBuffer* pBuffer = NULL ){
		switch( nType ){
		case OP_READ:
			OnRead( pBuffer );
			break;

		case OP_WRITE:
			OnWrite( pBuffer );
			break;

		case OP_ACCEPT:
			OnAccept( pBuffer );
			break;

		case OP_CLOSE:
			OnClose();
			break;

		case OP_CONNECT:
			OnConnect( pBuffer );
			break;

		default:
			return -1;
		}
		return 1;
	}

	virtual int		OnRead( LPVOID )	{ return 0; }
	virtual int		OnWrite( LPVOID )	{ return 0; }
	virtual int		OnAccept( LPVOID )	{ return 0; }
	virtual int		OnConnect( LPVOID )	{ return 0; }
	virtual void	OnClose( void )		{ return; }
};
