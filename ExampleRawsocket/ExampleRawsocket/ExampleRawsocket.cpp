// ExampleRawsocket.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleRawsocket.h"

#include "UseToolkit.h"

#include "Inet.h"

#include <Mstcpip.h>

typedef struct {
	BYTE				version;
	#define	ip_v		version>>4
	#define	ip_hl		version&0x0F
	BYTE				type;
	WORD				length;
	WORD				identification;
	WORD				offset;
	BYTE				ttl;
	BYTE				protocol;
	WORD				checksum;
	struct in_addr		src;
	struct in_addr		dst;
} IP_HEADER;

typedef struct {
	WORD				src_port;
	WORD				dst_port;
	DWORD				sequence;
	DWORD				acknowledgement;
	BYTE				length;
	BYTE				reserved;
	WORD				win_siz;
	WORD				checksum;
	WORD				urgent;
} TCP_HEADER;

class CAdapterSelect : public CForm {
protected:
	enum { IDC_LISTVIEW = 1001 };
	CListView		m_wndList;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( m_wndList.Create(this, 0, 0, 0, 0, IDC_LISTVIEW) ){
			for( int i=0; i<256; i++ ){
				m_wndList.InsertColumn(i, _T(""), 30);
			}

			for( int i=0; i<256; i++ ){
				m_wndList.InsertItem(i, _T(""));
			}
		}
		return TRUE;
	}

	void	OnSize(UINT,int cx,int cy){
		if( cx <= 0 || cy <= 0 )
			return;
		m_wndList.MoveWindow(0,0,cx,cy);
	}
public:
	CAdapterSelect( HWND hWnd){
		Create(_T("AdpaterSelect"),0,0,100,100,100,100,hWnd);
	}

	int		DoModal(){
		ShowWindow( SW_SHOW );
		UpdateWindow();
		return MessageLoop();
	}
};

class CReciverThread : public MT::IRunnable {
public:
	INET::CSocket		m_sock;

	void	run( void ){
		if( !m_sock.Create(AF_INET, SOCK_RAW, IPPROTO_IP) ){
			return;
		}

		DWORD	dwByte;
		std::vector<BYTE>	buf(4096);
		
		if( WSAEFAULT == ::WSAIoctl((SOCKET)m_sock,	SIO_ADDRESS_LIST_QUERY, NULL, 0, &buf.at(0), buf.size(), &dwByte, NULL, NULL ) ){
			return;
		}

		int	nNIC	= 0;

		SOCKET_ADDRESS_LIST*	pAddrList = (SOCKET_ADDRESS_LIST*)&buf.at(0);
		if( pAddrList->iAddressCount == 1 ){
			nNIC	= 0;
		}else
		if( pAddrList->iAddressCount > 1 ){
			for( int i=0; i<pAddrList->iAddressCount; i++ ){
				SOCKADDR_IN*	pNIC = (SOCKADDR_IN*)pAddrList->Address[ i ].lpSockaddr;
				TRACE("NIC [%02d]: %s\n", i, pNIC);
			}
		}else{
			return;
		}

		SOCKADDR_IN*	pNIC = (SOCKADDR_IN*)pAddrList->Address[ nNIC ].lpSockaddr;
		SOCKADDR_IN		addr = {0};
		addr.sin_addr.s_addr	= pNIC->sin_addr.s_addr;
		addr.sin_family			= AF_INET;
		addr.sin_port			= htons(0);

		if( SOCKET_ERROR == m_sock.Bind( (SOCKADDR*)&addr, sizeof(addr) ) ){
			return;
		}

		ULONG uRCVALL_OPTION = RCVALL_ON;
		if( ::WSAIoctl( (SOCKET)m_sock
				,	SIO_RCVALL, &uRCVALL_OPTION, sizeof(ULONG), NULL, 0, &dwByte, NULL, NULL ) ){
			return;
		}

		IP_HEADER	ipHdr;
		while( 1 )
		{
			dwByte	= (DWORD)m_sock.Recv( &buf.at(0), buf.size() );
			if( (dwByte == SOCKET_ERROR) || (dwByte == 0) ){
				break;
			}

			memmove( &ipHdr, &buf.at(0), sizeof(IP_HEADER) );

			Dump( ((ipHdr.ip_hl) * 4), &buf.at(0));
		}
	}

