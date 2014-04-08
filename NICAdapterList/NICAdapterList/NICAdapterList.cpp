// NICAdapterList.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "NICAdapterList.h"

#include "UseToolkit.h"

#include "Inet.h"
#include "Thread.h"


using namespace INET;
using namespace MT;

class CEnumAdapter {
public:
	CSocket		m_sock;

	typedef std::vector<ULONG>			ADDR_LIST;
	typedef ADDR_LIST::const_iterator	ITERATOR;

	ADDR_LIST			m_AddrList;
	ITERATOR			m_It;

	std::vector<ULONG>		address;
	ULONG		m_Address[10];
	int			m_nAddresEntry;

	CEnumAdapter( void ){
		m_nAddresEntry	= 0;

		if( m_sock.Create(AF_INET, SOCK_STREAM, IPPROTO_IP) )
		{
			char	Buffer[ 1024 ];
			DWORD	d;
			if( 0 == WSAIoctl(m_sock.m_hHandle, SIO_ADDRESS_LIST_QUERY, NULL, 0, Buffer, 1024, &d, NULL, NULL))
			{
				SOCKET_ADDRESS_LIST*	pList = (SOCKET_ADDRESS_LIST*)Buffer;

				for( int i=0; i<pList->iAddressCount; i++ )
				{
					if( ((sockaddr*)(pList->Address[ i ].lpSockaddr))->sa_family == AF_INET )
					{
						m_Address[ i ]	= ((sockaddr_in*)(pList->Address[ i ].lpSockaddr))->sin_addr.s_addr;
						m_nAddresEntry++;


					}
				}
			}
		}
	}

	ITERATOR&	Begin( void ){
		return(m_It = m_AddrList.begin());
	}

	ITERATOR&	Next( void ){
		return(++m_It);
	}

	bool		hasNext( void ) const {
		return(m_It!=m_AddrList.end());
	}
};

class CAdapterSelectForm : public CForm {
public:
	enum {
		IDC_LISTBOX1	= 1001,
	};
protected:
	CListBox		m_ListBox;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_ListBox.Create(this, 0, 0, 0, 0, IDC_LISTBOX1);

		CEnumAdapter	EnumAdapter;
		TString	str;
		for( int i=0; i<EnumAdapter.m_nAddresEntry; i++ ){
			unsigned char*	p = (unsigned char*)&EnumAdapter.m_Address[ i ];
			str.Format(_T("%d.%d.%d.%d"), *(p+0), *(p+1), *(p+2), *(p+3) ); 
			m_ListBox.AddString( str );
		}

		CEnumAdapter::ITERATOR	it = EnumAdapter.Begin();
		for( ; EnumAdapter.hasNext(); it = EnumAdapter.Next() ){
			
		}

		SetFontChildren();
		return TRUE;
	}

	void	OnSize(UINT, int cx, int cy){
		m_ListBox.MoveWindow(0, 0, cx, cy);
	}

public:
	~CAdapterSelectForm( void ){
		DestroyWindow();
	}

	void	DoModal( CWnd* pParentWnd = NULL ) {
		Create(_T(""), 0, 0, 100, 100, 300, 300, pParentWnd->m_hWnd);		
		ShowWindow( SW_SHOW );
		UpdateWindow();
		MessageLoop();
		DestroyWindow();
	}
};


class CCaptureThread : public IThread {
protected:
	CEvent		m_evtExit;

	bool		IsExit( void ){
		WaitForSingleObject(m_evtExit.m_hObject, 0) == INFINITE);
	}

	unsigned	run( LPVOID lpParam ){


		while( IsExit() )
		{
		}

	}
};

class CMainForm : public TSplitter<CForm, CHorizontalSplitter> {
public:
	enum {
		IDC_BTN_ADAPTERSELECT	= 1001	,
		IDC_BTN_CAPTURE					,
		IDC_LISTVIEW					,
		IDC_EDIT						,
	};
protected:
	CButton					m_btn, m_btnCapture;
	CListView				m_ListView;
	CMultiEdit				m_Edit;

	TCommandHandler<CMainForm>	cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_btn.Create(this, 10, 10, 80, 30, IDC_BTN_ADAPTERSELECT, _T("AdpaterSelect"));
		m_btnCapture.Create(this, 10, 40, 80, 30, IDC_BTN_CAPTURE, _T("Capture"));

		if( m_ListView.Create(this, 0, 0, 0, 0, IDC_LISTVIEW) ){
			m_ListView.InsertColumn( 0, _T("TIME"), 100 );
			m_ListView.InsertColumn( 1, _T("PROTOCOL"), 100 );
			m_ListView.InsertColumn( 2, _T("IP(SOURCE)"), 100 );
			m_ListView.InsertColumn( 3, _T("IP(DESTINATION)"), 100 );
			m_ListView.InsertColumn( 4, _T("PORT(SOURCE)"), 100 );
			m_ListView.InsertColumn( 5, _T("PORT(DESTINATION)"), 100 );
			m_ListView.InsertColumn( 6, _T("SIZE"), 100 );
		}
		m_Edit.Create(this, 0, 0, 0, 0, IDC_EDIT);

		m_Splitter.Bind( this );
		m_Splitter.Append( &m_ListView	);
		m_Splitter.Append( &m_Edit		);

		cmd.Initialize( this );
		cmd.Register( IDC_BTN_ADAPTERSELECT, &CMainForm::OnBtnAdapterSelect );
		cmd.Register( IDC_BTN_CAPTURE, &CMainForm::OnBtnCapture );
		SetFontChildren();
		return TRUE;
	}

	void	OnCommand(UINT uID, HWND hWndCtrl, UINT nCodeNotify){
		cmd.Dispach(uID, nCodeNotify);
	}

	void	OnBtnAdapterSelect( void ){
		CAdapterSelectForm	form;
		form.DoModal(this);
	}

	void	OnBtnCapture( void ){
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INET::WinInet::Startup();

	CMainForm	Form;
	if( Form.Create(_T(""), 0, 0, 100, 100, 600, 600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}

	INET::WinInet::Cleanup();
	return 0;
}
