// TreeAndList.cpp : アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "TreeAndList.h"

#include "UseToolkit.h"

/**********************************************************************************
 *
 *
 *
 */
class CDocument : public CSubject {
public:

	typedef std::map<int, std::map<int,TString> >	TABLE;
	typedef std::map<TString, TStringList>			TREE;

private:
	CDocument(const CDocument&);
	CDocument& operator=(const CDocument&);

	CDocument(){}

	TABLE	tbl;
	TREE	tree;

public:
	void	SetTable( int nRow, int nCol, TString& str ){
		tbl[ nRow ][ nCol ]	= str;
	}


	void	AppendTree( LPCTSTR lpszParent, LPCTSTR lpszChild ){
		TString	strParent(lpszParent), strChild(lpszChild);
		AppendTree( strParent, strChild );
	}
	void	AppendTree( TString& strParent, TString& strChild ){
		tree[ strParent ].push_back( strChild );
		NotifyAll();
	}

	TABLE&	GetTable( void ){
		return tbl;
	}

	TREE&	GetTree( void ){
		return tree;
	}

public:
	static CDocument& Instance( void ){
		static CDocument	doc;
		return doc;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CInputPanel : public CPanel, public IObservable {
public:
	enum {
		IDC_CMB_PARENT		= 1001	,
		IDC_CMB_CHILD				,
		IDC_BTN_ADD					,
	};
protected:
	CComboBox		m_cmbParent;
	CComboBox		m_cmbChild;
	CButton			m_btnAdd;

	TCommandHandler<CInputPanel>	cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( m_cmbParent.Create(this, 3, 3, 160, 100, IDC_CMB_PARENT) ){
			m_cmbParent.SetFont();
			m_cmbParent.AddString( _T("system") );
			m_cmbParent.AddString( _T("semaphore") );
			m_cmbParent.AddString( _T("sys_info") );
			m_cmbParent.AddString( _T("image") );
		}

		if( m_cmbChild.Create(this, 165, 3, 160, 100, IDC_CMB_CHILD) ){
			m_cmbChild.SetFont();
		}

		if( m_btnAdd.Create(this, 325, 3, 50, 20, IDC_BTN_ADD, _T("ADD")) ){
			m_btnAdd.SetFont();
		}

		cmd.Initialize( this );
		cmd.Register( IDC_CMB_PARENT, CBN_SELCHANGE	, &CInputPanel::OnCmbSelChange	);
		cmd.Register( IDC_CMB_PARENT, CBN_DBLCLK	, &CInputPanel::OnCmbDblClk		);
		cmd.Register( IDC_CMB_PARENT, CBN_EDITUPDATE, &CInputPanel::OnCmbEditUpdate	);
		cmd.Register( IDC_CMB_PARENT, CBN_EDITCHANGE, &CInputPanel::OnCmbEditChange	);
		cmd.Register( IDC_CMB_PARENT, CBN_SETFOCUS	, &CInputPanel::OnCmbSetFocus	);
		cmd.Register( IDC_CMB_PARENT, CBN_KILLFOCUS	, &CInputPanel::OnCmbKillFocus	);
		cmd.Register( IDC_CMB_PARENT, CBN_DROPDOWN	, &CInputPanel::OnCmbDropDown	);
		cmd.Register( IDC_BTN_ADD	, BN_CLICKED	, &CInputPanel::OnBtnClick		);

		return TRUE;
	}

	void	OnCmbSelChange( void ){
		DBG::TRACE(_T("CBN_SELENDOK"));
	}

	void	OnCmbDblClk( void ){
		DBG::TRACE(_T("CBN_DBLCLK"));
	}

	void	OnCmbEditUpdate( void ){
		DBG::TRACE(_T("CBN_EDITUPDATE"));
	}

	void	OnCmbEditChange( void ){
		DBG::TRACE(_T("CBN_EDITCHANGE"));
	}

	void	OnCmbSetFocus( void ){
		DBG::TRACE(_T("CBN_SETFOCUS"));
	}

	void	OnCmbKillFocus( void ){
		DBG::TRACE(_T("CBN_KILLFOCUS"));
		TCHAR	szBuf[ 256 ];
		m_cmbParent.GetWindowText( szBuf, 256 );
		TString	str( szBuf );
		if( str.size() > 0 ){
			DBG::TRACE( str );
			if( m_cmbParent.FindString( -1, str ) == CB_ERR )
			{
				m_cmbParent.AddString( str );
			}
		}
	}

	void	OnCmbDropDown( void ){
		DBG::TRACE(_T("CBN_DROPDOWN"));
		TCHAR	szBuf[ 256 ];
		m_cmbParent.GetWindowText( szBuf, 256 );
		TString	str( szBuf );
		if( str.size() > 0 ){
			DBG::TRACE( str );
			if( m_cmbParent.FindString( -1, str ) == CB_ERR )
			{
				m_cmbParent.AddString( str );
			}
		}
	}

	void	OnBtnClick( void ){
		TCHAR	szParent[256], szChild[256];

		int nParentCurSel	= m_cmbParent.GetCurSel();
		if( nParentCurSel == CB_ERR ){
			m_cmbParent.GetWindowText( szParent, 256 );
		}else{
			m_cmbParent.GetLBText( nParentCurSel , szParent );
		}

		int	nChildCurSel	= m_cmbChild.GetCurSel();
		if( nChildCurSel == CB_ERR ){
			m_cmbChild.GetWindowText( szChild, 256 );
		}else{
			m_cmbChild.GetLBText( nChildCurSel , szChild );
		}

		CDocument::Instance().AppendTree( szParent, szChild );
	}

	void	OnKeyDown( UINT nVk, BOOL fDown, int nRepeat, UINT uFlgas ){
		if( nVk == VK_RETURN ){
			DBG::TRACE(_T("OnKeyDown( VK_RETURN )"));
		}
	}

	void	OnCommand( UINT nID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach( nID, nCodeNotify );
	}

	void	Notify(){
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CExprolerPanel : public CPanel, public IObservable {
public:
	enum {
		IDC_TREEVIEW	= 1001	,
	};
protected:
	CTreeView				m_TreeView;
	CGridPanel				m_wndGrid;
	CVerticalSplitter		m_Splitter;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rc;
		GetClientRect( &rc );

		if( m_TreeView.Create(this, 0, 0, rc.Width(), rc.Height(), IDC_TREEVIEW) ){
		}

		if( m_wndGrid.Create(this, 0, 0, 0, 0) ){
			CListView&	ListView = m_wndGrid.GetListView();
			ListView.SetExtendedLitViewStyle( LVS_EX_GRIDLINES );
	
			ListView.InsertColumn( 0, _T("name"), 200 );
			ListView.InsertColumn( 1, _T("window1") );
			ListView.InsertColumn( 2, _T("window2") );
			ListView.InsertColumn( 3, _T("window3") );
			ListView.InsertColumn( 4, _T("window4") );

			for( int i=0; i<1024; i++ ){
				TString	str;
				str.Format(_T("%d"), i );		ListView.InsertItem( i, str );
				str.Format(_T("%d, 1"), i );	ListView.SetItem( i, 1, str );
				str.Format(_T("%d, 2"), i );	ListView.SetItem( i, 2, str );
				str.Format(_T("%d, 3"), i );	ListView.SetItem( i, 3, str );
				str.Format(_T("%d, 4"), i );	ListView.SetItem( i, 4, str );
			}
		}

		m_Splitter.Bind( this );
		m_Splitter.Append( &m_TreeView	);
		m_Splitter.Append( &m_wndGrid	);

		return TRUE;
	}

	void	OnLButtonDown( int x, int y, UINT KeyFlags ){
		m_Splitter.BeginSplitterMoving();
	}

	void	OnLButtonUp( int x, int y, UINT KeyFlags ){
		m_Splitter.EndSplitterMoving();
	}

	void	OnMouseMove( int x, int y, UINT KeyFlags ){
		if( m_Splitter.IsSplitterMoving( KeyFlags ) )
			m_Splitter.MoveSplitter( x, y );
	}

	void	OnSize( UINT state, int cx, int cy ){
		m_Splitter.AdjustWindow( cx, cy );
	}

	void	Notify(){

		CDocument::TREE& tree = CDocument::Instance().GetTree();
		CDocument::TREE::iterator	it = tree.begin();
		for( ; it!=tree.end(); it++ ){
			DBG::TRACE(_T("%s"), (LPCTSTR)(TString)(it->first) );
		}
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public CForm {
protected:
	CInputPanel				m_wndInput;
	CExprolerPanel			m_wndExproler;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rc;
		GetClientRect( &rc );

		if( !m_wndInput.Create(this, 0, 0, 300, 30) ){
			return FALSE;
		}

		if( !m_wndExproler.Create(this, 0, 0, 300, 300) ){
			return FALSE;
		}

		CDocument::Instance().add( &m_wndInput );
		CDocument::Instance().add( &m_wndExproler );

		return TRUE;
	}

	void	OnSize( UINT state, int cx, int cy ){
		m_wndInput.MoveWindow( 0, 0, cx, 30 );
		m_wndExproler.MoveWindow( 0, 30, cx, cy-30 );
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

	CMainForm	Form;
	if( Form.Create(_T("TreeAndList"),0,0,100,100,600,600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
