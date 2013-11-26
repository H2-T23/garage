
// Mdb2TxtDlg.h : ヘッダー ファイル
//

#pragma once

template<typename T>
class TMovableWnd : public T
{
private:
	CPoint		m_ptPrev, m_ptCurr;
	bool		m_bMoving;
	
	void		InitMovableWnd(){
		m_bMoving = false;
		m_ptPrev.SetPoint( 0, 0 );
		m_ptCurr.SetPoint( 0, 0 );
	}

public:
	TMovableWnd( void ) : T(){
		InitMovableWnd();
	}

	TMovableWnd(CWnd* pParent = NULL) : T(pParent){
		InitMovableWnd();
	}

	TMovableWnd(UINT nIDTemplate, CWnd* pParentWnd = NULL) : T(nIDTemplate, pParentWnd){
		InitMovableWnd();
	}

	virtual BOOL	PreTranslateMessage( MSG* pMsg ){
		if( pMsg->hwnd == m_hWnd )
		{
			CPoint	point( pMsg->pt.x, pMsg->pt.y );

			switch( pMsg->message ){
			case WM_LBUTTONDOWN:
				m_bMoving = true;
				m_ptPrev = point;
				SetParent( NULL );
				SetCapture();
				break;

			case WM_LBUTTONUP:
				{
					::ReleaseCapture();
					m_bMoving = false;

					CWnd* pWnd = CWnd::WindowFromPoint( point );
					if( pWnd != NULL && IsWindow(pWnd->m_hWnd) )
					{
						SetParent( pWnd );
					}
				}
				break;

			case WM_MOUSEMOVE:
				if( m_bMoving )
				{
					CRect	rc(0,0,0,0);
					GetWindowRect( &rc );

					m_ptCurr.SetPoint( point.x, point.y );

					CPoint	pt(	m_ptCurr.x - m_ptPrev.x
							,	m_ptCurr.y - m_ptPrev.y	);

					TRACE( _T("[%d, %d]\n"), pt.x, pt.y );

					rc.top		+= pt.y;
					rc.bottom	+= pt.y;
					rc.left		+= pt.x;
					rc.right	+= pt.x;

					MoveWindow( &rc );

					m_ptPrev = m_ptCurr;
				}
				break;
			}
		}
		return T::PreTranslateMessage( pMsg );
	}
};


// CMdb2TxtDlg ダイアログ
class CMdb2TxtDlg : public TMovableWnd<CDialog>
{
// コンストラクション
public:
	CMdb2TxtDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_MDB2TXT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
