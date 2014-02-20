#pragma once

#include "Sync.h"


/**********************************************************************************
 *
 *
 *
 */
class CLog {
};
/**********************************************************************************
 *
 *
 *
 */
class CLogManager : public TSingleton<CLogManager> {
protected:
	CCriticalSection			m_cs;

public:
	typedef std::vector<CLog*>	ListLog;

	void	Startup( void ){
	}

	void	Cleanup( void ){
	}

	void	Append( CLog* pLog ){
	}

	void	Snapshot( ListLog& logs ){
	}
};

