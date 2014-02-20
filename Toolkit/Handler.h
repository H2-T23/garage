#pragma once

#include <map>

#define	NON_COPYABLE(CLASS)		private:CLASS& operator=(CLASS&);

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, typename HANDLER = LRESULT (CLASS::*)(WPARAM, LPARAM), typename MAP_HANDLER = std::map<DWORD, HANDLER> >
class TMessageHandler {
	NON_COPYABLE(TMessageHandler);
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

	void		Register( DWORD dwID, HANDLER lpHandler ){
		mapHandler[ dwID ]	= lpHandler;
	}

	void		Unregister( DWORD dwID ){
		mapHandler[ dwID ]	= NULL;
	}

public:
	LRESULT		Dispach( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		HANDLER	lpHandler = Find( uMsg );
		if( lpHandler ){
			return (pTarget->*lpHandler)(wParam, lParam);
		}
		return 0;
	}
};

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, typename HANDLER = void (CLASS::*)(void), typename MAP_HANDLER = std::map<DWORD, HANDLER> >
class TCommandHandler {
	NON_COPYABLE(TCommandHandler);
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

	void		Register( DWORD dwID, HANDLER lpHandler ){
		mapHandler[ dwID ]	= lpHandler;
	}

	void		Register( DWORD dwID, DWORD dwNotify, HANDLER lpHandler ){
		Register( MAKELONG(dwID,dwNotify), lpHandler );
	}

	void		Unregister( DWORD dwID ){
		mapHandler[ dwID ]	= NULL;
	}

public:
	void	Dispach( UINT uID, UINT nCodeNotify ){
		HANDLER	lpHandler = Find( MAKELONG(uID, nCodeNotify) );
		if( lpHandler ){
			(pTarget->*lpHandler)();
		}
	}
};

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, typename HANDLER = void (CLASS::*)(LPNMHDR), typename MAP_HANDLER = std::map<DWORD, HANDLER> >
class TNotifyHandler	 {
	NON_COPYABLE(TNotifyHandler);
private:
	typedef std::map<DWORD, MAP_HANDLER>		MAP_CTRLID;

	CLASS*			pTarget;
	MAP_CTRLID		mapCtrlID;

protected:
	HANDLER		Find( DWORD dwCode, DWORD dwID ){
		MAP_CTRLID::iterator itID = mapCtrlID.find( dwID );
		if( itID != mapCtrlID.end() ){
			MAP_HANDLER::iterator	itCode	= itID->second.find( dwCode );
			if( itCode != itID->second.end() ){
				return itCode->second;
			}
		}
		return (HANDLER)NULL;
	}

public:
	void	Initialize( CLASS* p ){
		pTarget	= p;
	}

	void	Register( DWORD dwCode, DWORD dwID, HANDLER lpHandler ){
		mapCtrlID[ dwID ][ dwCode ]	= lpHandler;
	}

	void	Unregister( DWORD dwCode, DWORD dwID ){
		mapCtrlID[ dwID ][ dwCode ]	= NULL;
	}

public:
	void	Dispach( WPARAM wParam, LPARAM lParam ){
		LPNMHDR lpNmhdr = (LPNMHDR)lParam;
		if( !lpNmhdr )
			return;

		HANDLER	lpHandler = Find( lpNmhdr->code, lpNmhdr->idFrom );
		if( lpHandler ){
			(pTarget->*lpHandler)( lpNmhdr );
		}
	}
};

/**********************************************************************************
 * Example: TNotifyListener
 *
 *
	class ExampleNotifyHandler {
	private:
		const unsigned IDC_BUTTON;
		TNotifyHandler<ExampleNotifyHandler>		notify;

	public:
		ExampleNotifyHandler( void ) : IDC_BUTTON(1101) {}

		void	Initialize( void ){
			notify.Initialize( this );
			notify.Register( NM_CLICK, IDC_BUTTON, &ExampleNotifyHandler::OnNmClick );
		}

		void	OnNmClick( LPNMHDR lpNmhdr ){
			notify.Unregister( NM_CLICK, IDC_BUTTON );
		}

		void	OnNotify( WPARAM wParam, LPARAM lParam ){
			notify.Dispach(wParam, lParam);
		}
	};
 */
