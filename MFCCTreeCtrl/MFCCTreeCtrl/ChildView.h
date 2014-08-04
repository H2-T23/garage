
// ChildView.h : CChildView クラスのインターフェイス
//


#pragma once
#include "TreeCtrlEx.h"


// CChildView ウィンドウ

class CChildView : public CWnd {
// コンストラクション
public:
	CChildView();

// 属性
public:
	CImageList			m_ImageList;
	CTreeCtrlEx			m_TreeCtrl;

// 操作
public:

// オーバーライド
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 実装
public:
	virtual ~CChildView();

	// 生成された、メッセージ割り当て関数
protected:
	afx_msg void OnPaint();
	afx_msg int		OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( __super::OnCreate(lpCreateStruct) == -1 ){
			return -1;
		}


		DWORD	dwStyle	= (WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);
		if( !m_TreeCtrl.Create(dwStyle,CRect(0,0,0,0),this,1001) ){
			return -1;
		}

		m_ImageList.Create( 32, 32, TRUE, ILC_COLOR8, 1 );
		m_ImageList.Add( (HICON)LoadImage(NULL, _T("setup.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE) );

		m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );


		HTREEITEM hRoot	= m_TreeCtrl.InsertItem( _T("Root") );
		HTREEITEM hParent	= m_TreeCtrl.InsertItem( _T("Item1"), hRoot );
		if( hParent ){
			m_TreeCtrl.InsertItem( _T("Item1.Sub1"), hParent );
			m_TreeCtrl.InsertItem( _T("Item1.Sub2"), hParent );
			m_TreeCtrl.InsertItem( _T("Item1.Sub3"), hParent );
		}
		hParent	= m_TreeCtrl.InsertItem( _T("Item2"), hRoot );
		if( hParent ){
			m_TreeCtrl.InsertItem( _T("Item2.Sub1"), hParent );
			m_TreeCtrl.InsertItem( _T("Item2.Sub2"), hParent );
			m_TreeCtrl.InsertItem( _T("Item2.Sub3"), hParent );
		}
		hParent	= m_TreeCtrl.InsertItem( _T("Item3"), hRoot );
		if( hParent ){
			m_TreeCtrl.InsertItem( _T("Item3.Sub1"), hParent );
			m_TreeCtrl.InsertItem( _T("Item3.Sub2"), hParent );
			m_TreeCtrl.InsertItem( _T("Item3.Sub3"), hParent );
		}

		return 0;
	}
	afx_msg void	OnSetFocus( CWnd* /*pOldWnd*/ ){
		m_TreeCtrl.SetFocus();
	}
	afx_msg void	OnSize( UINT nFlags, int cx, int cy ){
		__super::OnSize(nFlags, cx, cy);
		m_TreeCtrl.MoveWindow(0,0,cx,cy);
	}
	DECLARE_MESSAGE_MAP()
public:
	
};

