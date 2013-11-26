// SnapProcess.cpp 
#include <tchar.h>
#include <windows.h>
#include <tlhelp32.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <memory>


typedef std::vector<PROCESSENTRY32>		VecProcessEntry;
typedef std::auto_ptr<VecProcessEntry>	PVecProcessEntry;

PVecProcessEntry	CreateProcessShapShot(void){
	PVecProcessEntry p( new VecProcessEntry() );

	HANDLE	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapShot != INVALID_HANDLE_VALUE){
		PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
		BOOL bProcess = Process32First(hSnapShot, &pe);
		for( ; bProcess; bProcess=Process32Next(hSnapShot,&pe) ){
			p->push_back( pe );
		}
		CloseHandle( hSnapShot );
	}
	return(p);
}

typedef std::vector<MODULEENTRY32>		VecModuleEntry;
typedef std::auto_ptr<VecModuleEntry>	PVecModuleEntry;

PVecModuleEntry		CreateModuleSnapShot(DWORD dwPID){
	PVecModuleEntry	p( new VecModuleEntry() );

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPID);
	if(hSnapShot != INVALID_HANDLE_VALUE){
		MODULEENTRY32 me = { sizeof(MODULEENTRY32) };
		BOOL bModule = Module32First(hSnapShot, &me);
		for( ; bModule; bModule=Module32Next(hSnapShot,&me) ){
			p->push_back( me );
		}
		CloseHandle(hSnapShot);
	}
	return(p);
}

BOOL	InjectDLL(DWORD dwPID, LPCSTR lpszLibPath){
	printf("%s(%d,%s)\n",__FUNCTION__,dwPID,lpszLibPath);

	BOOL	bResult = FALSE;

	HANDLE					hProcess		= NULL;
	HANDLE					hThread			= NULL;
	LPTHREAD_START_ROUTINE	pLoadLibrary	= NULL;

	LPVOID					pMem			= NULL;	
	DWORD					dwMemSize		= (strlen(lpszLibPath) + 1);
	
	__try{
		puts("OpenProcess");
		hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPID);
		if(hProcess==NULL){
			__leave;
		}

		puts("VirtualAllocEx");		
		pMem = ::VirtualAllocEx(hProcess,NULL,dwMemSize,MEM_COMMIT,PAGE_READWRITE);
		if(pMem==NULL){
			__leave;
		}

		puts("WriteProcessMemory");
		if(!::WriteProcessMemory(hProcess,pMem,(void*)lpszLibPath,dwMemSize,NULL)){
			__leave;
		}

		puts("GetProcAddress");
		pLoadLibrary = (LPTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle("Kernel32"),"LoadLibraryA");
		if(pLoadLibrary==NULL){
			__leave;
		}

		puts("CreateRemoteThread");
		hThread = ::CreateRemoteThread(hProcess,NULL,0,pLoadLibrary,pMem,0,NULL);
		if(hThread==NULL){
			__leave;
		}

		puts("WatiForSingleObject");
		::WaitForSingleObject(hThread,INFINITE);
		
		bResult = TRUE;
				
	}__finally{
		if(hThread)
			CloseHandle(hThread);
		if(pMem)
			VirtualFreeEx(hProcess,pMem,dwMemSize,MEM_RELEASE);
		if(hProcess)
			CloseHandle(hProcess);
	}

	return bResult;
}

BOOL	EjectDLL(DWORD dwPID,LPCSTR lpszLibPath){
	printf("%s(%d,%s)\n",__FUNCTION__,dwPID,lpszLibPath);

	BOOL	bResult = FALSE;
	
	HANDLE					hProcess		= NULL;
	HANDLE					hThread			= NULL;
	LPTHREAD_START_ROUTINE	pFreeLibrary	= NULL;

	__try{
		puts("ModuleEntry");
		PVecModuleEntry	pModuleEntry = CreateModuleSnapShot(dwPID);
		MODULEENTRY32	me;
		BOOL bFound = FALSE;
		for( int i=0; i<pModuleEntry->size(); i++ ){
			if( (bFound = ((strcmp((*pModuleEntry)[i].szModule, lpszLibPath)==0)||(strcmp((*pModuleEntry)[i].szExePath,lpszLibPath)==0))) ){
				me = (*pModuleEntry)[i];
				break;
			}
		}
		if(bFound==FALSE){
			__leave;
		}

		puts("OpenProcess");
		hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPID);
		if(hProcess==NULL){
			__leave;
		}
		
		puts("GetProcAddress");
		pFreeLibrary = (LPTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle("Kernel32"),"FreeLibrary");
		if(pFreeLibrary==NULL){
			__leave;
		}

		puts("CreateRemoteThread");
		hThread = ::CreateRemoteThread(hProcess,NULL,0,pFreeLibrary,me.modBaseAddr,0,NULL);
		if(hThread==NULL){
			__leave;
		}

		::WaitForSingleObject(hThread,INFINITE);


		bResult = TRUE;

	}__finally{
		if(hThread)
			CloseHandle(hThread);
		if(hProcess)
			CloseHandle(hProcess);
	}

	return bResult;
}
	
