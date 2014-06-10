// NetworkInterfaceMonitor.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "NetworkInterfaceMonitor.h"

#include <mstcpip.h>

#include "UseToolkit.h"
#include "inet.h"
#include "Thread.h"


/**********************************************************************************
 *
 *
 *
 */
typedef struct {
#if		__BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char		ihl:4;
	unsigned char		version:4;
#elif	__BYTE_ORDER == __BIG_ENDIAN
	unsigned char		version:4;
	unsigned char		ihl:4;
#endif
	unsigned char		tos;
	unsigned short		tot_len;
	unsigned short		id;
#if		__BYTE_ORDER == __LITTLE_ENDIAN
	unsigned short		frag_off:13;
	unsigned short		flags:3;
#elif	__BYTE_ORDER == __BIG_ENDIAN
	unsigned short		flags:3;
	unsigned short		frag_off:13;
#endif
	unsigned char		ttl;
	unsigned char		protocol;
	unsigned short		check;
	unsigned int		saddr;
	unsigned int		daddr;
} IP_HEADER;
/**********************************************************************************
 *
 *
 *
 */
typedef struct {
	unsigned short		source;
	unsigned short		dest;
	unsigned int		seq;
	unsigned int		ack_seq;
#if		__BYTE_ORDER == __LITTLE_ENDIAN
	unsigned short
	res1:4,
	doff:4,
	fin:1,
	syn:1,
	rst:1,
	psh:1,
	ack:1,
	urg:1,
	res2:2;
#elif	__BYTE_ORDER == __BIG_ENDIAN
	unsigned short
	doff:4,
	res1:4,
	res2:2,
	urg:1,
	ack:1,
	psh:1,
	rst:1,
	syn:1,
	fin:1;
#endif
	unsigned short		window;
	unsigned short		check;
	unsigned short		urg_ptr;
} TCP_HEADER;
/**********************************************************************************
 *
 *
 *
 */
typedef struct {
	unsigned short		sport;
	unsigned short		dport;
	unsigned short		length;
	unsigned short		checksum;
} UDP_HEADER;
/**********************************************************************************
 *
 *
 *
 */
enum ENUM_PROTOCOL {
	PROTOCOL_TCP		= 6		,
	PROTOCOL_UDP		= 17	,
};
/**********************************************************************************
 *
 *
 *
 */
class CProtocolNumber {
protected:
	class CProtocolName {
	public:
		TString		strKeyword;
		TString		strProtocol;

		CProtocolName( LPCTSTR lpKey, LPCTSTR lpProtocol ) : strKeyword(lpKey), strProtocol(lpProtocol) {}
	};

	typedef std::map<BYTE, CProtocolName>	MAP_PROTOCOL;
	typedef std::pair<BYTE, CProtocolName>	PAIR_PROTOCOL;

	MAP_PROTOCOL	map;

protected:
	void		Create( void ){
		map.insert( PAIR_PROTOCOL(1,CProtocolName(_T("ICMP")	,_T("Internet Control Message"))) );
		map.insert( PAIR_PROTOCOL(2,CProtocolName(_T("IGMP")	,_T("Internet Group Management"))) );
		map.insert( PAIR_PROTOCOL(4,CProtocolName(_T("IP")		,_T("IP in IP(encapsulation)"))) );
		map.insert( PAIR_PROTOCOL(PROTOCOL_TCP,CProtocolName(_T("TCP")		,_T("Transmission Control"))) );
		map.insert( PAIR_PROTOCOL(7,CProtocolName(_T("CBT")		,_T("CBT"))) );
		map.insert( PAIR_PROTOCOL(8,CProtocolName(_T("EGP")		,_T("Exterior Gateway Protocol"))) );
		map.insert( PAIR_PROTOCOL(9,CProtocolName(_T("IGP")		,_T("any private interior gateway"))) );
		map.insert( PAIR_PROTOCOL(PROTOCOL_UDP,CProtocolName(_T("UDP")	,_T("User Datagram"))) );
	}

public:
	CProtocolNumber( void ){
		Create();
	}

