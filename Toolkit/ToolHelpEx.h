#pragma once
#include <vector>
#include <map>
#include <utility>
#include <TlHelp32.h>

#include "TString.h"
#include "DBG.h"


namespace TOOLHELP {

	class CSnapshotProcess {
	public:
		typedef PROCESSENTRY32			PROCESS;
		typedef	MODULEENTRY32			MODULE;
		typedef THREADENTRY32			THREAD;

		class CProcessEntry : public PROCESS {
		public:
			TString		ToString( void ){
			}
		};

		class CModuleEntry

		typedef	std::vector<MODULE>		MODULE_LIST;
		typedef std::vector<THREAD>		THREAD_LIST;
		typedef struct : public PROCESS {
			MODULE_LIST			Modules;
			THREAD_LIST			Threads;
		} PROCESS_ENTRY ;
		typedef std::vector<PROCESS>			PS_ENTRYS;
		typedef	PS_ENTRYS::const_iterator		ITERATOR;

		typedef std::map<DWORD, PROCESS_ENTRY>	PROCESS_LIST;

		class CProcessEntrys {
		public:
			PROCESS_LIST		m_Entrys;

			void	Entry( PROCESS& ps ){
				m_Entrys.insert( make_pair<ps.th32ProcessID, ps> );
			}

			void	Entry( DWORD dwPID, MODULE& module ){
				m_Entrys[ dwPID ].Modules.push_back( module );
			}

			void	Entry( DWORD dwPID, THREAD& thread ){
				m_Entrys[ dwPID ].Threads.push_back( thread );
			}

			void	Append( MODULE& module ){
				m_Entrys[ module.th32ProcessID ].Modules.push_back( module );
			}

			void	Append( THREAD& thread ){
				m_Entrys[ thread.th32OwnerProcessID ].Threads.push_back( thread );
			}
		};

		CProcessEntrys		m_ProcessEntry;

	protected:
		PS_ENTRYS		m_PsEntrys;
		ITERATOR		m_It;

	public:
		ITERATOR		First( void ) {
			return(m_It = m_PsEntrys.begin());
		}

		ITERATOR		Next( void ) {
			return(++m_It);
		}

		BOOL			hasNext( void ) const {
			return(m_It!=m_PsEntrys.end());
		}

	public:
		BOOL	Snapshot( void ){
			HANDLE	hPsSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
			if( hPsSnap == INVALID_HANDLE_VALUE ){
				return FALSE;
			}

			PROCESSENTRY32	PsEntry;
			PsEntry.dwSize	= sizeof(PROCESSENTRY32);

			if( ::Process32First( hPsSnap, &PsEntry ) )
			{
				do{
					DBG::TRACE(_T("[%s]"), PsEntry.szExeFile);
					DBG::TRACE(_T("   Process ID        : 0x%08X")	, PsEntry.th32ProcessID);
					DBG::TRACE(_T("   Thread count      : %d")		, PsEntry.cntThreads);
					DBG::TRACE(_T("   Parent process ID : 0x%08X")	, PsEntry.th32ParentProcessID);
					DBG::TRACE(_T("   Priority byte     : %d")		, PsEntry.pcPriClassBase);

					PIDModules( PsEntry.th32ProcessID );
					PIDThreads( PsEntry.th32ProcessID );

					m_PsEntrys.push_back( PsEntry );
					
				}while( ::Process32Next( hPsSnap, &PsEntry ) );
			}
			::CloseHandle( hPsSnap );

			return TRUE;
		}

		BOOL	PIDModules( const DWORD dwProcessID ){
			HANDLE	hMdSnap	= ::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwProcessID );
			if( hMdSnap == INVALID_HANDLE_VALUE ){
				return FALSE;
			}

			MODULEENTRY32	MdEntry;
			MdEntry.dwSize	= sizeof(MODULEENTRY32);

			if( ::Module32First( hMdSnap, &MdEntry ) )
			{
				do{
					DBG::TRACE(_T("[0x%08X:%s](%s)"), MdEntry.th32ModuleID, MdEntry.szModule, MdEntry.szExePath);
				}while( ::Module32Next( hMdSnap, &MdEntry ) );
			}
			::CloseHandle( hMdSnap );
			
			return TRUE;
		}

		BOOL	PIDThreads( const DWORD dwProcessID ){
			HANDLE	hThSnap	= ::CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
			if( hThSnap == INVALID_HANDLE_VALUE ){
				return FALSE;
			}

			THREADENTRY32	ThEntry;
			ThEntry.dwSize	= sizeof(THREADENTRY32);

			if( ::Thread32First( hThSnap, &ThEntry ) )
			{
				do{
					if( dwProcessID == ThEntry.th32OwnerProcessID )
					{
						DBG::TRACE(_T("Thread ID: 0x%08X"), ThEntry.th32ThreadID);
					}
				}while( ::Thread32Next( hThSnap, &ThEntry ) );
			}
			::CloseHandle( hThSnap );

			return TRUE;
		}
	};


	void	UseToolhelp( void )
	{
		CSnapshotProcess	snapPs;
		for( CSnapshotProcess::ITERATOR entry = snapPs.First(); snapPs.hasNext(); entry = snapPs.Next() ){
			DBG::TRACE(_T("[%s]"), entry->szExeFile);
			DBG::TRACE(_T("   Process ID        : 0x%08X")	, entry->th32ProcessID);
			DBG::TRACE(_T("   Thread count      : %d")		, entry->cntThreads);
			DBG::TRACE(_T("   Parent process ID : 0x%08X")	, entry->th32ParentProcessID);
			DBG::TRACE(_T("   Priority byte     : %d")		, entry->pcPriClassBase);
		}

		snapPs.Snapshot();
		for( CSnapshotProcess::ITERATOR entry = snapPs.First(); snapPs.hasNext(); entry = snapPs.Next() ){
			DBG::TRACE(_T("[%s]"), entry->szExeFile);
			DBG::TRACE(_T("   Process ID        : 0x%08X")	, entry->th32ProcessID);
			DBG::TRACE(_T("   Thread count      : %d")		, entry->cntThreads);
			DBG::TRACE(_T("   Parent process ID : 0x%08X")	, entry->th32ParentProcessID);
			DBG::TRACE(_T("   Priority byte     : %d")		, entry->pcPriClassBase);
		}
	}

}// end of "TOOLHELP".