class CProcessSnapShot {
public:
	typedef std::auto_ptr<CProcessSnapShot>		PSnapShot;

	VecProcessEntry			vecProcessEntry;

private:
	CProcessSnapShot(void){}
	CProcessSnapShot(const CProcessSnapShot&);
	const CProcessSnapShot& operator=(const CProcessSnapShot&);

	void		Enum(void){
		vecProcessEntry.clear();

		HANDLE	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hSnapShot == INVALID_HANDLE_VALUE){
			return;
		}

		PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
		BOOL bProcess = Process32First(hSnapShot, &pe);
		for( ; bProcess; bProcess=Process32Next(hSnapShot,&pe) ){
			printf(	"PROCESSENTRY32.dwSize:              %d\n"
					"PROCESSENTRY32.cntUsage:            %d\n"
					"PROCESSENTRY32.th32ProcessID:       %d\n"
					"PROCESSENTRY32.th32DefaultHeapID:   %d\n"
					"PROCESSENTRY32.th32ModuleID:        %d\n"
					"PROCESSENTRY32.cntThreads:          %d\n"
					"PROCESSENTRY32.th32ParentProcessID: %d\n"
					"PROCESSENTRY32.pcPriClassBase:      %d\n"
					"PROCESSENTRY32.dwFlags:             0x%.8x\n"
					"PROCESSENTRY32.szExeFile:           %s\n\n"
					,pe.dwSize
					,pe.cntUsage
					,pe.th32ProcessID
					,pe.th32DefaultHeapID
					,pe.th32ModuleID
					,pe.cntThreads
					,pe.th32ParentProcessID
					,pe.pcPriClassBase
					,pe.dwFlags
					,pe.szExeFile
					);
			vecProcessEntry.push_back( pe );
		}
		CloseHandle( hSnapShot );
	}
public:
	~CProcessSnapShot(void){
		puts(__FUNCTION__);
	}
public:
	static PSnapShot	Create(void){
		PSnapShot	pSnapShot( new CProcessSnapShot() );
		pSnapShot->Enum();
		return(pSnapShot);
	}
};



int main(int argc,TCHAR** argv)
{
	CProcessSnapShot::PSnapShot pSnapShot = CProcessSnapShot::Create();
	for( int i=0; i<pSnapShot->vecProcessEntry.size(); i++ ){
		printf("%-8d %s\n"
				,pSnapShot->vecProcessEntry[ i ].th32ProcessID
				,pSnapShot->vecProcessEntry[ i ].szExeFile
				);
	}
	

	CHAR szCmd[256];

	char buf[1024];
	while(true){
		printf(">");
		gets(buf);
		if(strcmp("quit",buf)==0||strcmp("q",buf)==0){
			break;
		}
		else
		if(strncmp("process",buf,strlen("process"))==0){
			PVecProcessEntry	pProcessEntry = CreateProcessShapShot();
			for( int i=0; i<pProcessEntry->size(); i++ ){
				printf("%-8d %s\n"
						,(*pProcessEntry)[i].th32ProcessID
						,(*pProcessEntry)[i].szExeFile
						);
			}
		}
		else
		if(strncmp("module",buf,strlen("module"))==0){
			DWORD dwPID=0;
			sscanf(buf,"%s %d",szCmd,&dwPID);

			PVecModuleEntry	pModuleEntry = CreateModuleSnapShot(dwPID);
			for( int i=0; i<pModuleEntry->size(); i++ ){
				printf("%-30s %s\n"
						,(*pModuleEntry)[i].szModule
						,(*pModuleEntry)[i].szExePath
						);
			}
		}
		else
		if((strncmp("inject",buf,strlen("inject"))==0)
		|| (strncmp("eject",buf,strlen("eject"))==0) ){
			puts(buf);
			DWORD dwPID=0;
			CHAR szDLL[256];
			sscanf(buf,"%s %d %s",szCmd,&dwPID,szDLL);
			if(buf[0]=='i')
				InjectDLL(dwPID,szDLL);
			else
				EjectDLL(dwPID,szDLL);
		}
		else{
			puts("Not supported");
		}
	}

	return(0);
}