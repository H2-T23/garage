// Enum.cpp 
#include <iostream>

#include <iterator>
#include <vector>
#include <map>
#include <string>

class CEnum {
private:
	std::map<int,std::string>				map;
	std::map<int,std::string>::iterator		it;
	
	int		currId;

public:
	CEnum( void ) : currId(0) {}
	
public:
	void			Insert( int id, const char* pName ){
		currId = id;
		map[ currId ]	= pName;
	}
	
	void			Append( const char* pName ){
		currId ++ ;
		map[ currId ]	= pName;
	}
	
	void			Erase( int id ){
		map.erase( id );
	}
	
public:
	int				ID( const char* pName ) const {
		return(Find(pName));
	}
	
	int				ID( void ) const {
		return(it->first);
	}
	
	const char*		Name( int id ) const {
		return(Find(id));
	}
	
	const char*		Name( void ) const {
		return((it->second).c_str());
	}
	
public:
	void			First( void ){
		it = map.begin();
	}
	
	void			Next( void ){
		it++;
	}
	
	bool			isDone( void ) const {
		return(it!=map.end());
	}
	
	int				Current( void ) const {
		return(it->first);
	}
	
protected:
	const char*		Find( int id ) const {
		std::map<int,std::string>::const_iterator it = map.find( id );
		if( it != map.end() ){
			return (it->second).c_str();
		}
		return NULL;
	}
	
	int				Find( const char* pName ) const {
		std::map<int,std::string>::const_iterator it = map.begin();
		for( ; it!=map.end(); it++ ){
			if( it->second == pName ){
				return(it->first);
			}
		}
		return 0;
	}
};


using namespace std;

enum {
	ID_SYSTEM		= 100	,
	ID_APPLICATION			,
	ID_MODE					,
	
	ID_001			= 1001	,
	ID_002					,
	ID_003					,
	ID_004					,
};

void main( void )
{
	CEnum		Enum;
	Enum.Insert( ID_SYSTEM		, "ID_SYSTEM"		);
	Enum.Insert( ID_APPLICATION	, "ID_APPLICATION"	);
	Enum.Insert( ID_MODE		, "ID_MODE"			);
	Enum.Insert( ID_001			, "ID_001"			);
	Enum.Insert( ID_002			, "ID_002"			);
	Enum.Insert( ID_003			, "ID_003"			);
	Enum.Insert( ID_004			, "ID_004"			);
	Enum.Append( "ID_005" );
	Enum.Append( "ID_006" );
	Enum.Append( "ID_007" );
	Enum.Append( "ID_008" );
	Enum.Append( "ID_009" );
	
	cout << "<for loop>" << endl;
	for( Enum.First(); Enum.isDone(); Enum.Next() ){
		cout << Enum.ID() << " : " << Enum.Name() << endl;
	}
	
	cout << "<find item>" << endl;
	cout << Enum.ID( "ID_APPLICATION" ) << " : " << Enum.Name( ID_APPLICATION ) << endl;

	cout << "<erase item>" << endl;
	Enum.Erase( ID_003 );
	Enum.Erase( ID_004 );
	
	cout << "<while loop>" << endl;
	Enum.First();
	while( Enum.isDone() ){
		cout << Enum.ID() << " : " << Enum.Name() << endl;
		Enum.Next();
	}
}
