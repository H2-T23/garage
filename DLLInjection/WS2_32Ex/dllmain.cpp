// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include "APIHook.h"
#include "TString.h"
#include "File.h"

#include <string>

#include "SystemInfo.h"
#include "Trace.h"

#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")

typedef int	(WINAPI* LPSEND)(SOCKET, const char*, int, int);
typedef int	(WINAPI* LPRECV)(SOCKET, const char*, int, int);

extern "C" 
{
	CTxtFile						sendLog(_T("send.log"));
	CTxtFile						recvLog(_T("recv.log"));

	std::basic_string<TCHAR>		strErrMsg;

	static LPSEND	pSendOriginal		= NULL;
	static LPRECV	pRecvOriginal		= NULL;

	static int WINAPI	SendHook(SOCKET s, const char* buf, int len, int flags)
	{
		int	nRet	= 0;
		if( pSendOriginal )
		{
			if( sendLog.IsOpen() )
			{
				AString	strA;
				TString	strT;

				strA.Format("send(%u, %s, %d, %d)\n\0", s, buf, len, flags);
				TString::A2W( strA, strT );
				sendLog.WriteString( strT );
			}
			nRet	= pSendOriginal(s, buf, len, flags);
		}
		return nRet;
	}

	static int WINAPI	RecvHook(SOCKET s, const char* buf, int len, int flags)
	{
		int	nRet	= 0;
		if( pRecvOriginal )
		{
			if( recvLog.IsOpen() )
			{
				AString		strA;
				TString		strT;

				strA.Format("send(%u, %s, %d, %d)\n\0", s, buf, len, flags);
				TString::A2W( strA, strT );
				recvLog.WriteString( strT );
			}
			nRet	= pRecvOriginal(s, buf, len, flags);
		}
		return nRet;
	}

	static BOOL		SendReplace( HMODULE hModule )
	{
		if( hModule )
		{
			pSendOriginal	= (LPSEND)::GetProcAddress(hModule, "send");
			if( pSendOriginal )
			{
				CAPIHook	APIHook;
				return APIHook.Replace( pSendOriginal, &SendHook );
			}
			else
			{
				strErrMsg = _T("SendReplace failed: GetProcAddress().");
			}
		}
		return FALSE;
	}

	static BOOL		RecvReplace( HMODULE hModule )
	{
		if( hModule )
		{
			pRecvOriginal	= (LPRECV)::GetProcAddress(hModule, "recv");
			if( pRecvOriginal )
			{
				CAPIHook	APIHook;
				return APIHook.Replace( pRecvOriginal, &RecvHook );
			}
			else
			{
				strErrMsg = _T("RecvReplace failed: GetProcAddress().");
			}
		}
		return FALSE;
	}

	static BOOL		EntryHook( void )
	{
		HMODULE	hModule	= ::GetModuleHandle(_T("ws2_32.dll"));
		if( hModule )
		{
			if( !SendReplace( hModule ) )
				return FALSE;

			if( !RecvReplace( hModule ) )
				return FALSE;
		}
		else
		{
			strErrMsg = _T("failed: GetModuleHandle().");
		}
		return FALSE;
	}

	static void		LeaveHook( void )
	{
		if( pSendOriginal )
		{
			CAPIHook	APIHook;
			APIHook.Replace( &SendHook, pSendOriginal );
		}
	}


	static void		ImportSection( void )
	{
		DWORD	dwSize	= 0;
		HMODULE	hModule	= ::GetModuleHandle(NULL);

		IMAGE_IMPORT_DESCRIPTOR*	pImport	= 
			(IMAGE_IMPORT_DESCRIPTOR*)::ImageDirectoryEntryToData(hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &dwSize );
		if( pImport )
		{
			IMAGE_IMPORT_DESCRIPTOR*	p = pImport;

			while( p->Name )
			{
				PSTR	pModuleName	= (PSTR)((BYTE*)hModule + p->Name);
				BYTE*	pBase		= (BYTE*)GetModuleHandleA( pModuleName );
				TRACE("%p %s\n", pBase, pModuleName);

				IMAGE_THUNK_DATA*	pIAT	= (IMAGE_THUNK_DATA*)((BYTE*)hModule + p->FirstThunk);
				IMAGE_THUNK_DATA*	pINT	= (IMAGE_THUNK_DATA*)((BYTE*)hModule + p->OriginalFirstThunk);

				while( pIAT->u1.Function )
				{
					if( IMAGE_SNAP_BY_ORDINAL32((DWORD)pINT) )
					{
						TRACE("%u\n", (0x7FFFFFFF) & (DWORD)pINT);
					}
					else
					{
						IMAGE_IMPORT_BY_NAME*	pByName	= (IMAGE_IMPORT_BY_NAME*)(pBase +(DWORD)pINT/*->u1.AddressOfData*/);
						TRACE("%-28s (%x)\n", (char*)&pByName->Name, pByName->Hint);
					}
					
					pIAT++;
					pINT++;
				}

				p++;
			}
		}
	}
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	DWORD		dwVersion	= 0;
	TCHAR		szBuf[MAX_PATH];
	TString		str;

	switch( ul_reason_for_call ){
	case DLL_PROCESS_ATTACH:

		ImportSection();

		dwVersion	= VERSION;//CSystemInfo::Instance().GetVersion();
		str.Format(_T("%08X\n\0"), dwVersion);

		::GetModuleFileName(hModule, szBuf, MAX_PATH);
		str.AppendFormat(_T("%08X: %s\n\0"), hModule, szBuf);

		::GetCurrentDirectory(MAX_PATH, szBuf);
		str.AppendFormat(_T("PATH: %s\n\0"), szBuf);

		OutputDebugString( str );
		MessageBox(NULL, str, _T(""), MB_OK);

		if( EntryHook() ){
			MessageBox(NULL, _T("Entry"), _T("Hook"), MB_OK);
		}else{
			MessageBox(NULL, strErrMsg.c_str(), _T("Hook"), MB_OK);
		}
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		LeaveHook();
		break;
	}
	return TRUE;
}
