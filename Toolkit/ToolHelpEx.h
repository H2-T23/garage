#pragma once
#include <vector>
#include <map>
#include <utility>
#include <TlHelp32.h>

#include "TString.h"
#include "DBG.h"


namespace TOOLHELP {
	/****************************************************************
	 */
	class CModuleEntry;
	class CThreadEntry;
	class CProcessEntry;
	/****************************************************************
	 */
	typedef	MODULEENTRY32						MODULE;
	typedef THREADENTRY32						THREAD;
	typedef PROCESSENTRY32						PROCESS;
	/****************************************************************
	 */
	typedef	std::vector<CModuleEntry>			MODULE_LIST;
	typedef std::vector<CThreadEntry>			THREAD_LIST;
	typedef std::map<DWORD, CProcessEntry*>		PROCESS_LIST;
	typedef PROCESS_LIST::const_iterator		ITERATOR;
	/****************************************************************
	 */
	template<class CLASS, typename STRUCT, unsigned SIZE=sizeof(STRUCT)>
	class TStructEx : public STRUCT {
	protected:
		typedef TStructEx<CLASS, STRUCT>	SUPER;

		VOID	Zero( void ){
			memset((STRUCT*)this, 0x00, SIZE);
		}
		BOOL	Copy(const STRUCT& obj){
			if(this!=&obj){
				memcpy((STRUCT*)this, &obj, SIZE);
				return TRUE;
			}
			return FALSE;
		}
		BOOL	Copy(const CLASS& obj){
			return Copy( (STRUCT)obj );
		}
	public:
		TStructEx(void)					{ Zero();		}
		TStructEx(const MODULE& obj)	{ Copy(obj);	}
		TStructEx(const CLASS& obj)		{ Copy(obj);	}

		STRUCT&	operator=(const STRUCT& obj){
			Copy(obj);
			return (STRUCT)(*this);
		}
		STRUCT&	operator=(const CLASS& obj){
			Copy(obj);
			return (STRUCT)(*this);
		}
	};
	/****************************************************************
	 */
	class CModuleEntry			: public MODULE {
	public:
		CModuleEntry(const MODULE& obj){
			if(this!=&obj){
				memcpy((MODULE*)this, &obj, sizeof(MODULE));
			}
		}

		CModuleEntry&	operator=(const MODULE& obj){
			if(this!=&obj){
				memcpy((MODULE*)this, &obj, sizeof(MODULE));
			}
			return(*this);
		}
	public:
		TString		ToString( void ) const {
			TString	str;
			str.Format(_T("[%s](%s)"), this->szModule, this->szExePath);
			return str;
		}
	};
	/****************************************************************
	 */
	class CThreadEntry			: public THREAD {
	public:
		CThreadEntry(const THREAD& obj){
			if(this!=&obj){
				memcpy((THREAD*)this, &obj, sizeof(THREAD));
			}
		}

		CThreadEntry&	operator=(const THREAD& obj){
			if(this!=&obj){
				memcpy((THREAD*)this, &obj, sizeof(THREAD));
			}
			return(*this);
		}
	public:
		TString		ToString( void ) const {
			TString	str;
			str.Format(_T("0x%08X"), this->th32ThreadID);
			return str;
		}
	};
	/****************************************************************
	 */
	class CProcessEntry			: public PROCESS {
	private:
		CProcessEntry(const CProcessEntry&);
		CProcessEntry&	operator=(const CProcessEntry&);
	public:
		CProcessEntry(const PROCESS& obj){
			memcpy((PROCESS*)this, &obj, sizeof(PROCESS));
		}
	public:
		MODULE_LIST			Modules;
		THREAD_LIST			Threads;

		TString		GetHeaderString( void ) const {
			TString	str;
			//                    123456789012345678901234567890
			str.AppendFormat(_T("[           ExeFile            ]"));
			str.AppendFormat(_T("[   Process ID    ]"));
			str.AppendFormat(_T("[  Thread count   ]"));
			str.AppendFormat(_T("[Parent process ID]"));
			str.AppendFormat(_T("[  Priority byte  ]"));
			return str;
		}
		
