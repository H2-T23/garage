#include "stdafx.h"
#include "ColorControl.h"



BOOL ColorInfoCollection::Find( DWORD key ) const
{
	MAP::const_iterator it = items.find( key );
	if( it != items.end() ){
		return TRUE;
	}
	return FALSE;
}

VOID ColorInfoCollection::Set(DWORD key, COLORINFO& clr)
{
	COLORINFO* pColorInfo = Get( key );
	if( pColorInfo ){
		*pColorInfo = clr;
	}
	items.insert( PAIR(key,clr) );
}

COLORINFO*	ColorInfoCollection::Get(DWORD key)
{
	ITERATOR it = items.find( key );
	if( it != items.end() ){
		return &(it->second);				
	}
	return NULL;
}

VOID ColorInfoCollection::Remove(DWORD key)
{
	ITERATOR it = items.find( key );
	if( it != items.end() ){
		items.erase( it );
	}
}

VOID ColorInfoCollection::RemoveAll(void)
{
	items.clear();
}
