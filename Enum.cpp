// Enum.cpp 
#include <iostream>

#include <iterator>
#include <vector>
#include <map>
#include <string>

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

namespace ITERATOR {
	
	class Iterable {
	public:
		virtual void		First( void ) = 0;
		virtual void		Next( void ) = 0;
		virtual bool		hasNext( void ) const = 0;
	};
	
}// End of "ITERATOR".

namespace ENUM1{

	/******************************************************
	 * class CEnum.
	 *
	 *
 	 */
	class CEnum : public ITERATOR::Iterable {
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
		
		bool			hasNext( void ) const {
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

	/******************************************************
	 *
	 *
	 *
 	 */
	void	main( void )
	{
		cout << __FUNCTION__ << endl;

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
				
		cout << "[while loop]" << endl;
		Enum.First();
		while( Enum.hasNext() ){
			cout << Enum.ID() << " : " << Enum.Name() << endl;
			Enum.Next();
		}

		cout << "[find item]" << endl;
		cout << Enum.ID( "ID_APPLICATION" ) << " : " << Enum.Name( ID_APPLICATION ) << endl;

		cout << "[erase item] " << ID_003 << "," << ID_004  << endl;
		Enum.Erase( ID_003 );
		Enum.Erase( ID_004 );		
		
		cout << "[for loop]" << endl;
		for( Enum.First(); Enum.hasNext(); Enum.Next() ){
			cout << Enum.ID() << " : " << Enum.Name() << endl;
		}
	}

}// End of "ENUM1"

namespace ENUM2{
	
	/******************************************************
	 *
	 *
	 *
 	 */
	class CEnum : public ITERATOR::Iterable {
	private:
		map<string,int>						m_map;
		map<string,int>::const_iterator		m_it;
		
		int			currId;
		
	public:
		int			Insert( int id, const char* pName ){
			currId = id;
			m_map[ pName ] = currId;
			return(currId);
		}
		
		int			Append( const char* pName ){
			return(Insert(++currId,pName));
		}
		
		void		Erase( int id ){
			m_map.erase( Find(id) );
		}
	
	public:
		void		First( void ){
			m_it = m_map.begin();
		}
		
		void		Next( void ){
			m_it++;
		}
		
		bool		hasNext( void ) const {
			return(m_it!=m_map.end());
		}
	
	public:
		int			ID( void ) const {
			return(m_it->second);
		}
		
		int			ID( const char* pName ) const {
			return(Find(pName));
		}
		
		const char*	Name( void ) const {
			return(m_it->first.c_str());
		}
		
		const char*	Name( int id ) const {
			return(Find(id));
		}
		
	protected:
		int			Find( const char* pName ) const {
			map<string,int>::const_iterator it = m_map.find( pName );
			if( it != m_map.end() ){
				return(it->second);
			}
			return(NULL);
		}
		
		const char*	Find( int id ) const {
			map<string,int>::const_iterator it = m_map.begin();
			for( ; it!=m_map.end(); it++ ){
				if( it->second == id ){
					return(it->first.c_str());
				}
			}
			return(NULL);
		}
	};

	/******************************************************
	 *
	 *
	 *
 	 */
	void	main( void )
	{
		cout << __FUNCTION__ << endl;

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
		
		cout << "[while loop]" << endl;
		Enum.First();
		while( Enum.hasNext() ){
			cout << Enum.ID() << " : " << Enum.Name() << endl;
			Enum.Next();
		}
		
		cout << "[find item]" << endl;
		cout << Enum.ID( "ID_APPLICATION" ) << " : " << Enum.Name( ID_APPLICATION ) << endl;

		cout << "[erase item]" << endl;
		Enum.Erase( ID_003 );
		Enum.Erase( ID_004 );
		
		cout << "[for loop]" << endl;
		for( Enum.First(); Enum.hasNext(); Enum.Next() ){
			cout << Enum.ID() << " : " << Enum.Name() << endl;
		}
	}
}// End of "ENUM2".

namespace ENUM3 {
	
	/******************************************************
	 *
	 *
	 *
 	 */
	class CEnum;
	class CEnumIterator;

	/******************************************************
	 *
	 *
	 *
 	 */
	class CEnumIterator : public ITERATOR::Iterable
	{
		friend class CEnum;
	private:
		CEnumIterator( void );
		CEnumIterator( CEnumIterator& );
		CEnumIterator& operator=( CEnumIterator& );

	private:
		CEnum&			rEnum;
		CEnumIterator( CEnum& Enum );
				
	public:
		void		First( void );
		void		Next( void );
		bool		hasNext( void ) const;
		int			ID( void ) const;
		string&		Name( void ) const;
	};

	/******************************************************
	 *
	 *
	 *
 	 */
	class CEnum 
	{
		friend class CEnumIterator;
	private:
		std::map<int,std::string>					m;
		std::map<int,std::string>::iterator			it;
		
		int	currId;
		CEnumIterator		EnumIterator;
		
	public:
		CEnumIterator&		Iterator( void );
		
	public:
		CEnum( void );
		
		int					Insert( int, string );
		int					Append( string );
		void				Erase( int );


	protected:
		string&				Find( int id ) const {
			std::map<int,string>::const_iterator	it = m.find( id );
		}
	};
	
	
	/******************************************************
	 *
 	 */
	CEnum::CEnum( void ) : EnumIterator(*this), currId(0) {
	}
	
	/******************************************************
	 *
 	 */
	CEnumIterator&	CEnum::Iterator( void ){
		return(EnumIterator);
	}
	
	/******************************************************
	 *
 	 */
	int		CEnum::Insert( int id, string str ){
		currId = id;
		m[ currId ]	= str;
		return(currId);
	}
	
	/******************************************************
	 *
 	 */
	int		CEnum::Append( string str ){
		return(Insert(++currId,str));
	}
	
	/******************************************************
	 *
 	 */
	void	CEnum::Erase( int id ){
		m.erase( id );
	}
		
	/******************************************************
	 *
 	 */
	CEnumIterator::CEnumIterator( CEnum& Enum ) : rEnum(Enum) {
	}
	
	/******************************************************
	 *
 	 */
	void	CEnumIterator::First( void ){
		rEnum.it = rEnum.m.begin();
	}
	
	/******************************************************
	 *
 	 */
	void	CEnumIterator::Next( void ){
		(rEnum.it)++;
	}
	
	/******************************************************
	 *
 	 */
	bool	CEnumIterator::hasNext( void ) const {
		return(rEnum.it!=rEnum.m.end());
	}
	
	/******************************************************
	 *
 	 */
	int		CEnumIterator::ID( void ) const {
		return(rEnum.it->first);
	}
		
	/******************************************************
	 *
 	 */
	string&	CEnumIterator::Name( void ) const {
		return(rEnum.it->second);
	}
	
	/******************************************************
	 *
	 *
	 *
 	 */
	void	main( void )
	{
		cout << __FUNCTION__ << endl;

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

		CEnumIterator& it = Enum.Iterator();
		cout << "[while loop]" << endl;
		it.First();
		while( it.hasNext() ){
			cout << it.ID() << " : " << it.Name() << endl;
			it.Next();
		}
		
		cout << "[erase] : " << ID_001 << endl;
		Enum.Erase( ID_001 );
		
		cout << "[for loop]" << endl;
		for( it.First(); it.hasNext(); it.Next() ){
			cout << it.ID() << " : " << it.Name() << endl;
		}	
	}
	
}// End of "ENUM3".

void main( void )
{
	ENUM3::main();
}
