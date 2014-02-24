#pragma once

#include <map>

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS>
class TNotifyListener {
private:
	typedef void (CLASS::*ON_NOTIFY)( LPNMHDR );

	typedef std::map<DWORD, ON_NOTIFY>		Handler;
	typedef std::map<DWORD, Handler>		MapHandler;

	CLASS*			pTarget;
	MapHandler		mapHandler;

protected:
	ON_NOTIFY	Find( DWORD dwCode, DWORD dwID ){
		MapHandler::iterator itID = mapHandler.find( dwID );
		if( itID != mapHandler.end() ){
			Handler::iterator	itCode	= itID->second.find( dwCode );
			if( itCode != itID->second.end() ){
				return itCode->second;
			}
		}
		return (ON_NOTIFY)NULL;
	}

public:
	void	Initialize( CLASS* p ){
		pTarget	= p;
	}

	void	Register( DWORD dwCode, DWORD dwID, ON_NOTIFY lpMethod ){
		mapHandler[ dwID ][ dwCode ]	= lpMethod;
	}

	void	Unregister( DWORD dwCode, DWORD dwID ){
		mapHandler[ dwID ][ dwCode ]	= NULL;
	}

	void	Dispach( WPARAM wParam, LPARAM lParam ){
		LPNMHDR lpNmhdr = (LPNMHDR)lParam;
		if( !lpNmhdr )
			return;

		ON_NOTIFY	lpMethod = Find( lpNmhdr->code, lpNmhdr->idFrom );
		if( lpMethod ){
			(pTarget->*lpMethod)( lpNmhdr );
		}
	}
};

/**********************************************************************************
 * Example: TNotifyListener
 *
 *
	class ExampleNotifyListener {
	private:
		const unsigned IDC_BUTTON;
		TNotifyListener<ExampleNotifyListener>		notify;

	public:
		CUseNotifyListener( void ) : IDC_BUTTON(1101) {}

		void	Initialize( void ){
			notify.Initialize( this );
			notify.Register( NM_CLICK, IDC_BUTTON, &ExampleNotifyListener::OnNmClick );
		}

		void	OnNmClick( LPNMHDR lpNmhdr ){
			notify.Unregister( NM_CLICK, IDC_BUTTON );
		}

		void	OnNotify( WPARAM wParam, LPARAM lParam ){
			notify.OnNotify(wParam, lParam);
		}
	};
 */
