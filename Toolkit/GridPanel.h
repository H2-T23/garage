#pragma once

#include "Trace.h"
#include "Panel.h"
#include "ComponentWnd.h"

/**********************************************************************************
 *
 *
 *
 */
class CGridPanel : public CPanel {
public:
	enum { IDC_EDITLABEL = 1001, IDC_LISTVIEW };

private:
	static 
	LRESULT CALLBACK	EditProc(	HWND		hWnd
								,	UINT		uMsg
								,	WPARAM		wParam
								,	LPARAM		lParam
								,	UINT_PTR	uIDSubclass
								,	DWORD_PTR	dwRefData 
								)
	{
		if( (uIDSubclass == 1) && (dwRefData != NULL) )
		{
			if( (uMsg == WM_KEYDOWN) && (VK_RETURN == wParam)
			||	(uMsg == WM_KILLFOCUS)
			){
				CGridPanel* pGridPanel = (CGridPanel*)dwRefData;
				pGridPanel->EndEditLabel();
				return 0;
			}
		}
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

protected:
	CEdit			m_edtLabel;
	CListView		m_ListView;
	bool			m_bEdit;
	int				m_nItem, m_nSubItem;

	TNotifyHandler<CGridPanel>	notify;


	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		ModifyStyle( WS_BORDER, 0 );

		m_bEdit	= false;
		m_nItem = m_nSubItem = -1;
		m_edtLabel.Create(this, 0,0,0,0, IDC_EDITLABEL);
		m_edtLabel.SetFont();
		m_edtLabel.ShowWindow( SW_HIDE );

		m_ListView.Create(this, 0,0,0,0, IDC_LISTVIEW);

		notify.Initialize( this );
		notify.Register( NM_DBLCLK	, IDC_LISTVIEW, &CGridPanel::OnListViewDblClk	);
		notify.Register( NM_SETFOCUS, IDC_LISTVIEW, &CGridPanel::OnListViewSetFocus	);

		return TRUE;
	}

	void	OnNotify( WPARAM wParam, LPARAM lParam ){
		notify.Dispach(wParam, lParam);
	}

	void	OnSize( UINT state, int cx, int cy ){
		if( cx < 0 || cy < 0 )
			return;

		m_ListView.MoveWindow( 0, 0, cx, cy );
	}

	void	OnSetFocus( HWND hwnd ){
		if( m_hWnd == hwnd )
			m_ListView.SetFocus();
		else
			EndEditLabel();
	}

	void	OnKillFocus( HWND hwnd ){
		EndEditLabel();
	}

	void	OnListViewDblClk( LPNMHDR lpNmhdr ){
		LPNMITEMACTIVATE pItem = (LPNMITEMACTIVATE)lpNmhdr;
		if( pItem ){
			TRACE(_T("NM_CLICK(%d, %d)[%d, %d]"), pItem->iItem, pItem->iSubItem, pItem->ptAction.x, pItem->ptAction.y );

			CRect	rc;
			int		nItem		= pItem->iItem;
			int		nSubItem	= pItem->iSubItem;

			if( nItem >= 0 )
			{
				m_ListView.GetItemRect( nItem, &rc );
			}
			else
			{
				if( nSubItem == 0 )
				{
					POINT	pt;
					GetCursorPos( &pt );
					ScreenToClient( &pt );
					LVHITTESTINFO	hit = {0};
					hit.pt	= pt;
					m_ListView.HitTest( &hit );
					TRACE(_T("ItemHitTest()=(%d, %d)"), hit.iItem, hit.iSubItem );
				}
				else
				{
					LVHITTESTINFO	hit = {0};
					hit.pt	= pItem->ptAction;
					m_ListView.SubItemHitTest( &hit );
					TRACE(_T("SubItemHitTest()=(%d, %d)"), hit.iItem, hit.iSubItem );

					nItem		= hit.iItem;
					nSubItem	= hit.iSubItem;
					m_ListView.GetSubItemRect( nItem, nSubItem, &rc );
				}
			}

			TRACE(_T("RECT(%d, %d, %d, %d)"), rc.X(), rc.Y(), rc.Width(), rc.Height() );
			BeginEditLabel( nItem, nSubItem );
		}
	}

	void	OnListViewSetFocus( LPNMHDR lpNmhdr ){
		if( m_bEdit )
			EndEditLabel();
	}

public:
	CListView&	GetListView( void ){ return(m_ListView); }

	void		BeginEditLabel( int nItem, int nSubItem = 0 ){
		if( nItem < 0 && nSubItem < 0 )
			return;

		CRect	rc;
		if( nSubItem == 0 ){
			m_ListView.GetItemRect( nItem, &rc );
		}else{
			m_ListView.GetSubItemRect( nItem, nSubItem, &rc );
		}

		if( rc.IsRectEmpty() )
			return;

		m_bEdit			= true;
		m_nItem			= nItem;
		m_nSubItem		= nSubItem;

		TCHAR	szBuf[256];
		m_ListView.GetItemText( m_nItem, m_nSubItem, szBuf, 256 );

		m_edtLabel.SetWindowText( szBuf );
		m_edtLabel.SetSel( 0, 255 );
		m_edtLabel.MoveWindow( rc.X(), rc.Y(), rc.Width(), rc.Height(), FALSE );
		m_edtLabel.ShowWindow( SW_SHOW );
		m_edtLabel.SetFocus();
		::SetWindowSubclass( m_edtLabel.m_hWnd, EditProc, 1, (DWORD_PTR)this );
	}

	void		EndEditLabel( void ){
		if( m_bEdit	== false )
			return;

		TCHAR	szBuf[256];

		::RemoveWindowSubclass( m_edtLabel.m_hWnd, EditProc, 1 );
		m_edtLabel.ShowWindow( SW_HIDE );
		m_edtLabel.GetWindowText( szBuf, 256 );

		m_ListView.SetItem( m_nItem, m_nSubItem, szBuf );
		SetFocus();

		m_bEdit	= false;
		m_nItem = m_nSubItem = -1;
	}
};