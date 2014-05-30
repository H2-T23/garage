// ExampleWinHttp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleWinHttp.h"

#include "UseToolkit.h"

#include <winhttp.h>
#pragma comment(lib,"winhttp.lib")


/**********************************************************************************
 *
 *
 *
 */
class CWinHttp {
protected:
	typedef std::basic_string<TCHAR>	String;
	typedef std::vector<BYTE>			Buffer;
	typedef std::map<DWORD,String>		ErrorStatus;
	typedef std::pair<DWORD,String>		ErrorPair;

	HINTERNET		m_hSession, m_hConnect, m_hRequest;
	std::string		m_Header, m_Body;
	ErrorStatus		m_ERR;

	void	MakeErrorStatus( void ){
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_CANNOT_CONNECT			, _T("ERROR_WINHTTP_CANNOT_CONNECT")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED	, _T("ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_CONNECTION_ERROR			, _T("ERROR_WINHTTP_CONNECTION_ERROR")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_INCORRECT_HANDLE_STATE	, _T("ERROR_WINHTTP_INCORRECT_HANDLE_STATE")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_INCORRECT_HANDLE_TYPE		, _T("ERROR_WINHTTP_INCORRECT_HANDLE_TYPE")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_INTERNAL_ERROR			, _T("ERROR_WINHTTP_INTERNAL_ERROR")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_INVALID_URL				, _T("ERROR_WINHTTP_INVALID_URL")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_LOGIN_FAILURE				, _T("ERROR_WINHTTP_LOGIN_FAILURE")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_NAME_NOT_RESOLVED			, _T("ERROR_WINHTTP_NAME_NOT_RESOLVED")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_OPERATION_CANCELLED		, _T("ERROR_WINHTTP_OPERATION_CANCELLED")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW	, _T("ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_SECURE_FAILURE			, _T("ERROR_WINHTTP_SECURE_FAILURE")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_SHUTDOWN					, _T("ERROR_WINHTTP_SHUTDOWN")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_TIMEOUT					, _T("ERROR_WINHTTP_TIMEOUT")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_UNRECOGNIZED_SCHEME		, _T("ERROR_WINHTTP_UNRECOGNIZED_SCHEME")) );
		m_ERR.insert( ErrorPair(ERROR_NOT_ENOUGH_MEMORY					, _T("ERROR_NOT_ENOUGH_MEMORY")) );
		m_ERR.insert( ErrorPair(ERROR_INVALID_PARAMETER					, _T("ERROR_INVALID_PARAMETER")) );
		m_ERR.insert( ErrorPair(ERROR_WINHTTP_RESEND_REQUEST			, _T("ERROR_WINHTTP_RESEND_REQUEST")) );
	}

	void	OutputErrorMessage( DWORD dwErrCode ){
		if( m_ERR.size() <= 0 ){
			MakeErrorStatus();
		}

		ErrorStatus::iterator	it = m_ERR.find( dwErrCode );
		if( it != m_ERR.end() ){
			OutputDebugString( it->second.c_str() );
		}
	}
	
