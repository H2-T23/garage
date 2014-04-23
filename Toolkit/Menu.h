#pragma once

class CMenu {
public:
	HMENU		m_hMenu;

	CMenu( void ) : m_hMenu(NULL) {
	}

	virtual BOOL		Create( void ){
		m_hMenu	= ::CreateMenu();
		return(m_hMenu?TRUE:FALSE);
	}

	BOOL		Insert( LPTSTR lpszItemName, WORD wID, HMENU hMenuSub = NULL ){
		MENUITEMINFO	miim = {0};
		miim.cbSize			= sizeof(MENUITEMINFO);
		miim.fMask			= MIIM_ID | MIIM_TYPE;
		miim.wID			= wID;

		if( lpszItemName == NULL )
		{
			miim.fType		= MFT_SEPARATOR;
		}
		else
		{
			miim.fType		= MFT_STRING;
			miim.dwTypeData	= lpszItemName;
		}

		if( hMenuSub )
		{
			miim.fMask		|= MIIM_SUBMENU;
			miim.hSubMenu	= hMenuSub;
		}

		return InsertMenuItem(m_hMenu, wID, FALSE, &miim);
	}

	operator HMENU () { return m_hMenu; }
};

class CPopupMenu : public CMenu {
public:
	BOOL		Create( void ){
		m_hMenu	= ::CreatePopupMenu();
		return(m_hMenu?TRUE:FALSE);
	}
};