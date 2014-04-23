#pragma once
#include "MessageStructure.h"

#include <vector>
#include <list>
#include <map>

#include "Generic.h"
#include "Thread.h"


using namespace GENERIC;
using namespace MT;

class CMessagePool : public TSingleton<CMessagePool> {
public:
	enum {
		_MAX_		= (int)0xFFFF,
	};

	typedef std::list<CMessageHeader>		LIST;
	typedef std::vector<CMessageHeader>		ARRAY;

protected:
	CMutex					m_Mutex;
//	LIST					m_list;

	static int						Idx;
	static CMessageHeader			MessageHeader[_MAX_] ;

//	void		CopyList( LIST& list ){
//		CSingleLock	lock(&m_Mutex);
//		list.swap(m_list);
//		m_list.clear();
//	}

	void		CopyArray( ARRAY& ary ){
		CSingleLock	lock(&m_Mutex);
		ary.clear();
	//	LIST::const_iterator it	= m_list.begin();
	//	for( ; it!=m_list.end(); it++ ){
	//		ary.push_back( *it );
	//	}
	//	m_list.clear();
		for( int i=0; i<Idx; i++ ){
			ary.push_back( MessageHeader[ i ] );
			MessageHeader[ i ].Clear();
		}
		Idx = 0;
	}

public:
	void		Append( CMessageHeader* pMsg ){
		CSingleLock	lock(&m_Mutex);
	//	m_list.push_back( *pMsg );

		if( Idx >= _MAX_ )
			Idx = 0;
		MessageHeader[Idx++]	= *pMsg;
	}

	void		RemoveAll( void ){
		CSingleLock	lock(&m_Mutex);
	//	m_list.clear();
		Idx = 0;
		for( int i=0; i<_MAX_; i++ ){
			MessageHeader[i].Clear();
		}
	}

	void		GetSnapshot( ARRAY& ary ){
#if 0
		LIST	list(0);
		CopyList( list );
		int sz = list.size();
		if( sz > 0 )
		{
			ary.clear();
		//	ary.resize( sz );
			LIST::iterator	it = list.begin();
			for( int i=0; it!=list.end(); it++, i++ )
			{
			//	ary[ i ]	= *it;
			//	ary[ i ]	= list[ i ];
				ary.push_back( *it );
			}
			list.clear();
		}
#else
		CopyArray( ary );
#endif
	}
};

int						CMessagePool::Idx = 0;
CMessageHeader			CMessagePool::MessageHeader[_MAX_] ;
