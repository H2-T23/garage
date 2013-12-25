#include "UseGUIToolkit.h"

#include "OpneGL.h"

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
class CCtrlPanel : public TCommandListener<CCtrlPanel,CPanel> {
private:
	void		OnBtnAClick( int nID ){
		if( nID == 1011 ){
		}
	}

	void		OnBtnBClick( int nID ){
		if( nID == 1012 ){
		}
	}

public:
	CCtrlPanel( void ){
		AddCommandListener( 1011, &CCtrlPanel::OnBtnAClick );
		AddCommandListener( 1012, &CCtrlPanel::OnBtnBClick );
	}

protected:
	CLabel			label;
	CButton			btnA, btnB;
	CListView		listview;
	CHeaderCtrl		header;
	CEdit			editA;
	CUpDown			updownA;

	CGroupBox		groupbox;
	CCheckBox		checkbox1, checkbox2;
	CRadioButton	radio1, radio2;

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

		rc.Set( 120, 10, 300, 100 );
		if( listview.Create(this, rc, 1100) ){
			listview.InsertColumn( 0, _T("Col1") );
			listview.InsertColumn( 1, _T("Col2") );
			listview.InsertItem( 0, 0, _T("Item1") );
			listview.InsertItem( 1, 0, _T("Item2") );
			listview.SetItem( 0, 1, _T("SubItem1") );
			listview.SetItem( 1, 1, _T("SubItem2") );
		}

		rc.Set( 10, 200, 300, 50 );
		if( header.Create(this, rc, 1111) ){
			header.InsertItem( 0, _T("Name") );
			header.InsertItem( 1, _T("Image") );
		}

		rc.Set( 10, 280, 400, 100 );
		if( groupbox.Create(this, rc, 1200, _T("GroupBox")) ){
			radio1.Create(&groupbox, 10, 30, 130, 30, 1201, _T("check1"));
			radio2.Create(&groupbox, 10, 60, 130, 30, 1202, _T("check2"));
		}

		return TRUE;
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CHeaderPanel : public CPanel {
protected:
	CHeaderCtrl		header;

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		if( header.Create(this, 0, 0, 0, 0, 1000, NULL, (WS_CHILD | WS_BORDER | HDS_BUTTONS)) ){
			header.InsertItem( 0, _T("NAME") );
			header.InsertItem( 1, _T("IMAGE") );
		}

		return TRUE;
	}

	void	OnSize(  UINT state, int cx, int cy ){
		CRect		rc(0, 0, cx, cy);
		WINDOWPOS	wpos;
		header.SetLayout( rc, wpos );
		header.SetWindowPos( wpos.hwndInsertAfter, wpos.x, wpos.y, wpos.cx, wpos.cy, (wpos.flags | SWP_SHOWWINDOW) );
	}

	void	OnNotify( WPARAM wParam, LPARAM lParam ){
	
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CDrawPanel : public CPanel {
protected:
	void	OnPaint( HDC hDC ){
		CDC		dc(hDC);

		CRect	rc(10, 10, 100, 100);

		dc.DrawRect( rc, RGB(0xFF,0x00,0xFF) );
	}
};


/**********************************************************************************
 *
 *
 *
 */
class COpenGLPanel : public CPanel {
protected:
	virtual DWORD		WindowStyle( void ) const { return(CPanel::WindowStyle() | WS_CLIPSIBLINGS | WS_CLIPCHILDREN); }

	COpenGL			GL;

	void	subAxis( float d, float s ){
		const double	pi = 3.14;

		glBegin( GL_QUAD_STRIP );
		{
			for( double i=0; i<=6.0; i++ ){
				double t	= (i * 2 * pi / 6);
				glNormal3f( cos(t), 0, sin(t) );
				glVertex3f( d * cos(t), -s, d * sin(t) );
				glVertex3f( d * cos(t),  s, d * sin(t) );
			}
		}
		glEnd();

		glTranslatef( 0, s, 0 );
		glRotatef( -90, 1, 0, 0 );
	}

	void	Axis( float d, float s ){
		glPushMatrix();
		subAxis( d, s );
		glPopMatrix();

		glPushMatrix();
		glRotatef( 90, 1, 0, 0 );
		subAxis( d, s );
		glPopMatrix();

		glPushMatrix();
		glRotatef( -90, 0, 0, 1 );
		subAxis( d, s );
		glPopMatrix();
	}

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
	
		if( GL.Startup( GetDC(m_hWnd) ) ){
			glClearColor( 0, 0, 0.5, 1 );
			glClearDepth( 1 );
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_CULL_FACE );
		}

		return TRUE;
	}

	virtual void	OnPaint( HDC hDC ){
		CDC	dc(hDC);

		wglMakeCurrent( hDC, GL.m_hRC );
#if 0
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glColor3f( 1, 0, 1 );
		glRectd(-0.5,-0.5,0.5,0.5);
#else

	//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		static GLfloat vertices [8][3] =
		{
			{-0.5f, -0.5f,  0.5f},
			{ 0.5f, -0.5f,  0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{ 0.5f, -0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},
			{ 0.5f,  0.5f, -0.5f}
		};


		Axis( 0.05, 2.0 );

		// ‘O
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[0]);
			glVertex3fv(vertices[1]);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[2]);
			glVertex3fv(vertices[3]);
		glEnd();