public:
	CWinHttp( void ) : m_hSession(NULL), m_hConnect(NULL), m_hRequest(NULL) {
	}

	CWinHttp( HINTERNET hSession ) : m_hSession(hSession), m_hConnect(NULL), m_hRequest(NULL) {
	}

	inline BOOL	IsOpen( void ) const {
		return(m_hSession==NULL ? FALSE : TRUE);
	}
	inline BOOL	IsConnect( void ) const {
		if( IsOpen() )
			return(m_hConnect==NULL ? FALSE : TRUE);
		return FALSE;
	}
	inline BOOL	IsRequest( void ) const {
		if( IsOpen() && IsConnect() )
			return(m_hRequest==NULL ? FALSE : TRUE);
		return FALSE;
	}

	BOOL		Open( void ){
		m_hSession = WinHttpOpen(_T("Sample Application/1.0"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		return IsOpen();
	}

	inline void		Close( HINTERNET hInternet ){
		WinHttpCloseHandle(hInternet);
	}

	void		Close( void ){
		if( m_hRequest ){
			Close( m_hRequest );
			m_hRequest	= NULL;
		}

		if( m_hConnect ){
			Close( m_hConnect );
			m_hConnect	= NULL;
		}

		if( m_hSession ){
			Close( m_hSession );
			m_hSession	= NULL;
		}
	}

	BOOL		Connect( LPTSTR lpHostName, int nPort = INTERNET_DEFAULT_PORT ){
		if( IsOpen() )
			m_hConnect	= WinHttpConnect(m_hSession, lpHostName, nPort, 0);
		return IsConnect();
	}

	BOOL		Request( LPCTSTR lpszCmd, LPTSTR lpszUrlPath ){
		if( IsConnect() )
		{
			::SetLastError( 0 );
			m_hRequest	= WinHttpOpenRequest(m_hConnect, lpszCmd, lpszUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

			if( IsRequest() )
			{
				if( !WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH, 0) )
				{
					OutputErrorMessage( ::GetLastError() );
					Close();
				}
			}
		}
		return IsRequest();
	}

	void		New( LPCTSTR lpURL ){
		if( !Open() )
			return;
		
		TCHAR		szHostName[MAX_PATH];
		TCHAR		szUrlPath[(1024*2)];


		String	strURL(lpURL);

		URL_COMPONENTS	urlcomp	= {0};
		urlcomp.dwStructSize			= sizeof(URL_COMPONENTS);
		urlcomp.lpszHostName			= szHostName;
		urlcomp.dwHostNameLength		= _countof(szHostName);
		urlcomp.lpszUrlPath				= szUrlPath;
		urlcomp.dwUrlPathLength			= _countof(szUrlPath);

		if( !WinHttpCrackUrl(&strURL.at(0), strURL.length(), 0, &urlcomp) ){
			Close();
			return;
		}

		if( !Connect(szHostName,5899) ){
			Close();
			return;
		}

		if( !Request(_T("GET"), szUrlPath) ){
			Close();
			return;
		}

		WinHttpReceiveResponse(m_hRequest, NULL);

		DWORD	dwSize	= 0;
		WinHttpQueryHeaders( m_hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX );

		m_Header.resize(dwSize);
		WinHttpQueryHeaders( m_hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, &m_Header.at(0), &dwSize, WINHTTP_NO_HEADER_INDEX );

		Buffer		buf;
		while( 1 )
		{
			WinHttpQueryDataAvailable( m_hRequest, &dwSize );
			if( dwSize > 0 )
			{
				buf.resize( dwSize );
				WinHttpReadData( m_hRequest, &buf.at(0), dwSize, NULL );
				buf.push_back( 0 );
				m_Body.append( (char*)&buf.at( 0 ) );
			}
			else
			{
				break;
			}
		}

		Close();
	}
};
/**********************************************************************************
 *
 *
 *
 */
template<class CLASS, class SUPERCLASS>
class ICommandHandler : public SUPERCLASS {
protected:
	TCommandHandler<CLASS>	cmd;

public:
	void	OnCommand(UINT uID, HWND, UINT uCodeNotify){
		cmd.Dispach(uID, uCodeNotify);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public ICommandHandler<CMainForm,CForm> {
public:
	enum {
		ID_MENU			= 100	,
		ID_FILE					,
		ID_FILE_OPEN			,
		ID_FILE_EXIT			,
		ID_HELP					,
		ID_HELP_ABOUT			,

		IDD				= 500	,
		IDD_DIALOG				,

		IDC				= 1000	,
		IDC_EDT_URL				,
		IDC_BTN_URL				,
		IDC_EDT_TXT				,
	};
protected:
	CEdit			m_edtURL;
	CButton			m_btnURL;
	CMultiEdit		m_edtTXT;

	void		InitMenu( void ){
		CPopupMenu	menuFile;
		menuFile.Create();
		menuFile.Insert(_T("Open")	, ID_FILE_OPEN);
		menuFile.Insert(NULL, NULL);
		menuFile.Insert(_T("Exit")	, ID_FILE_EXIT);

		CPopupMenu	menuHelp;
		menuHelp.Create();
		menuHelp.Insert(_T("About")	, ID_HELP_ABOUT);

		CMenu		menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File"), ID_FILE	, (HMENU)menuFile);
		menuMain.Insert(_T("Help"), ID_HELP	, (HMENU)menuHelp);

		SetMenu(m_hWnd, (HMENU)menuMain);
	}

	BOOL		OnCreate( LPCREATESTRUCT lpCreateStruct ){
		InitMenu();
		
		int	w	= lpCreateStruct->cx;
		int	h	= lpCreateStruct->cy;

		if( m_edtURL.Create(this, 0, 0, 0, 0, IDC_EDT_URL) ){
		}

		if( m_btnURL.Create(this, 0, 0, 0, 0, IDC_BTN_URL, _T("★")) ){
		}

		if( m_edtTXT.Create(this, 0, 0, 0, 0, IDC_EDT_TXT) ){
		}

		cmd.Initialize(this);
		cmd.Register(ID_FILE_OPEN	, &CMainForm::OnFileOpen);
		cmd.Register(ID_FILE_EXIT	, &CMainForm::OnFileExit);
		cmd.Register(ID_HELP_ABOUT	, &CMainForm::OnHelpAbout);

		cmd.Register(IDC_BTN_URL	, &CMainForm::OnFileOpen);

		SetFontChildren();
		return TRUE;
	}

	void	Resize(int w, int h){
		m_edtURL.MoveWindow(0   ,  0, w-36, 20  );
		m_btnURL.MoveWindow(w-35,  0, 35  , 20  );
		m_edtTXT.MoveWindow(0   , 20, w   , h-20);
	}

	void	OnSize(UINT state, int cx, int cy){
		if( cx <= 0 || cy <= 0 )
			return;
		Resize(cx, cy);
	}

public:
	void		OnFileOpen( void ){
		TString		strURL;
		m_edtURL.GetText( strURL );

		CWinHttp	http;
		http.New( strURL );
	}

	void		OnFileExit( void ){
	}

	static 
	INT_PTR CALLBACK	DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
		switch( uMsg ){
		case WM_INITDIALOG:
			return 0;
		}
		return ::DefDlgProc(hWnd, uMsg, wParam, lParam);
	}

	void		OnHelpAbout( void ){
		DLGTEMPLATE	temp	= {0};
		temp.x					= 0;
		temp.y					= 0;
		temp.cx					= 100;
		temp.cy					= 100;
		temp.style				= WS_CAPTION;
		temp.dwExtendedStyle	= 0;
		temp.cdit				= IDD_DIALOG;

		HWND	hDlg	= ::CreateDialogIndirect( ::GetModuleHandle(NULL), &temp, m_hWnd, (DLGPROC)DlgProc );
		if( hDlg )
		{
			::ShowWindow( hDlg, SW_SHOW );
		}
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

	//                    0123456789					
	std::string		str("\n0123456789\n\n\0");
	int idx = str.find("\n\n\0",0);


	CMainForm	Form;
	if( Form.Create(_T("WinHTTP"), 0, 0, 100, 100, 600, 300) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		Form.MessageLoop();
	}
	return 0;
}
