#pragma once

#include <stdio.h>
#include <time.h>

class CMessageHeader {
protected:
	void	Copy( const CMessageHeader& obj ){
		if( this != &obj ){
			ID		= obj.ID;
			tm		= obj.tm;
			memcpy(subject, obj.subject, sizeof(subject));
			szBody	= obj.szBody;
			pBody	= obj.pBody;
		}
	}

public:
	unsigned		ID;
	time_t			tm;
	char			subject[256];
	unsigned		szBody;
	void*			pBody;

	CMessageHeader( void ) 
		: ID(0), tm(0), szBody(0), pBody(NULL)
	{
	}

	explicit CMessageHeader( unsigned id ) 
		: ID(id), tm(0), szBody(0), pBody(NULL) 
	{
	}

	CMessageHeader( const CMessageHeader& obj ) 
	{
		Copy( obj );
	}

	CMessageHeader&	operator=( const CMessageHeader& obj ){
		Copy( obj );
		return(*this);
	}

	void	SetCurrentTime( void ){
		time( &tm );
	}

	void	GetCurrentTime( std::string& str ){
		struct tm* p = localtime( &tm );
		char buf[256];
		sprintf(buf,"%02d/%02d/%02d %02d:%02d:%02d\0"	
			, 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday
			, p->tm_hour, p->tm_min, p->tm_sec );
										
	}

	int		Size( void ) const {
		return sizeof(*this);
	}

	void	ToNetworkByteorder( void ){
	}

	void	ToHostByteorder( void ){
	}

	bool	IsBlank( void ) const {
		return ( ID || tm ) ? false : true ;
	}

	void	Clear( void ){
		memset(this, 0x0, sizeof(*this));
	}
};
