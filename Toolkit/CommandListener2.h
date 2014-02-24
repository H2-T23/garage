#pragma once

#include <map>

#define	NON_COPYABLE(CLASS)		private:CLASS& operator=(CLASS&);

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, typename HANDLER = LRESULT (CLASS::*)(WPARAM, LPARAM), typename MAP_HANDLER = std::map<DWORD, HANDLER> >
class TMessageListener {
	NON_COPYABLE(TMessageListener);
private:
	CLASS*			pTarget;
	MAP_HANDLER		mapHandler;

protected:
	HANDLER		Find( DWORD dwID ){
		MAP_HANDLER::iterator it = mapHandler.find( dwID );
		if( it != mapHandler.end() ){
			return it->second;
		}
		return (HANDLER)NULL;
	}

public:
	void		Initialize( CLASS* pClass ){
		pTarget	= pClass;
	}

	void		Register( DWORD dwID, HANDLER lpMethod ){
		mapHandler[ dwID ]	= lpMethod;
	}

	void		Unregister( DWORD dwID ){
		mapHandler[ dwID ]	= NULL;
	}

	LRESULT		Dispach( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		HANDLER	lpMethod = Find( uMsg );
		if( lpMethod ){
			return (pTarget->*lpMethod)(wParam, lParam);
		}
		return 0;
	}
};

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS>
class TCommandListener	: public TMessageListener<CLASS, void (CLASS::*)( void )> {
	NON_COPYABLE(TCommandListener);
public:
	void	Dispach( WPARAM wParam, LPARAM lParam ){
		HANDLER	lpMethod = Find( wParam & 0xFFFF );
		if( lpMethod ){
			(pTarget->*lpMethod)();
		}
	}
};

//template<class CLASS>
//class TNotifyListener	 : public TMessageListener<CLASS, void (CLASS::*)( void )> {
//	NON_COPYABLE(TCommandListener);
//};
