#include "stdafx.h"
#include "systemInfo.h"

extern "C" DWORD		VERSION	= 0x00AA;

CSystemInfo::CSystemInfo( void )
{
}


CSystemInfo&	CSystemInfo::Instance( void )
{
	static CSystemInfo	info;
	return info;
}

void	CSystemInfo::SetVersion( DWORD dwVersion )
{
	m_dwVersion	= dwVersion;
}

DWORD	CSystemInfo::GetVersion( void ) const 
{
	return m_dwVersion;
}
