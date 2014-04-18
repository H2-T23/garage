// TableData.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "TableData.h"

#include "UseToolkit.h"
/****************************************************************************************
 *
 *
 *
 *
 *
 */
template<typename TYPE>
class TTable {
private:
	TTable(const TTable&);
	TTable&	operator=(const TTable&);

public:
	int									Rows;
	int									Cols;
	std::vector<std::vector<TYPE> >		Table;

	TTable(int row=1024,int col=256){
		TTable::Resize(row,col);
	}

	int		Resize(int row, int col){
		Rows = row;
		Cols = col;
		Table.resize(Rows);
		for( int i=0; i<Rows; i++ ){
			Table[ i ].resize(Cols);
		}
		return(Rows*Cols);
	}

	std::vector<TYPE>&	operator[](int i){
		return(Table[i]);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CDocument : public CSubject {
public:
//	typedef std::map<int, std::map<int,TString> >	TABLE;
	typedef TTable<TString>							TABLE;
	typedef std::map<TString, TStringList>			TREE;
	typedef std::list<TString>						LIST;

private:
	CDocument(const CDocument&);
	CDocument& operator=(const CDocument&);

	CDocument() : m_UpdateTable(FALSE), m_UpdateTree(FALSE) {
	}

	TABLE			tbl;
	TREE			tree;
	LIST			m_SelectedItems;

	BOOL			m_UpdateTable;
	BOOL			m_UpdateTree;

	void		UpdateTable( void ){
		m_UpdateTable	= TRUE;
		NotifyAll();
		m_UpdateTable	= FALSE;
	}

	void		UpdateTree( void ){
		m_UpdateTree	= TRUE;
		NotifyAll();
		m_UpdateTree	= FALSE;
	}

public:
	BOOL		IsUpdateTable( void ) const {		return m_UpdateTable;	}
	BOOL		IsUpdateTree( void ) const {		return m_UpdateTree;	}

	TABLE&		GetTable( void )		{	return tbl;	}
	TREE&		GetTree( void )			{	return tree;}
	LIST&		GetSelctedItem( void )	{	return m_SelectedItems; }

public:
	void	Selected( LPCTSTR lpszItem, BOOL bSelected ){
		if( bSelected ){
			m_SelectedItems.push_back( TString(lpszItem) );
		}else{
			m_SelectedItems.remove( TString(lpszItem) );
		}
		UpdateTable();
	}

	void	SetTable( int nRow, int nCol, TString& str ){
		tbl[ nRow ][ nCol ]	= str;
		UpdateTable();
	}

	void	AppendTree( LPCTSTR lpszParent, LPCTSTR lpszChild ){
		TString	strParent(lpszParent), strChild(lpszChild);
		AppendTree( strParent, strChild );
	}

	void	AppendTree( TString& strParent, TString& strChild ){
		tree[ strParent ].push_back( strChild );
		UpdateTree();
	}

	BOOL		Open( LPCTSTR lpFilename ){
		CTxtFile	txt(lpFilename);
		if( !txt.IsOpen() ){
			return FALSE;
		}

		std::vector<TString>		strList;

		BYTE	buf[512];

		int	nSize = 0;
		while( (nSize = txt.Read((LPVOID)buf, sizeof(buf))) > 0 )
		{
			LPTSTR	lpStr = (LPTSTR)buf;
			
			int	start = 0;
			for( int i=0; i<(nSize/sizeof(TCHAR)); i++ ){
				if( lpStr[ i ] == _T('\n') )
				{
					TString	str;
					str.assign( &lpStr[ start ], i-start );
					strList.push_back( str );
					start = i+1;
				}
			}
		}

		for( int i=0; i<strList.size(); i++ ){
			int	idx = strList[ i ].find(_T('.'), 0);
			if( idx > 0 )
			{
				TString	str1, str2;
				str1.assign( &strList[ i ].at( 0 ), idx );
				str2.assign( &strList[ i ].at( idx+1 ) );
				tree[ str1 ].push_back( str2 );
			}
		}

		UpdateTree();
		return TRUE;
	}

	BOOL		Save( LPCTSTR lpFilename ){
		CTxtFile	txt(lpFilename);
		if( !txt.IsOpen() ){
			return FALSE;
		}

		TString		str;

		TREE::iterator	it1 = tree.begin();
		for( ; it1!=tree.end(); it1++ )
		{
			TStringList::iterator	it2	= it1->second.begin();
			for( ; it2!=(it1->second.end()); it2++ )
			{
				str.Format(_T("%s.%s\n\0"), (LPCTSTR)it1->first, (LPCTSTR)*it2);
				DBG::TRACE( str );
				txt.WriteString( str );
			}
		}

		return TRUE;
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
		IDC_BTN_OPEN				,
		IDC_BTN_SAVE				,
	};
protected:
	CComboBox		m_cmbParent;
	CComboBox		m_cmbChild;
	CButton			m_btnAdd;
	CButton			m_btnOpne;
	CButton			m_btnSave;

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

		if( m_btnOpne.Create(this, 400, 3, 50, 20, IDC_BTN_OPEN, _T("OPEN")) ){
			m_btnOpne.SetFont();
		}

		if( m_btnSave.Create(this, 455, 3, 50, 20, IDC_BTN_SAVE, _T("SAVE")) ){
			m_btnSave.SetFont();
		}

		cmd.Initialize( this );
		cmd.Register( IDC_CMB_PARENT, CBN_SELCHANGE	, &CInputPanel::OnCmbSelChange	);
		cmd.Register( IDC_CMB_PARENT, CBN_DBLCLK	, &CInputPanel::OnCmbDblClk		);
		cmd.Register( IDC_CMB_PARENT, CBN_EDITUPDATE, &CInputPanel::OnCmbEditUpdate	);
		cmd.Register( IDC_CMB_PARENT, CBN_EDITCHANGE, &CInputPanel::OnCmbEditChange	);
		cmd.Register( IDC_CMB_PARENT, CBN_SETFOCUS	, &CInputPanel::OnCmbSetFocus	);
		cmd.Register( IDC_CMB_PARENT, CBN_KILLFOCUS	, &CInputPanel::OnCmbKillFocus	);
		cmd.Register( IDC_CMB_PARENT, CBN_DROPDOWN	, &CInputPanel::OnCmbDropDown	);
		cmd.Register( IDC_BTN_ADD	, BN_CLICKED	, &CInputPanel::OnBtnClickAdd	);
		cmd.Register( IDC_BTN_OPEN	, BN_CLICKED	, &CInputPanel::OnBtnClickOpen	);
		cmd.Register( IDC_BTN_SAVE	, BN_CLICKED	, &CInputPanel::OnBtnClickSave	);

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

	void	OnBtnClickAdd( void ){
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

	void	OnBtnClickOpen( void ){
		DBG::TRACE(_T("Opne"));
		CDocument::Instance().Open(_T("param.txt"));
	}

	void	OnBtnClickSave( void ){
		DBG::TRACE(_T("Save"));
		CDocument::Instance().Save(_T("param.txt"));
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

		WM_TREE_CHECKBOX_CLICK	= (WM_USER+3),
	};
protected:
	CCheckBoxTreeView						m_TreeView;
	CGridPanel								m_wndGrid;
	CVerticalSplitter						m_Splitter;
	
	TNotifyHandler<CExprolerPanel>			notify;
	TMessageHandler<CExprolerPanel>			msg;

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

		notify.Initialize( this );
		notify.Register( NM_CLICK, IDC_TREEVIEW, &CExprolerPanel::OnNmClick );

		msg.Initialize( this );
		msg.Register( WM_TREE_CHECKBOX_CLICK, &CExprolerPanel::OnTreeCheckBoxClick );

		CDocument::Instance().Open( _T("param.txt") );

		return TRUE;
	}

	LRESULT	OnTreeCheckBoxClick( WPARAM wParam, LPARAM lParam ){
		HTREEITEM	hParent	= (HTREEITEM)wParam;
		HTREEITEM	hChild	= (HTREEITEM)lParam;

		TCHAR		buf[256];
		
		if( m_TreeView.GetItemText(hParent, buf, _countof(buf)) )
		{
			DBG::TRACE(_T("%s"),buf);
			TString		strName(buf);

			if( m_TreeView.GetItemText(hChild, buf, _countof(buf)) )
			{
				strName.AppendFormat(_T(".%s\0"),buf);

				m_TreeView.Select( hChild );


				if( m_TreeView.GetCheckState( hChild ) ){
					CDocument::Instance().Selected( strName, TRUE );
				}else{
					CDocument::Instance().Selected( strName, FALSE );
				}
			}
		}
		return 0;
	}

	void	OnNmClick( LPNMHDR lpNmhdr ){
		DWORD	dwPos	= ::GetMessagePos();
		TVHITTESTINFO	hit = {0};
		hit.pt.x	= GET_X_LPARAM(dwPos);
		hit.pt.y	= GET_Y_LPARAM(dwPos);
		::MapWindowPoints( HWND_DESKTOP, lpNmhdr->hwndFrom, &hit.pt, 1 );
		HTREEITEM hItem = TreeView_HitTest( lpNmhdr->hwndFrom, &hit );
		if( TVHT_ONITEMSTATEICON & hit.flags )
		{
			HTREEITEM	hParent = m_TreeView.GetParent( hItem );
			DBG::TRACE(_T("hit:%d %d"),hItem,hParent);

			if( hParent == NULL )
			{
				DBG::TRACE(_T("Parent"));
			}
			else
			{
				DBG::TRACE(_T("Child"));
				PostMessage( WM_TREE_CHECKBOX_CLICK, (WPARAM)hParent, (LPARAM)hItem );
			}
		}
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

	void	OnNotify( WPARAM wParam, LPARAM lParam ){
		notify.Dispach(wParam, lParam);
	}

	void	OnWndMsg( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		msg.Dispach(uMsg, wParam, lParam);
	}

	void	Notify(){
		if( CDocument::Instance().IsUpdateTree() )
		{
			std::map<TString,BOOL>	ItemChecked;

			TCHAR	buf1[256], buf2[256];

			HTREEITEM	hParent	= m_TreeView.GetRoot();
			while( hParent )
			{
				m_TreeView.GetItemText( hParent, buf1, _countof(buf1) );

				HTREEITEM	hChild	= m_TreeView.GetChild( hParent );
				while( hChild )
				{
					m_TreeView.GetItemText( hChild, buf2, _countof(buf2) );
					BOOL	bChecked	= m_TreeView.GetCheckState( hChild );

					TString	str;
					str.AppendFormat(_T("%s.%s\0"), buf1, buf2);

					ItemChecked.insert( std::pair<TString,BOOL>(str, bChecked) );

					hChild	= m_TreeView.GetSibling( hChild );
				}

				hParent	= m_TreeView.GetSibling( hParent );
			}

			m_TreeView.DeleteAllItems();

			{
				std::map<TString,BOOL>::iterator it = ItemChecked.begin();
				for( ; it!=ItemChecked.end(); it++ )
				{
					TString	str( it->first );
					str.AppendFormat(_T(": %d\0"), it->second );
					DBG::TRACE(_T("\t%s"), (LPCTSTR)str);
				}
			}

		//	HTREEITEM	hRoot		= m_TreeView.InsertRoot( _T("") );

			CDocument::TREE& tree = CDocument::Instance().GetTree();
			CDocument::TREE::reverse_iterator	it = tree.rbegin();

			for( ; it!=tree.rend(); it++ )
			{
				DBG::TRACE(_T("%s"), (LPCTSTR)(TString)(it->first) );

				HTREEITEM	hRoot		= m_TreeView.InsertRoot( (LPTSTR)(TString)(it->first) );

				TStringList::iterator	list = it->second.begin();
				for( ; list!=it->second.end(); list++ )
				{
					DBG::TRACE(_T(".%s"), (LPCTSTR)&list->at(0));

					HTREEITEM	hItem	= m_TreeView.InsertItem( hRoot, (LPTSTR)&list->at(0) );
					if( hItem )
					{
					//	m_TreeView.SetCheckState( hItem, TRUE );
					}
				}
			}

			// AllExpand
			{
				HTREEITEM	hItem	= m_TreeView.GetRoot();
				while( hItem ){
					m_TreeView.Expand( hItem );
					hItem	= m_TreeView.GetSibling( hItem );
				}
			}
		}

		if( CDocument::Instance().IsUpdateTable() )
		{
			CListView&	ListView = m_wndGrid.GetListView();
			ListView.DeleteAllItems();

			CDocument::TABLE&	tbl = CDocument::Instance().GetTable();
			LPTSTR	lpName	= tbl[ 0 ][ 0 ];
			DBG::TRACE( lpName );
			ListView.InsertItem( 0, lpName );

			CDocument::LIST&	items	= CDocument::Instance().GetSelctedItem();
			CDocument::LIST::iterator	it = items.begin();
			for( int i=0; it!=items.end(); i++, it++ ){
				ListView.InsertItem( i, *it );
			}
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

		CDocument::Instance().add( &m_wndInput );
		CDocument::Instance().add( &m_wndExproler );

		if( !m_wndInput.Create(this, 0, 0, 300, 30) ){
			return FALSE;
		}

		if( !m_wndExproler.Create(this, 0, 0, 300, 300) ){
			return FALSE;
		}



		SetFontChildren();

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
	if( Form.Create(_T("TableData"),0,0,100,100,600,600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
