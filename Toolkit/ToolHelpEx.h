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

		class CModuleEntry;
		class CThreadEntry;
		class CProcessEntry;

		#if 0
		typedef	CModuleEntry					MODULE;
		typedef CThreadEntry					THREAD;
		typedef CProcessEntry					PROCESS;
		#else
		typedef	MODULEENTRY32					MODULE;
		typedef THREADENTRY32					THREAD;
		typedef PROCESSENTRY32					PROCESS;
		#endif

		typedef	std::vector<MODULEENTRY32>		MODULE_LIST;
		typedef std::vector<THREADENTRY32>		THREAD_LIST;
		typedef std::map<DWORD, CProcessEntry>	PROCESS_LIST;

		template<class CLASS, class BASECLASS>
		class TEntry : public BASECLASS {
		protected:
			void	Copy(const BASECLASS& base){
				if(this!=&base){
					memcpy(this, &base, sizeof(BASECLASS));
				}
			}
		public:
			TEntry(void){}
			TEntry(const CLASS& obj){
				Copy((BASECLASS)obj);
			}
			TEntry(const BASECLASS& base){
				Copy(base);
			}

			CLASS&		operator=(const CLASS& obj){
				Copy((BASECLASS)obj);
				return(CLASS)(*this);
			}
			BASECLASS&	operator=(const BASECLASS& base){
				Copy(base);
				return (BASECLASS)(*this);
			}

			operator BASECLASS() { return(BASECLASS)(*this); }

			virtual TString		ToString( void ) const	= 0;
		};

		class CModuleEntry	: public TEntry<CModuleEntry,MODULEENTRY32> {
		public:
			TString		ToString( void ) const {
				TString str;
				str.Format(_T("$s(%s)"), szModule, szExePath);
				return str;
			}
		};

		class CThreadEntry	: public TEntry<CThreadEntry,THREADENTRY32> {
		public:
			TString		ToString( void ) const {
				TString	str;
				str.Format(_T("0x%08X"), th32ThreadID);
				return str;
			}
		};

		class CProcessEntry	: public TEntry<CProcessEntry,PROCESSENTRY32> {
		public:
			MODULE_LIST				Modules;
			THREAD_LIST				Threads;

			TString		ToString( void ) const {
				TString	str;
				str.Format(_T("0x%08X %d 0x%08X %d"), th32ProcessID,  cntThreads, th32ParentProcessID, pcPriClassBase);
				return str;
			}
		};

		typedef std::vector<PROCESSENTRY32>		PS_ENTRYS;
		typedef	PS_ENTRYS::const_iterator		ITERATOR;


		class CProcessEntrys {
		public:
			PROCESS_LIST		m_Entrys;

			void	Append( PROCESSENTRY32& ps ){
				m_Entrys.insert( std::make_pair(ps.th32ProcessID, ps) );
			}
			void	Append( MODULEENTRY32& module ){
				m_Entrys[ module.th32ProcessID ].Modules.push_back( module );
			}
			void	Append( THREADENTRY32& thread ){
				m_Entrys[ thread.th32OwnerProcessID ].Threads.push_back( thread );
			}
		};

		CProcessEntrys		m_ProcessEntrys;

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

				//	m_ProcessEntrys.Append( PsEntry );

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
					m_ProcessEntrys.Append( MdEntry );

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
						m_ProcessEntrys.Append( ThEntry );
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
