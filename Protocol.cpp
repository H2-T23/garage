// Protocol.cpp

/***********************************************************************************
 *
 *
 */
class CProtocolPacket {
private:
	enum { MAXIMUM_PACKET_LENGTH = 1500 };

	struct Region {
		int		offset;
		int		length;
	};
	
	Region		m_header;
	Region		m_body;
	Region		m_trailer;

	char		m_buffer[ MAXIMUM_PACKET_LENGTH ];

public:
	CProtocolPacket( int nBodyLength, int nBodyOffset )
		: m_header.length(0)
		, m_trailer.length(0)
		, m_body.offset(nBodyOffset)
		, m_body.length(nBodyLenght)
	{}

	void	AddHeader( int length ){
		m_body.Offste	+= m_header.length;
		m_body.length	+= m_header.length;

		m_header.length	= lenght;
		m_header.offset	= m_body.offset - length;
	}

	void	AddTrailer( int length ){
		m_body.lenght	+= m_trailer.length;

		m_trailer.offset	= (m_body.offset + m_body.length);
		m_trailer.length	= length;
	}

	void	ExtractHeader( int length ){
		m_header.offset	= m_body.offset;
		m_header.length	= length;
		
		m_body.offset	+= length;
		m_body.lenght	-= length;
	}

	void	ExtractTrailer( int length ){
		m_body.length	-= length;

		m_trailer.offset	= (m_body.offset + m_body.length);
		m_trailer.length	= length;
	}

public:
	char*	GetHeader( void ) const {
		return &m_buffer[ m_header.offset ];
	}

	char*	GetBody( void ) const {
		return &m_buffer[ m_body.offset ];
	}

	char*	GetTrailer( void ) const {
		return &m_buffer[ m_trailer.offset ];
	}

	int		GetLength( void ) const {
		return(m_header.length + m_body.length + m_trailer.length);
	}
};

/***********************************************************************************
 *
 *
 */
class IProtocolLayer {
private:
	IProtocolLayer*		m_pLowerLayer;
	IProtocolLayer*		m_pUpperLayer;

public:
	IProtocolLayer( void ) : m_pLowerLayer(NULL), m_pUpperLayer(NULL) {}

public:
	virtual void	Transmit( CProtocolPacket* )		= 0;
	virtual void	HandleReceive( CProtocolPacket* )	= 0;

public:
	void			SetLowerLayer( IProtocolLayer* pLayer )	{ m_pLowerLayer	= pLayer;	}
	IProtocolLayer*	GetLowerLayer( void ) const				{ return m_pLowerLayer ;	}

	void			SetUpperLayer( IProtocolLayer* pLayer )	{ m_pUpperLayer	= pLayer;	}
	IProtocolLayer*	GetUpperLayer( void ) const				{ return m_pUpperLayer;		}
};


/***********************************************************************************
 *
 *
 */
class CProtoclStack {
private:
	IProtocolLayer*		m_pHighestLayer;
	IProtocolLayer*		m_pLowestLayer;

public:
	enum Placement { TCP, ABOVE, BELOW };

public:
	void	HandleTransmit( CProtocolPacket* pPacket );
	void	HandleReceive( CProtocolPacket* pPacket );

	void	AddLayer( IProtocolLayer* pLayer, Placement placement=TCP, IProtocolLayer* pExistingLayer = NULL );
	void	RemoveLayer( IProtocolLayer* pLayer );

public:
	CProtocolStack();
};



/***********************************************************************************
 *
 *
 */
class CDatalinkLayer : public IProtocolLayer {
private:
	TransmitProtocolHandler		m_TransmitProtocolHandler;
	ReceiveProtocolHandler		m_ReciveProtocolHandler;	

public:
	CDatalinkLayer( void ) : m_TransmitProtocolHandler(this), m_ReciveProtocolHandler(this) {}

	void		Transmit( CProtocolPacket* pPacket ){
		m_TransmitProtocolHandler.HandleTransmitRequest( (Datagram*)pPacket );
	}

	void		HandleReceive( CProtocolPacket* pPacket ){
		m_ReceiveProtocolHandler.HandleReceivedPacket( (Datagram*)pPacket );
	}
};
