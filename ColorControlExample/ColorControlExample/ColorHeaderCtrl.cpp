// ColorHeaderCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ColorControlExample.h"
#include "ColorHeaderCtrl.h"

//IMPLEMENT_DYNAMIC(CHeaderCtrlEx, CHeaderCtrl)
//BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
//END_MESSAGE_MAP()
//
//CHeaderCtrlEx::CHeaderCtrlEx()
//{
//}
//
//CHeaderCtrlEx::~CHeaderCtrlEx()
//{
//}


IMPLEMENT_DYNAMIC(CColorHeaderCtrl, CHeaderCtrl)
BEGIN_MESSAGE_MAP(CColorHeaderCtrl, CHeaderCtrl)
//	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CColorHeaderCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()

CColorHeaderCtrl::CColorHeaderCtrl()
{
}

CColorHeaderCtrl::~CColorHeaderCtrl()
{
}

void CColorHeaderCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
#if 0
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	int	iItem		= (int)pNMCD->nmcd.dwItemSpec;
	int iSubItem	= pNMCD->iSubItem;

	switch( pNMCD->nmcd.dwDrawStage ){
	case CDDS_PREPAINT:
		*pResult	= CDRF_NOTIFYSUBITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		*pResult	= CDRF_NOTIFYSUBITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		pNMCD->clrText		= GetTextColor( iItem );
		pNMCD->clrTextBk	= GetBkColor( iItem );
		*pResult	= CDRF_NEWFONT;
		break;

	default:
		*pResult	= CDRF_DODEFAULT;
		break;
	}
#else
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	switch( pNMCD->dwDrawStage ){
	case CDDS_PREPAINT:
		*pResult	= CDRF_NOTIFYSUBITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		*pResult	= CDRF_NOTIFYSUBITEMDRAW;
		if( pNMCD->dwItemSpec >= 0 ){
			CDC* pDC = CDC::FromHandle( pNMCD->hdc );
			if( pDC ){

				pDC->SetBkMode( TRANSPARENT );
			//	CBrush brush( GetBkColor(pNMCD->dwItemSpec) ); 
			//	pDC->FrameRect( &pNMCD->rc, &brush );
				pDC->FillSolidRect( &(pNMCD->rc), GetBkColor( pNMCD->dwItemSpec ) );
				*pResult	= CDRF_NEWFONT;
			}
		}
		break;
	
	default:
		*pResult	= CDRF_DODEFAULT;
		break;
	}
#endif
}

BOOL CColorHeaderCtrl::SubclassWindow(HWND hWnd)
{
	BOOL bResult = __super::SubclassWindow(hWnd);
	if( bResult )
	{
		HDITEM	hItem = {0};

		for( int i=0; i<GetItemCount(); i++ )
		{
			GetItem( i, &hItem );
			hItem.fmt |= HDF_OWNERDRAW;
			SetItem( i, &hItem );
		}
	}
	return bResult;
}

void CColorHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int		nItem	= lpDrawItemStruct->itemID;
	TCHAR	Buffer[256];

	HDITEM hItem = {0};
	hItem.mask = HDI_TEXT;
	hItem.pszText		= Buffer;
	hItem.cchTextMax	= sizeof(Buffer)/sizeof(TCHAR);

	GetItem( nItem, &hItem );

	CRect	rcItem( lpDrawItemStruct->rcItem );

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( pDC )
	{
		// 状態情報をコピーしてデバイスコンテキストの
		// 現在の状態（クリッピング領域、選択したオブジェクト、およびWindowsによって
		// 保持されるコンテキストのスタックのモードをマップするなど）に
		// 保存します。
		int nSavedDC = pDC->SaveDC();

		CString str( Buffer );

		pDC->DrawFrameControl( rcItem, DFC_BUTTON, DFCS_BUTTONPUSH );

		pDC->SetBkColor( GetBkColor( nItem ) );
		pDC->SetTextColor( GetTextColor( nItem ) );
		pDC->DrawText( str, &rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER );

		// nSavedCDによって識別される前の状態にデバイスコンテキストを復元します。
		pDC->RestoreDC( nSavedDC );
	}
}
