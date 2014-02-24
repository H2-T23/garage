#pragma once

#include <map>
#include <algorithm>

/**********************************************************************************
 *
 *
 *
 */
template<typename DerivedClass, typename BaseClass>
class TCommandListener : public BaseClass {
private:
	typedef void (DerivedClass::*CommandHandler)(int);
	std::map<int, CommandHandler>	mapCmd;

public:
	void	AddCommandListener( int id, CommandHandler handler ){
		mapCmd[ id ] = handler;
	}

	void	RemoveCommandListener( int id ){
		mapCmd.erase( id );
	}

protected:
	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		if( mapCmd.end() != mapCmd.find( int(uID) ) ){
			(((DerivedClass*)this)->*mapCmd[ int(uID) ])( int(uID) );
		}
	}
};
