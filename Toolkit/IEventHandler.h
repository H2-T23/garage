#pragma once

enum {
	OP_READ	= 0, OP_WRITE, OP_ACCEPT, OP_CLOSE, OP_CONNECT
};

/**********************************************************************************
 */
class IEventHandler {
public:
	enum EnumType {
		OP_READ	= 0, OP_WRITE, OP_ACCEPT, OP_CLOSE, OP_CONNECT
	};

	virtual int		Preform( EnumType nType, LPVOID* pParam = NULL ){
		switch( nType ){
		case OP_READ:
			OnRead( pParam );
			break;

		case OP_WRITE:
			OnWrite( pParam );
			break;

		case OP_ACCEPT:
			OnAccept( pParam );
			break;

		case OP_CLOSE:
			OnClose();
			break;

		case OP_CONNECT:
			OnConnect( pParam );
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
