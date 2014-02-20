#pragma once

#include <string>
#include <vector>

namespace BASE64 {
	using namespace std;

	typedef unsigned char				Byte;
	typedef std::vector<Byte>			ByteArray;

	bool	Encode( const ByteArray& src, string& dst );
	bool	Decode( const string& src, ByteArray& dst );


	const string&		Table( void ){
		static const string table("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
		return(table);
	}

	bool	Encode( const ByteArray& src, string& dst ){

		string	wk;

		size_t	i = 0;
		for( ; i < src.size(); i++ ){
			switch( (i % 3) ){
			case 0:
				{
					wk.push_back( Table()[ ((src[i] & 0xFC) >> 2) ] );
					if( (i + 1) == src.size() ){
						wk.push_back( Table()[ ((src[i] & 0x03) << 4) ] );
						wk.push_back( '=' );
						wk.push_back( '=' );
					}
				}
				break;

			case 1:
				{
					wk.push_back( Table()[ ((src[i - 1] & 0x03) << 4) | ((src[i + 0] & 0xF0) >> 4) ] );
					if( (i + 1) == src.size() ){
						wk.push_back( Table()[ ((src[i] & 0x0F) << 2) ] );
						wk.push_back( '=' );
					}
				}
				break;

			case 2:
				{
					wk.push_back( Table()[ ((src[i - 1] & 0x0F) << 2) | ((src[i + 0] & 0xC0) >> 6) ] );
					wk.push_back( Table()[ src[ i ] & 0x3F ] );
				}
				break;
			}
		}

		dst.swap( wk );
		return true;
	}

	bool	Decode( const string& src, ByteArray& dst ){
		if( src.size() & 0x03 )
			return false;

		ByteArray	wk;

		size_t	i = 0;
		for( ; i<src.size(); i+=4 ){
			if( src[ i + 0 ] == '=' )
				return false;

			if( src[ i + 1 ] == '=' )
				return false;

			if( src[ i + 2 ] == '=' ){
				const string::size_type	s1	= Table().find( src[ i + 0 ] );
				const string::size_type	s2	= Table().find( src[ i + 1 ] );

				if( string::npos == s1 || string::npos == s2 )
					return false;

				wk.push_back( static_cast<Byte>( ((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4) ) );

				break;
			}
			else
			if( src[ i + 3 ] == '=' ){
				const string::size_type	s1	= Table().find( src[ i + 0 ] );
				const string::size_type	s2	= Table().find( src[ i + 1 ] );
				const string::size_type	s3	= Table().find( src[ i + 2 ] );

				if( string::npos == s1 || string::npos == s2 || string::npos == s3 )
					return false;

				wk.push_back( static_cast<Byte>( ((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4) ) );
				wk.push_back( static_cast<Byte>( ((s2 & 0x0F) << 4) | ((s3 & 0x3C) >> 2) ) );

				break;
			}
			else{
				const string::size_type	s1	= Table().find( src[ i + 0 ] );
				const string::size_type	s2	= Table().find( src[ i + 1 ] );
				const string::size_type	s3	= Table().find( src[ i + 2 ] );
				const string::size_type	s4	= Table().find( src[ i + 3 ] );

				if( string::npos == s1 || string::npos == s2 || string::npos == s3 || string::npos == s4 )
					return false;

				wk.push_back( static_cast<Byte>( ((s1 & 0x3F) << 2) | ((s2 & 0x30) >> 4) ) );
				wk.push_back( static_cast<Byte>( ((s2 & 0x0F) << 4) | ((s3 & 0x3C) >> 2) ) );
				wk.push_back( static_cast<Byte>( ((s2 & 0x03) << 6) | ((s3 & 0x3F) >> 0) ) );

				break;
			}
		}

		dst.swap( wk );

		return true;
	}

	void	Test( void ){
		ByteArray	v1, v2;

		v1.push_back( 0xDE );
		v1.push_back( 0xAD );
		v1.push_back( 0xDE );
		v1.push_back( 0xEF );

		string	str;
		Encode( v1, str );
		cout << str << endl;

		Decode( str, v2 );
		ByteArray::const_iterator	it	= v2.begin();
		for( ; it!=v2.end(); it++ ){
			cout << hex << static_cast<int>( *it );
		}
	}
}