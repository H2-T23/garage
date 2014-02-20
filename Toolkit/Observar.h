#pragma once

#include <vector>
#include <algorithm>

/**********************************************************************************
 *
 *
 *
 */
class IObservable {
public:
	virtual void		Notify( void )	= 0;
};

/**********************************************************************************
 *
 *
 *
 */
class CSubject {
protected:
	typedef std::vector<IObservable*>	Observer;
	Observer	m_list;

public:
	void		add( IObservable* pObservable ){
		m_list.push_back( pObservable );
	}

	void		remove( IObservable* pObservable ){
		Observer::iterator it = std::remove( m_list.begin(), m_list.end(), pObservable );
		m_list.erase( it, m_list.end() );
	}

	void		NotifyAll( void ){
		for( Observer::iterator it=m_list.begin(); it!=m_list.end(); it++ ){
			(*it)->Notify();
		}
	}
};

