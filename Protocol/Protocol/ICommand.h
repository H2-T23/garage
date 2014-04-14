#pragma once

/**********************************************************************************
 */
class ICommand {
private:
	ICommand( const ICommand& );
	ICommand&	operator=( const ICommand& );

protected:
	const std::string		m_strCommandName;

	ICommand( void );

public:
	ICommand( const std::string& strCmdName ) : m_strCommandName(strCmdName) {}
	ICommand( const char* pCmdName ) : m_strCommandName(pCmdName) {}

	const char*			GetCommandName( void ) const {
		return m_strCommandName.c_str();
	}

	virtual int			Execute( LPARAM lParam, WPARAM wParam = NULL ) = 0;
};
/**********************************************************************************
 */
class CCommandList {
private:
	CCommandList( const CCommandList& );
	CCommandList&	operator=( const CCommandList& );

protected:
	typedef std::map<const std::string, ICommand*>		MAP;
	typedef std::pair<const std::string, ICommand*>		PAIR;
	typedef	MAP::iterator								ITERATOR;

	MAP			m_map;

public:
	CCommandList( void ){}
	~CCommandList( void ){
		Reset();
	}

	void		Register( ICommand* pCmd ){
		if( pCmd ){
			m_map.insert( PAIR(pCmd->GetCommandName(), pCmd) );
		}
	}

	void		Unregister( const std::string& strCmdName ){
		ITERATOR it = m_map.find( strCmdName );
		if( it != m_map.end() ){
			delete it->second;
			m_map.erase( it );
		}
	}

	void		Reset( void ){
		ITERATOR it = m_map.begin();
		for( ; it != m_map.end(); it++ ){
			delete it->second;
		}
		m_map.clear();
	}

	ICommand*	Find( const std::string& strCmdName ){
		ITERATOR	it = m_map.find( strCmdName );
		if( it != m_map.end() ){
			return it->second;
		}
		return NULL;
	}
};
/**********************************************************************************
 */
class CTestCommandList {
public:
	class CGetCommand : public ICommand {
	public:
		CGetCommand( void ) : ICommand("GetCommand") {
		}

		int		Execute( LPARAM, WPARAM ){
			TRACE("%s.\n",GetCommandName());
			return 0;
		}
	};

	class CPutCommand : public ICommand {
	public:
		CPutCommand( void ) : ICommand("PutCommand") {}

		int		Execute( LPARAM, WPARAM ){
			TRACE("%s.\n",GetCommandName());
			return 0;
		}
	};


	class CPostCommand : public ICommand {
	public:
		CPostCommand( void ) : ICommand("PostCommand") {}

		int		Execute( LPARAM, WPARAM ){
			TRACE("%s.\n",GetCommandName());
			return 0;
		}
	};

	CTestCommandList( void )
	{
		CCommandList	list;
		list.Register( new CGetCommand );
		list.Register( new CPostCommand );
		list.Register( new CPutCommand );

		ICommand*	pCmd = list.Find( "PostCommand" );
		if( pCmd ){
			pCmd->Execute( NULL );
		}
	}
};
