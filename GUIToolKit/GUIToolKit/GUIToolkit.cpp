#include "UseGUIToolkit.h"

/**********************************************************************************
 *
 *
 *
 */
class CListViewPanel : public CPanel {
protected:
	CVirtualListView	vlistview;
	TCHAR* lpData;
	CListView			chklistview;

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		lpData = NULL;

		CRect	rect;
		GetClientRect( &rect );

		int	h = rect.Height() / 2;

		rect.Height(  h - 1 );


		if( vlistview.Create(this, rect, 1200) ){
			vlistview.InsertColumn( 0, _T("col1") );

			const int	nCnt	= 58;
			lpData = (TCHAR*)HeapAlloc( GetProcessHeap(), 0, nCnt * sizeof(TCHAR) );
			for( int i=0; i<nCnt; i++ ){
				lpData[ i ]	= _T('A') + (TCHAR)i;
			}

			vlistview.SetItemCountEx( nCnt );
		}

		rect.top += h;
		rect.bottom += h;

		if( chklistview.Create(this, rect, 1300) ){
			chklistview.SetExtendedListViewStyleEx( LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES );

			chklistview.InsertColumn( 0, _T("col1") );
			chklistview.InsertItem( 0, 0, _T("Item1") );
			chklistview.InsertItem( 1, 0, _T("Item2") );
		}