	void	Dump( DWORD dwSize, LPBYTE buf ){
	
		char	strDigits[]	= "0123456789abcdef";
		char	strLine[ 100 ];
		char	cb;
		DWORD	dwCount	= 0;
		DWORD	dwIndex	= 0;
		for( ; 0<dwSize; dwSize-=dwCount, buf+=dwCount, dwIndex+=dwCount )
		{
			dwCount	= (dwSize > 16) ? 16 : dwSize;

			sprintf(strLine, "4.4x ", dwIndex);
			cb	= 6;

			DWORD	i=0;
			for( ; i<dwCount; i++ ){
				strLine[ cb++ ]	= strDigits[ (buf[i] >> 4) ];
				strLine[ cb++ ]	= strDigits[ (buf[i] & 0x0F) ];

				if( i==7 )
					strLine[ cb++ ]	= ':';
				else
					strLine[ cb++ ]	= ' ';
			}

			for( ; i<16; i++ ){
				strLine[ cb++ ]	= ' ';
				strLine[ cb++ ]	= ' ';
				strLine[ cb++ ]	= ' ';
			}
			strLine[ cb++ ]	= ' ';

			for( i=0; i<dwCount; i++ ){
				if( buf[ i ] < 32 || buf[ i ] > 126 ){
					strLine[ cb++ ]	= '.';
				}else{
					strLine[ cb++ ]	= buf[ i ];
				}
			}

			strLine[ cb++ ]	= 0;
			TRACE("%s\n", strLine);
		}
	}
};

template<class CLASS, class SUPERCLASS>
class ICommandHandler : public SUPERCLASS {
protected:
	TCommandHandler<CLASS>	cmd;

public:
	void	OnCommand(UINT uID, HWND, UINT nCodeNotify){
		cmd.Dispach(uID, nCodeNotify);
	}
};

class CMainForm : public ICommandHandler<CMainForm,CForm> {
public:
	enum {
		ID_FILE			= 100	,
		ID_FILE_EXIT			,
		ID_ADAPTER				,
		ID_ADAPTER_SELECT		,
		ID_HELP					,
		ID_HELP_ABOUT			,

		IDC				= 1000	,
		IDC_LISTVIEW			,
	};

protected:
	CListView			m_wndList;

	void	InitMenu( void ){
		CPopupMenu	menuFile;
		menuFile.Create();
		menuFile.Insert(_T("Exit")		, ID_FILE_EXIT		);

		CPopupMenu	menuAdapter;
		menuAdapter.Create();
		menuAdapter.Insert(_T("Select")	, ID_ADAPTER_SELECT	);

		CPopupMenu	menuHelp;
		menuHelp.Create();
		menuHelp.Insert(_T("About")		, ID_HELP_ABOUT		);

		CMenu	menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File")		, ID_FILE		, (HMENU)menuFile		);
		menuMain.Insert(_T("Adapter")	, ID_ADAPTER	, (HMENU)menuAdapter	);
		menuMain.Insert(_T("Help")		, ID_HELP		, (HMENU)menuHelp		);

		SetMenu(m_hWnd, (HMENU)menuMain);
	}

	void	InitCtrl( void ){
		if( m_wndList.Create(this, 0, 0, 0, 0, IDC_LISTVIEW) ){
			for( int i=0; i<256; i++ ){
				m_wndList.InsertColumn(i, _T(""), 30);
			}

			for( int i=0; i<256; i++ ){
				m_wndList.InsertItem(i, _T(""));
			}
		}
	}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		InitMenu();
		InitCtrl();

		cmd.Initialize(this);
		cmd.Register(ID_ADAPTER_SELECT, &CMainForm::OnAdapterSelect);
		return TRUE;
	}

	void	OnSizs(UINT, int cx, int cy){
		if( cx <= 0 || cy <= 0 )
			return;
		m_wndList.MoveWindow(0,0,cx,cy);
	}

	void	OnAdapterSelect( void ){
	//	MT::CThread* pThread = new MT::CThread( new CReciverThread() );
	//	pThread->Start();
		CAdapterSelect	AdapterSelect(m_hWnd);
		AdapterSelect.DoModal();
	}
};

class CMainFormUIThread : public MT::IRunnable {
public:
	void	run( void ){
		CMainForm	Form;
		if( Form.Create(_T(""), 0, 0, 100, 100, 600, 300) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INET::WinInet::Startup(2.2);

	MT::CThread* pThread = new MT::CThread( new CMainFormUIThread() );
	pThread->Start();
	pThread->Wait();

	INET::WinInet::Cleanup();
}


