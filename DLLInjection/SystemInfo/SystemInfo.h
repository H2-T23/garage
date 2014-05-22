#pragma once

extern "C" extern DWORD		VERSION;

class CSystemInfo {
private:
	CSystemInfo(void);
	CSystemInfo&	operator=(const CSystemInfo&){}

protected:
	DWORD			m_dwVersion;

public:
	void	SetVersion( DWORD dwVersion );
	DWORD	GetVersion( void ) const;

public:
	static CSystemInfo&		Instance( void );
};