		TString		ToString( void ) const {
			TString	str;
			str.AppendFormat(_T(" %30s ")	, this->szExeFile);
			str.AppendFormat(_T(" 0x%16X ")	, this->th32ProcessID);
			str.AppendFormat(_T(" %16d ")	, this->cntThreads);
			str.AppendFormat(_T(" 0x%16X ")	, this->th32ParentProcessID);
			str.AppendFormat(_T(" %16d ")	, this->pcPriClassBase);
			return str;
		}
	};
	/****************************************************************
	 */
	class CSnapshotProcess {
	public:
		/****************************************************************
		 */
		class CProcessEntrys {
		protected:
			PROCESS_LIST		m_Entrys;
			ITERATOR			m_it;

		public:
			CProcessEntry*	Find( DWORD dwPID ) const {
				PROCESS_LIST::const_iterator	it = m_Entrys.find( dwPID );
				if( it != m_Entrys.end() ){
					return it->second;
				}
				return NULL;
			}

			void	Append( const PROCESS& ps ){
				m_Entrys[ ps.th32ProcessID	]	= new CProcessEntry(ps);
			}

			void	Append( const MODULE& module ){
				CProcessEntry*	pEntry	= Find( module.th32ProcessID );
				if( pEntry ){
					pEntry->Modules.push_back( module );
				}
			}

			void	Append( const THREAD& thread ){
				CProcessEntry*	pEntry	= Find( thread.th32OwnerProcessID );
				if( pEntry ){
					pEntry->Threads.push_back( thread );
				}
			}

			void	RemoveAll( void ){
				PROCESS_LIST::iterator	it = m_Entrys.begin();
				for( ; it!=m_Entrys.end(); ){
					delete it->second;
					m_Entrys.erase( it );
				}
			}

			ITERATOR	First( void ){
				m_it = m_Entrys.begin();
				return(m_it);
			}

			ITERATOR	Next( void ){
				return(++m_it);
			}

			bool		hasNext( void ) const {
				return(m_it!=m_Entrys.end());
			}

			~CProcessEntrys( void ){
				RemoveAll();
			}
		};
		/****************************************************************
		 */
		CProcessEntrys		m_ProcessEntrys;
	public:
		/****************************************************************
		 */
		ITERATOR		First( void ) {
			return m_ProcessEntrys.First();
		}

		/****************************************************************
		 */
		ITERATOR		Next( void ) {
			return m_ProcessEntrys.Next();
		}

		/****************************************************************
		 */
		BOOL			hasNext( void ) const {
			return m_ProcessEntrys.hasNext();
		}

	public:
		/****************************************************************
		 */
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

					m_ProcessEntrys.Append( PsEntry );

					PIDModules( PsEntry.th32ProcessID );
					PIDThreads( PsEntry.th32ProcessID );

				}while( ::Process32Next( hPsSnap, &PsEntry ) );
			}
			::CloseHandle( hPsSnap );

			return TRUE;
		}
		/****************************************************************
		 */
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
					DBG::TRACE(_T("[%s](%s)"), MdEntry.szModule, MdEntry.szExePath);
					m_ProcessEntrys.Append( MdEntry );
				}while( ::Module32Next( hMdSnap, &MdEntry ) );
			}
			::CloseHandle( hMdSnap );
			
			return TRUE;
		}
		/****************************************************************
		 */
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
	/****************************************************************
	 */
	void	UseToolhelp( void )
	{
		CSnapshotProcess	snapPs;
		for( PROCESS_LIST::const_iterator entry = snapPs.First(); snapPs.hasNext(); entry = snapPs.Next() ){
			DBG::TRACE( entry->second->ToString() );
		}
		snapPs.Snapshot();
		for(  PROCESS_LIST::const_iterator entry = snapPs.First(); snapPs.hasNext(); entry = snapPs.Next() ){
			if( entry->first == 0 )
				DBG::TRACE( entry->second->GetHeaderString() );
			DBG::TRACE( entry->second->ToString() );

			MODULE_LIST&	modules = entry->second->Modules;
			for( MODULE_LIST::iterator it=modules.begin(); it!=modules.end(); it++ ){
				DBG::TRACE( it->ToString() );
			}
		}
	}
}// end of "TOOLHELP".