	bool		Lookup( BYTE num, TString& strKeywoard, TString& strProtocol ){
		MAP_PROTOCOL::iterator	it = map.find( num );
		if( map.end() != it ){
			strKeywoard		= it->second.strKeyword;
			strProtocol		= it->second.strProtocol;
			return true;
		}
		return false;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CCapture : public MT::IRunnable {
protected:
	CListView&			m_listview;

	INET::CSocket		m_sock;
	SOCKADDR_IN			m_addr;
	MT::CEvent*			m_pevtShutdown;

	BYTE				szBuff[0xFFFF];
	CProtocolNumber		m_ProtocolNumber;
	const bool&			m_bAutoScroll;

public:
	CCapture( SOCKET s, SOCKADDR_IN* pAddr, CListView& listview, MT::CEvent* pEvent, bool& bAutoScroll )
		: m_sock(s), m_listview(listview) , m_pevtShutdown(pEvent), m_bAutoScroll(bAutoScroll)
	{
		memcpy(&m_addr, pAddr, sizeof(SOCKADDR_IN));
	}


protected:
	int			AppendString( LPTSTR lpszTxt ){
		int nItem = m_listview.GetItemCount();
		m_listview.InsertItem(nItem, lpszTxt);
		if( m_bAutoScroll )
			m_listview.EnsureVisible( nItem );
		return nItem;
	}

	void	HexDump( unsigned nLength, BYTE* pBuff ) 
	{
		const CHAR	DIGITS[]	= "0123456789abcdef";

		CHAR	linebuf[100];
		TCHAR	buf[100];

		unsigned	i	= 0;
		unsigned	ofs	= 0;
		unsigned	idx	= 0;
		unsigned	cnt	= 0;

		for( ofs=0; nLength; nLength-=cnt, pBuff+=cnt, ofs+=cnt )
		{
			cnt	= (nLength > 16) ? 16 : nLength;

			sprintf_s( linebuf, sizeof(linebuf), "%4.4x  ", ofs );
			idx = 6;

			for( i=0; i<cnt; i++ )
			{
				linebuf[ idx++ ]	= DIGITS[ pBuff[i] >> 4  ];
				linebuf[ idx++ ]	= DIGITS[ pBuff[i] & 0x0f];

				if( i == 7 )
				{
					linebuf[ idx++ ]	= ':';
				} 
				else
				{
					linebuf[ idx++ ] = ' ';
				}
			}

			 for( ; i<16; i++ ){
				  linebuf[ idx++ ]	= ' ';
				  linebuf[ idx++ ]	= ' ';
				  linebuf[ idx++ ]	= ' ';
			 }

			 linebuf[ idx++ ]	= ' ';

			 for( i=0; i<cnt; i++ )
			 {
				  if( pBuff[i] < 32 || pBuff[i] > 126 )
				  {
					   linebuf[ idx++ ]	= '.';
				  } 
				  else
				  {
					   linebuf[ idx++ ]	= pBuff[i];
				  }
			 }

			 linebuf[ idx++ ] = 0;
		//	 TRACE( "%s\n", linebuf );

			 TString::A2W( linebuf, strlen(linebuf)+1, buf, _countof(buf) );
			 AppendString( buf );
		 }
	} // end PrintHexDump

	void	IpHeader( IP_HEADER* pIp ){

		TString		str, strKey, strProto;
		AppendString( str.Format(_T("Version              : %u"), pIp->version)			);
		AppendString( str.Format(_T("IHL                  : %u"), pIp->ihl)				);
		AppendString( str.Format(_T("Type of Service      : %u"), pIp->tos)				);
		AppendString( str.Format(_T("Total Length         : %u"), ntohs(pIp->tot_len))	);
		AppendString( str.Format(_T("Identification       : %u"), ntohs(pIp->id))		);
		AppendString( str.Format(_T("Various Control Flags: 0x%X"), pIp->flags));
		AppendString( str.Format(_T("Fragment Offset      : 0x%X"), pIp->frag_off));
		AppendString( str.Format(_T("Time To Live         : %u"), pIp->ttl));
		if( m_ProtocolNumber.Lookup( (BYTE)pIp->protocol, strKey, strProto ) )
		{
			AppendString( str.Format(_T("Protocol             : %u %s : %s"), pIp->protocol, (LPCTSTR)strKey, (LPCTSTR)strProto) );
		}
		else
		{
			AppendString( str.Format(_T("Protocol             : %u"), pIp->protocol));
		}
		AppendString( str.Format(_T("Header Checksum      : %u"), ntohs(pIp->check))	);
		AppendString( str.Format(_T("Source IP Address    : %u.%u.%u.%u"), (pIp->saddr)&0xFF, (pIp->saddr>>8)&0xFF, (pIp->saddr>>16)&0xFF, (pIp->saddr>>24)&0xFF));
		AppendString( str.Format(_T("Destinatio IP Address: %u.%u.%u.%u"), (pIp->daddr)&0xFF, (pIp->daddr>>8)&0xFF, (pIp->daddr>>16)&0xFF, (pIp->daddr>>24)&0xFF));
		AppendString( _T("") );
	}

	void	TcpHeader( TCP_HEADER* pTcp ){
		TString		str;
		AppendString( str.Format(_T("Source Port          : %u"), ntohs(pTcp->source))		);
		AppendString( str.Format(_T("Destination Port     : %u"), ntohs(pTcp->dest))		);
		AppendString( str.Format(_T("Sequence Number      : %u"), ntohl(pTcp->seq))			);
		AppendString( str.Format(_T("Acknowledgment Number: %u"), ntohl(pTcp->ack_seq))		);
		AppendString( str.Format(_T("Data Offset          : %u"), pTcp->doff)				);
		AppendString( str.Format(_T("RRG: %u"), pTcp->urg));
		AppendString( str.Format(_T("ACK: %u"), pTcp->ack));
		AppendString( str.Format(_T("PSH: %u"), pTcp->psh));
		AppendString( str.Format(_T("RST: %u"), pTcp->rst));
		AppendString( str.Format(_T("SYN: %u"), pTcp->syn));
		AppendString( str.Format(_T("FIN: %u"), pTcp->fin));
		AppendString( str.Format(_T("Window        : %u"), ntohs(pTcp->window))	);
		AppendString( str.Format(_T("Checksum      : %u"), ntohs(pTcp->ack))	);
		AppendString( str.Format(_T("Urgent Pointer: 0x%X"), ntohs(pTcp->urg_ptr)));
		AppendString( _T("") );
	}

	void	UdpHeader( UDP_HEADER* pUdp ){
		TString	str;
		AppendString( str.Format(_T("Source      Port: %u"), ntohs(pUdp->sport))	);
		AppendString( str.Format(_T("Destination Port: %u"), ntohs(pUdp->dport))	);
		AppendString( str.Format(_T("Length          : %u"), ntohs(pUdp->length))	);
		AppendString( str.Format(_T("Checksum        : %u"), ntohs(pUdp->checksum))	);
		AppendString( _T("") );
	}

public:
	void	run( void ){
		DWORD	dwByte	= 0;
		int		nRet	= 0;

		nRet	= m_sock.Bind( (SOCKADDR*)&m_addr, sizeof(m_addr) );
		if( SOCKET_ERROR == nRet ){
			return;
		}

		ULONG	uOption = RCVALL_ON;
		nRet	= ::WSAIoctl(m_sock, SIO_RCVALL, &uOption, sizeof(ULONG), NULL, 0, &dwByte, NULL, NULL);
		if( 0 != nRet ){
			return;
		}

		IP_HEADER	iphdr	= {0};
		TCP_HEADER	tcphdr	= {0};
		UDP_HEADER	udphdr	= {0};

		while( ::WaitForSingleObject(m_pevtShutdown->m_hObject, 0) == WAIT_TIMEOUT )
		{
			dwByte	= m_sock.Recv(szBuff, sizeof(szBuff));
			if( SOCKET_ERROR == dwByte || 0 == dwByte )
			{
				break;
			}

			memmove(&iphdr, szBuff, sizeof(iphdr));
			AppendString(_T("------ IP Header ------"));
			HexDump( (iphdr.ihl * 4), szBuff );
			IpHeader( &iphdr );

			switch( iphdr.protocol ){
			case PROTOCOL_TCP:
			{
				LPBYTE lpBuffer		= szBuff;
				lpBuffer += (iphdr.ihl * 4);
				memmove( &tcphdr, lpBuffer, sizeof(TCP_HEADER) );

				AppendString(_T("------ TCP HEADER ------"));
				HexDump( sizeof(TCP_HEADER), (BYTE*)&tcphdr );
				TcpHeader( &tcphdr );

				DWORD	dwTotalLen = ((iphdr.ihl) * 4) + ((tcphdr.doff * 4));
				if( (dwTotalLen > 0) && ((dwByte - dwTotalLen) > 0) )
				{
					lpBuffer = szBuff;
					lpBuffer += dwTotalLen;
	
					AppendString(_T("------ TCP Payload ------"));
					HexDump( dwByte - dwTotalLen, lpBuffer );
					AppendString(_T(""));
				}
			}
			break;
			case PROTOCOL_UDP:
			{
				LPBYTE	lpBuffer	= szBuff;
				lpBuffer += (iphdr.ihl * 4);
				memmove( &udphdr, lpBuffer, sizeof(UDP_HEADER) );

				AppendString(_T("----- UDP HEADER -----"));
				UdpHeader( &udphdr );
			}
			break;
			}

			AppendString(_T("------ RAW DATA ------"));
			HexDump( dwByte, szBuff );
			AppendString(_T(""));
		}
	//	::DebugBreak();
	}
};
/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, class SUPERCLASS = CForm>
class ICommandHandler : public SUPERCLASS {
protected:
	TCommandHandler<CLASS>		cmd;
	TNotifyHandler<CLASS>		notify;
public:
	ICommandHandler( void ){
		cmd.Initialize((CLASS*)this);
		notify.Initialize((CLASS*)this);
	}
	void	OnCommand(UINT uID, HWND, UINT uNotifyCode){
		cmd.Dispach(uID, uNotifyCode);
	}
	void	OnNotify(WPARAM wParam, LPARAM lParam){
		notify.Dispach(wParam, lParam);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CAdapterSelect : public ICommandHandler<CAdapterSelect> {
public:
	enum{
		IDC				= 2000	,
		IDC_LISTBOX				,
	};
protected:
	CAdapterSelect();

	CWnd*				m_pParentWnd;
	CListBox			m_ListBox;

	char				m_szBuf[MAX_PATH];

public:
	INET::CSocket		m_sock;
	SOCKADDR_IN			m_addr;

protected:
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		int w	= lpCreateStruct->cx;
		int h	= lpCreateStruct->cy;
		
		m_ListBox.Create(this,0,0,w,h,IDC_LISTBOX);

		if( m_sock.Create(AF_INET, /*SOCK_STREAM*/SOCK_RAW, IPPROTO_IP) )
		{
			DWORD	dwByte	= 0;

			TString	strT;
			AString	strA;

			::WSAIoctl(m_sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, m_szBuf, sizeof(m_szBuf), &dwByte, NULL, NULL);

			SOCKET_ADDRESS_LIST*	pAddrList = (SOCKET_ADDRESS_LIST*)m_szBuf;
			if( pAddrList->iAddressCount == 0 )
			{
			}
			else
			{
				if( pAddrList->iAddressCount > 0 )
				{
					for( int i=0; i<pAddrList->iAddressCount; i++ )
					{
						SOCKADDR_IN*	pAddr = (SOCKADDR_IN*)pAddrList->Address[ i ].lpSockaddr;
						char* pNIC	= inet_ntoa( pAddr->sin_addr );
						if( pNIC )
						{
							strA.Format("%s\0",pNIC);
							TString::A2W(strA, strT);
						}
						m_ListBox.AddString(strT);
					}
				}
				else
				{
					;
				}
			}
		}else{
			return FALSE;
		}

		cmd.Register(IDC_LISTBOX, LBN_DBLCLK, &CAdapterSelect::OnLvnDblClick);

		SetFontChildren();
		return TRUE;
	}

	void	OnSize(UINT state, int cx, int cy){
		if( cx <= 0 || cy <= 0 )
			return;
		m_ListBox.MoveWindow(0,0,cx,cy);
	}

	void	OnLvnDblClick( void ){
		int nCurSel = m_ListBox.GetCurSel();
		int	nCount	= m_ListBox.GetCount();
		if( (0 <= nCurSel) && (nCurSel < nCount) )
		{
			SOCKADDR_IN*	pAddr = (SOCKADDR_IN*)((SOCKET_ADDRESS_LIST*)m_szBuf)->Address[ nCurSel ].lpSockaddr;

			m_addr.sin_addr.s_addr	= pAddr->sin_addr.s_addr;
			m_addr.sin_family		= AF_INET;
			m_addr.sin_port			= htons(0);

			DestroyWindow();
		}
	}

public:
	CAdapterSelect(CWnd* pParentWnd) : m_pParentWnd(pParentWnd) {
		CRect	rc;
		pParentWnd->GetWindowRect( &rc );

		int	x	= rc.left;
		int	y	= rc.top;

		Create(_T(""), 0, 0, x, y, 400, 200, pParentWnd->m_hWnd);
	}

	void	DoModal(void){
		ShowWindow(SW_SHOW);
		UpdateWindow();
		MessageLoop();
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public ICommandHandler<CMainForm> {
public:
	enum{
		ID_FILE				= 0		,
		ID_CAPTURE					,
		ID_HELP						,

		ID_FILE_EXIT				,
		ID_CAPTURE_START			,
		ID_CAPTURE_STOP				,
		ID_CAPTURE_AUTOSCROLL		,
		ID_HELP_ABOUT				,

		IDC					= 1000	,
		IDC_BTN_SELECT				,
		IDC_LISTVIEW				,
	};

protected:
	MT::CThread*		m_pThread;
	MT::CEvent			m_evtShutdown;

	CListView			m_wndList;

	bool				m_bAutoScroll;

	void	InitMenu( void ){
		CPopupMenu		menuFile;
		menuFile.Create();
		menuFile.Insert(NULL, NULL);
		menuFile.Insert(_T("Exit")			, ID_FILE_EXIT		);

		CPopupMenu		menuCapture;
		menuCapture.Create();
		menuCapture.Insert(_T("Start")		, ID_CAPTURE_START	);
		menuCapture.Insert(_T("Stop")		, ID_CAPTURE_STOP	);
		menuCapture.Insert(NULL, NULL);
		menuCapture.Insert(_T("Auto Scroll"), ID_CAPTURE_AUTOSCROLL	);

		CPopupMenu		menuHelp;
		menuHelp.Create();
		menuHelp.Insert(_T("About")			, ID_HELP_ABOUT	);

		CMenu			menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File")		, ID_FILE		, (HMENU)menuFile);
		menuMain.Insert(_T("Captrue")	, ID_CAPTURE	, (HMENU)menuCapture);
		menuMain.Insert(_T("Help")		, ID_HELP		, (HMENU)menuHelp);

		SetMenu( (HMENU)menuMain);

		CheckAutoScroll( true );
	}

	void	CheckAutoScroll( bool bOnOff ){
		HMENU	hMenu	= GetSubMenu( GetMenu(m_hWnd), 1 );

		MENUITEMINFO	mii	= {0};
		mii.cbSize		= sizeof(mii);
		mii.fMask		= MIIM_ID | MIIM_STATE;
		mii.wID			= ID_CAPTURE_AUTOSCROLL;

		GetMenuItemInfo(hMenu, ID_CAPTURE_AUTOSCROLL, FALSE, &mii);

		if( bOnOff )
		{
			m_bAutoScroll	= true;
			mii.fState		= MFS_CHECKED;
			SetMenuItemInfo(hMenu, ID_CAPTURE_AUTOSCROLL, FALSE, &mii);
		}
		else
		{
			m_bAutoScroll	= false;
			mii.fState		= MFS_UNCHECKED;
			SetMenuItemInfo(hMenu, ID_CAPTURE_AUTOSCROLL, FALSE, &mii);
		}
	}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct )
	{
		m_evtShutdown.m_hObject	= ::CreateEvent(NULL, TRUE, FALSE, _T("Shutdown"));

		InitMenu();

		if( m_wndList.Create(this,0,0,0,0,IDC_LISTVIEW) ){
			m_wndList.SetExtendedLitViewStyle( LVS_EX_FULLROWSELECT /*| LVS_EX_GRIDLINES*/ );
			m_wndList.InsertColumn(0, _T(""), (1024*3));
		//	for( int i=0; i<64; i++ ){
		//		m_wndList.InsertItem(i,_T(""));
		//	}
		}

		cmd.Register(ID_CAPTURE_START		, &CMainForm::OnCaptureStart);
		cmd.Register(ID_CAPTURE_STOP		, &CMainForm::OnCaptureStop);
		cmd.Register(ID_CAPTURE_AUTOSCROLL	, &CMainForm::OnCaptureAutoScroll);
		cmd.Register(ID_FILE_EXIT		, &CMainForm::OnFileExit);

		SetFontChildren();
		return TRUE;
	}

	void	OnSize(UINT state, int cx, int cy){
		if( cx <= 0 || cy <= 0 )
			return;
		m_wndList.MoveWindow(0,0,cx,cy);
	}

public:
	void	OnCaptureStart( void ){
		CAdapterSelect	dlg(this);
		dlg.DoModal();
		if( dlg.m_sock.IsValid() )
		{
			m_wndList.DeleteAllItems();
			m_evtShutdown.ResetEvent();

			WString	strW;
			AString strA;
			strA.Format("%s : %s", APPLICATION_NAME, inet_ntoa(dlg.m_addr.sin_addr));
			TString::A2W(strA,strW);
			SetWindowText( strW );

			m_pThread = new MT::CThread( new CCapture(dlg.m_sock.Detach(), &dlg.m_addr, m_wndList, &m_evtShutdown, m_bAutoScroll) );
			if( m_pThread )
			{
				m_pThread->Start();
			}
		}
	}

	void	OnCaptureStop( void ){
		m_evtShutdown.SetEvent();
		if( m_pThread ){
//			m_pThread->Wait();
			m_pThread	= NULL;
		}
	}

	void	OnCaptureAutoScroll( void ){
		m_bAutoScroll	= m_bAutoScroll ? false : true;
		CheckAutoScroll( m_bAutoScroll );
	}

	void	OnFileExit( void ){
		DestroyWindow();
	}
};
/**********************************************************************************
 *
 *
 *
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INET::WinInet::Startup();

	CMainForm	Form;
	if( Form.Create(_T(APPLICATION_NAME),0,0,100,100,600,600) ){
		Form.ShowWindow(SW_SHOW);
		Form.UpdateWindow();
		Form.MessageLoop();
	}

	INET::WinInet::Cleanup();

	return 0;
}