		// Œã
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[4]);
			glVertex3fv(vertices[5]);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[6]);
			glVertex3fv(vertices[7]);
		glEnd();

		// ‰E
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[1]);
			glVertex3fv(vertices[4]);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[7]);
			glVertex3fv(vertices[2]);
		glEnd();

		// ¶
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[5]);
			glVertex3fv(vertices[0]);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[3]);
			glVertex3fv(vertices[6]);
		glEnd();

		// ã
//		glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[3]);
			glVertex3fv(vertices[2]);
			glVertex3fv(vertices[7]);
			glVertex3fv(vertices[6]);
		glEnd();

		// ‰º
//		glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[1]);
			glVertex3fv(vertices[0]);
			glVertex3fv(vertices[5]);
			glVertex3fv(vertices[4]);
		glEnd();
#endif
		glFinish();
		SwapBuffers( hDC );
		wglMakeCurrent( NULL, NULL );
	}

	virtual void	OnSize( UINT state, int cx, int cy ) {
		if( cx == 0 && cy == 0 )
			return;

		CRect	rc;
		GetClientRect( &rc );

		GLfloat	w	= 0.6 * rc.Width();
		GLfloat	h	= 0.6 * rc.Height();

		glViewport( 100, 100, w, h );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		gluPerspective(45, w/h, 0.1, 100);

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( 0.5, 1.5, 2.5, 0, 0, 0, 0, 1, 0 );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CTabPanel : public CPanel {
protected:
	CTabCtrl			tabctrl;
	CDrawPanel			wndDraw;
	CHeaderPanel		wndHeader;
	CListViewPanel		wndListView;
	CCtrlPanel			wndCtrl;
	COpenGLPanel		wndOpenGL;

	typedef std::vector<CPanel*>	VecPanel;
	VecPanel	vecPanel;

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rc;
		GetClientRect( &rc );

		wndDraw.Create( this, rc );
		wndDraw.ShowWindow( SW_HIDE );

		wndHeader.Create( this, rc );
		wndHeader.ShowWindow( SW_HIDE );

		wndListView.Create( this, rc );
		wndListView.ShowWindow( SW_HIDE );

		wndCtrl.Create( this, rc );
		wndListView.ShowWindow( SW_HIDE );

		wndOpenGL.Create( this, rc );
		wndOpenGL.ShowWindow( SW_HIDE );

		vecPanel.push_back( &wndDraw		);
		vecPanel.push_back( &wndHeader		);
		vecPanel.push_back( &wndListView	);
		vecPanel.push_back( &wndCtrl		);
		vecPanel.push_back( &wndOpenGL		);

		if( tabctrl.Create(this, rc, 200) ){
			tabctrl.InsertItem( 0, _T("AAAA") );
			tabctrl.InsertItem( 1, _T("BBBB") );
			tabctrl.InsertItem( 2, _T("CCCC") );
			tabctrl.InsertItem( 3, _T("DDDD") );
			tabctrl.InsertItem( 4, _T("EEEE") );
			tabctrl.InsertItem( 5, _T("FFFF") );
		}

		TabChange( 0 );

		return TRUE;
	}

	void		TabChange( int nCurSel ){
		if( tabctrl.GetCurSel() != nCurSel )
			tabctrl.SetCurSel( nCurSel );

		VecPanel::iterator it = vecPanel.begin();
		for( int i=0; it!=vecPanel.end(); i++, it++ ){
			(*it)->ShowWindow( nCurSel == i ? SW_SHOW : SW_HIDE );
		}
	}

	void		Resize(){
		CRect	rc;
		GetClientRect( &rc );
		
		tabctrl.MoveWindow( rc.X(), rc.Y(), rc.Width(), rc.Height(), TRUE );
		tabctrl.GetClientRect( &rc );
		tabctrl.AdjustRect( FALSE, rc );

		rc.X( rc.X() + 5 );
		rc.Y( rc.Y() + 5 );
		rc.Width( rc.Width() - 5 );
		rc.Height( rc.Height() - 5 );

		VecPanel::iterator it = vecPanel.begin();
		for( int i=0; it!=vecPanel.end(); i++, it++ ){
			(*it)->MoveWindow(rc.X(), rc.Y(), rc.Width(), rc.Height(), TRUE );
		}
	}

	virtual void	OnMove( int x, int y) {
		if( x && y )
			Resize();
	}

	virtual void	OnSize( UINT state, int cx, int cy ) {
		if( cx && cy )
			Resize();
	}

	virtual void	OnNotify( WPARAM wParam, LPARAM lParam ){
		LPNMHDR lpNmhdr = (LPNMHDR)lParam;
		if( lpNmhdr == NULL ){
			return ;
		}

		if( TCN_SELCHANGE == lpNmhdr->code )
		{
			TabChange( tabctrl.GetCurSel() );
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
	CTabPanel		wndTab;
	
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		CComponentWnd::InitCommCtrlEx();

		CRect	rect( 10, 10, 800, 600 );
		wndTab.Create( this, rect );

		return TRUE;
	}

	void	OnSize( UINT state, int x, int y ){
		if( x == 0 && y == 0 )
			return;

		CRect	rc;
		GetClientRect( &rc );
		wndTab.MoveWindow( rc.X(), rc.Y(), rc.Width(), rc.Height(), TRUE );
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