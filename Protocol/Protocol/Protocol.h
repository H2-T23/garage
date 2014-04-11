#pragma once

/**********************************************************************************
 */
class CBuffer : public std::string {};
//typedef std::string		CBuffer;
/**********************************************************************************
 */
class IEventHandler {
public:
	enum EnumType {
		OP_READ	= 0, OP_WRITE, OP_ACCEPT, OP_CLOSE, OP_CONNECT
	};

	virtual int		Preform( EnumType nType, CBuffer* pBuffer ){
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
			OnClose( pBuffer );
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
	virtual int		OnClose( LPVOID )	{ return 0; }
	virtual int		OnConnect( LPVOID )	{ return 0; }
};