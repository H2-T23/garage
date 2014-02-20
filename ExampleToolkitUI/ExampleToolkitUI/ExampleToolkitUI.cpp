// ExampleToolkitUI.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleToolkitUI.h"

#include "UseGUIToolkit.h"

/**********************************************************************************
 *
 *
 *
 */
class CModel : public CSubject {
private:
	CModel( void ){}
	CModel( const CModel& );
	CModel& operator=( const CModel& );

public:
	typedef	std::map<TString, TStringList>	NodeList;

protected:
	NodeList	node;

public:
	void	appnedNode( TString& strParent, TString& strChild ){
		node[ strParent ].push_back( strChild );
	}

	void	trace( void ) {
		TString	str;

		NodeList::iterator it = node.begin();
		for( ; it!=node.end(); it++ ){
			TStringList::iterator	it2 = (it->second).begin();
			for( ; it2!=(it->second).end(); it2++ ){
				str.Format( _T("[%s][%s]"), (it->first).c_str(), it2->c_str() );
				DBG::TRACE( str );
			}
		}
	}

	NodeList&	GetNodeList( void ) {
		return node;
	}

public:
	static CModel&	Instance( void ){
		static CModel	obj;
		return obj;
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CAddNodePanel : public CPanel, public IObservable {
public:
	enum {
		IDC_EDIT_PARENT		= 1001	,
		IDC_EDIT_CHILD				,
		IDC_BUTTON_ADD				,
	};

protected:
	CEdit		m_edtParent;
	CEdit		m_edtChild;
	CButton		m_btnAdd;

	TCommandHandler<CAddNodePanel>	cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( !m_edtParent.Create(this, 1, 1, 100, 25, IDC_EDIT_PARENT) )
			return FALSE;
			 
		if( !m_edtChild.Create(this, 105, 1, 100, 25, IDC_EDIT_CHILD) )
			return FALSE;

		if( !m_btnAdd.Create(this, 210, 1, 40, 25, IDC_BUTTON_ADD, _T("ADD")) )
			return FALSE;

		m_edtParent	.SendMessage( WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0 );
		m_edtChild	.SendMessage( WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0 );
		m_btnAdd	.SendMessage( WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0 );

		cmd.Initialize( this );
		cmd.Register( IDC_BUTTON_ADD, BN_CLICKED, &CAddNodePanel::OnButtonClicked ); 

		return TRUE;
	}

	void	OnButtonClicked( void ){
		DBG::TRACE(_T("BN_CLICKED"));

		TCHAR	szParent[128];
		TCHAR	szChild[128];

		ZeroMemory(szParent,sizeof(szParent));
		ZeroMemory(szChild,128);

		::GetWindowText(m_edtParent.m_hWnd, szParent, 128);

		m_edtParent.GetText(szParent,128);
		m_edtChild.GetText(szChild,128);

		TString	strParent(szParent), strChild(szChild);

		CModel::Instance().appnedNode( strParent, strChild );
		CModel::Instance().trace();

		CModel::Instance().NotifyAll();
	}

	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);
	}

	void	Notify( void ){
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public CForm, public IObservable {
public:
	enum {
		IDC_HEADER1			= 1101	,
		IDC_TREEVIEW1				,
		IDC_LISTVIEW1				,
	};

protected:
	CAddNodePanel		m_wndAddNode;
	CHeaderCtrl			m_header;
	CTreeView			m_treeview;
	CListView			m_listview;

	TNotifyHandler<CMainForm>	notify;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rc;
		GetClientRect( &rc );

		int nLWidth	= rc.Width()/10*3;
		int nRWidth	= rc.Width()-nLWidth;

		m_wndAddNode.Create(this, 0, 0, 0, 0);

		if( m_header.Create(this, 0, 0, 0, 0, IDC_HEADER1) ){
			m_header.InsertItem( 0, _T("TreeView") );
			m_header.InsertItem( 1, _T("ListView") );

			m_header.SetSize(0, 30, rc.Width(), 25);
			m_header.SetColumnWidth( 0, nLWidth );
			m_header.SetColumnWidth( 1, nRWidth );
		}

		if( m_treeview.Create(this, 0, 0, 0, 0, IDC_TREEVIEW1) ){
			HTREEITEM	hRoot	= m_treeview.InsertRoot( _T("ROOT") );
			HTREEITEM	hParent	= m_treeview.InsertItem( hRoot, _T("PARENT") );
			m_treeview.InsertItem( hParent, _T("CHILD1") );
			m_treeview.InsertItem( hParent, _T("CHILD2") ); 
		}

		if( m_listview.Create(this, 0, 0, 0, 0, IDC_LISTVIEW1) ){
			m_listview.InsertColumn( 0, _T("column1") );
			m_listview.InsertColumn( 1, _T("column2") );
			m_listview.InsertColumn( 2, _T("column3") );
			m_listview.InsertColumn( 3, _T("column4") );
		}

		notify.Initialize( this );
		notify.Register( HDN_ENDTRACK, IDC_HEADER1, &CMainForm::OnEndTrack );

		CModel::Instance().add( this );
		CModel::Instance().add( &m_wndAddNode );
		return TRUE;
	}

	void	OnSize( UINT state, int cx, int cy ){
		if( cx <= 0 || cy <= 0 )
			return;

		CRect	rc;
		GetClientRect( &rc );
		
		int	nWidth	= rc.Width();
		int nLWidth	= m_header.GetColumnWidth( 0 );
		int nRWidth	= nWidth - nLWidth;
		
		m_wndAddNode.MoveWindow( 0, 0, nWidth, 30 );

		m_header.SetSize( 0, 30, nWidth, 25 );
		m_header.SetColumnWidth( 1, nRWidth );

		m_treeview.MoveWindow( 0		, 53, nLWidth, rc.Height()-54 );
		m_listview.MoveWindow( nLWidth	, 53, nRWidth, rc.Height()-54 );
	}

	void	OnEndTrack( LPNMHDR lpNmhdr ){
		CRect	rc;
		CPoint	pt;
		
		LPNMHEADER	lpNmHeader	= (LPNMHEADER)lpNmhdr;
		switch( lpNmHeader->iItem ){
		case 0:
		{
			GetClientRect( &rc );

			int	nWidth	= rc.Width();
			int	nLWidth	= lpNmHeader->pitem->cxy;
			int	nRWidth	= nWidth - nLWidth;

			m_treeview.GetWindowRect( &rc );
			pt	= rc.TopLeft();
			ScreenToClient( &pt );
			m_treeview.MoveWindow( pt.x		, pt.y, nLWidth, rc.Height() );
			m_listview.MoveWindow( nLWidth	, pt.y, nRWidth, rc.Height() );

			GetClientRect( &rc );
			m_header.SetColumnWidth( 1, nRWidth );
			break;
		}
		case 1:
			break;
		}
	}

	void	OnNotify( WPARAM wParam, LPARAM lParam ){
		notify.Dispach(wParam, lParam);
	}

	void	Notify( void ){
		m_treeview.DeleteAllItems();

		CModel::NodeList&	node = CModel::Instance().GetNodeList();

		CModel::NodeList::iterator	itA = node.begin();
		for( ; itA!=node.end(); itA++ ){
			HTREEITEM hParent = m_treeview.InsertRoot( (LPTSTR)(itA->first).c_str() );

			TStringList::iterator	itB	= itA->second.begin();
			for( ; itB!=itA->second.end(); itB++ ){
				m_treeview.InsertItem( hParent, (LPTSTR)itB->c_str() );
			}

			m_treeview.Expand( hParent );
		}

		UpdateWindow();
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
	CMainForm	form;
	if( form.Create(_T("")) ){
		form.ShowWindow( SW_SHOW );
		form.UpdateWindow();
		return form.MessageLoop();
	}
	return 0;
}