		return TRUE;
	}

	virtual void	OnDestroy( void ){
		if( lpData ){
			HeapFree( GetProcessHeap(), 0, lpData );
		}
		CPanel::OnDestroy();
	}

	virtual LRESULT	WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ){

		switch( uMsg ){
		case WM_NOTIFY:
			{
				LPNMHDR	lpNmhdr = (LPNMHDR)lParam;
				if( !lpNmhdr ){
					return 0;
				}

				if( (lpNmhdr->code == LVN_GETDISPINFO) )
				{
					NMLVDISPINFO* lpDispInfo = (NMLVDISPINFO*)lParam;
					if( lpDispInfo && (lpDispInfo->item.mask & LVIF_TEXT) )
					{
						TCHAR	szBuf[ 256 ];
						wsprintf( szBuf, _T("%c"), lpData[ lpDispInfo->item.iItem ] );
						lstrcpy( lpDispInfo->item.pszText, szBuf );
					}
				}
				else
				if( (lpNmhdr->code == NM_CLICK)
				||	(lpNmhdr->code == NM_RCLICK)
				||	(lpNmhdr->code == NM_DBLCLK)
				){
					LVHITTESTINFO	hit;

					GetCursorPos( &hit.pt );
				//	ScreenToClient( lpNmhdr->hwndFrom, &hit.pt );
					chklistview.ScreenToClient( &hit.pt );

				//	chklistview.HitTest( lpNmhdr->hwndFrom, &hit );
					chklistview.HitTest( &hit );
					if( hit.flags & LVHT_ONITEMSTATEICON ){
						PostMessage( (WM_APP + 100), 0, (LPARAM)hit.iItem );
					}
				}
				else
				if(	(lpNmhdr->code == LVN_KEYDOWN)	)
				{
					if( ((LPNMLVKEYDOWN)lParam)->wVKey == VK_SPACE ){
						int nCnt = chklistview.GetItemCount();

						for( int i=0; i<nCnt; i++ ){
							if( chklistview.GetItemState(i, LVIS_SELECTED) ){
								PostMessage((WM_APP + 100), 0, (LPARAM)i);
								break;
							}
						}
					}
				}
			}
			return 0;
		
		case (WM_APP + 100):
			{
				TCHAR	szName[ 256 ];
				TCHAR	szBuff[ 256 ];

				LVITEM	item;
				item.mask		= int(lParam);
				item.iSubItem	= 0;
				item.pszText	= szName;
				item.cchTextMax	= sizeof(szName) / sizeof(TCHAR);
				chklistview.GetItem( item );

				if( chklistview.GetCheckState( int(lParam) ) ){
					wsprintf( szBuff, _T("%s Check"), szName );
					SetWindowText( m_hWnd, szBuff );
				}
				else{
					wsprintf( szBuff, _T("%s Uncheck"), szName );
					SetWindowText( m_hWnd, szBuff );
				}

			}
			return 0;
		}
		return CPanel::WindowProc(uMsg,wParam,lParam);
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CCtrlPanel : public CPanel {
protected:
	CLabel		label;
	CButton		btnA, btnB;
	CListView	listview;
	CHeaderCtrl	header;
	CEdit		editA;
	CUpDown		updownA;

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rc( 10, 10, 100, 30 );
		label.Create(this, rc, 1001, _T("Label"));

		rc.Set( 10, 50, 100, 30 );
		btnA.Create(this, rc, 1011, _T("A"));

		rc.Set( 10, 100, 100, 30 );
		btnB.Create(this, rc, 1012, _T("B"));

		rc.Set( 10, 150, 100, 30 );
		editA.Create(this, rc, 1013, _T("A"));
		if( updownA.Create(this, rc, 1014) ){
			updownA.SetBuddy( &editA );
			updownA.SetRange(10, 80);
			updownA.SetPos(40);
		}

		rc.Set( 120, 10, 300, 300 );
		if( listview.Create(this, rc, 1100) ){
			listview.InsertColumn( 0, _T("Col1") );
			listview.InsertColumn( 1, _T("Col2") );
			listview.InsertItem( 0, 0, _T("Item1") );
			listview.InsertItem( 1, 0, _T("Item2") );
			listview.SetItem( 0, 1, _T("SubItem1") );
			listview.SetItem( 1, 1, _T("SubItem2") );
		}

		rc.Set( 10, 200, 100, 50 );
		if( header.Create(this, rc, 1111) ){
			header.InsertItem( 0, _T("Name") );
			header.InsertItem( 1, _T("Image") );
		}

		return TRUE;
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CTabPanel : public CPanel {
protected:
	CTabCtrl	tabctrl;

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rc;
		GetClientRect( &rc );

		if( tabctrl.Create(this, rc, 200) ){
			tabctrl.InsertItem( 0, _T("AAAA") );
			tabctrl.InsertItem( 1, _T("BBBB") );
			tabctrl.InsertItem( 2, _T("CCCC") );
			tabctrl.InsertItem( 3, _T("DDDD") );
			tabctrl.InsertItem( 4, _T("EEEE") );
			tabctrl.InsertItem( 5, _T("FFFF") );
		}

		return TRUE;
	}

	virtual void	OnNotify( WPARAM wParam, LPARAM lParam ){
		LPNMHDR lpNmhdr = (LPNMHDR)lParam;
		if( lpNmhdr == NULL ){
			return ;
		}

		if( NM_RCLICK == lpNmhdr->code )
		{
			TCHITTESTINFO	info;
			GetCursorPos( &info.pt );
			tabctrl.ScreenToClient( &info.pt );

			int nIdx = tabctrl.HitTest( &info );
			if( nIdx >= 0 ){
				tabctrl.DeleteItem( nIdx );
				tabctrl.SetCurSel( 0 );
			}
		}

		if( TTN_GETDISPINFO == lpNmhdr->code )
		{
			LPTOOLTIPTEXT lpText = (LPTOOLTIPTEXT)lParam;

			if( 0 == lpText->hdr.idFrom ){
				lpText->lpszText	= _T("tab A");
			}else
			if( 1 == lpText->hdr.idFrom ){
				lpText->lpszText	= _T("tab B");
			}else
			if(	2 == lpText->hdr.idFrom ){
				lpText->lpszText	= _T("tab C");
			}else
			if( 3 == lpText->hdr.idFrom ){
				lpText->lpszText	= _T("tab D");
			}else
			if(	4 == lpText->hdr.idFrom ){
				lpText->lpszText	= _T("tab E");
			}else
			if( 5 == lpText->hdr.idFrom ){
				lpText->lpszText	= _T("tab F");
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
	CCtrlPanel		wndCtrlPanel;
	CListViewPanel	wndListViewPanel;
	CTabPanel		wndTab;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rect( 10, 10, 500, 400 );
		wndCtrlPanel.Create( this, rect );

		rect.Set( 10,  550, 400, 300 );
		wndListViewPanel.Create( this, rect );

		rect.Set( 350, 550, 400, 250 );
		wndTab.Create( this, rect );

		return TRUE;
	}
};

/**********************************************************************************
 *
 *
 *
 */
int APIENTRY _tWinMain(	HINSTANCE	hInstance
					,	HINSTANCE	hPrevInstance
					,	LPTSTR		lpCmdLine
					,	int			nCmdShow
					)
{
	CMainForm	form;
	if( form.Create(_T("Main")) ){
		form.ShowWindow( nCmdShow );
		form.UpdateWindow();
		return form.MessageLoop();
	}
	return 0;
}