// ColorListCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ColorControlExample.h"
#include "ColorListCtrl.h"


// CColorListCtrl

IMPLEMENT_DYNAMIC(CColorListCtrl, CListCtrl)

CColorListCtrl::CColorListCtrl()
{

}

CColorListCtrl::~CColorListCtrl()
{
}


BEGIN_MESSAGE_MAP(CColorListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CColorListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()



// CColorListCtrl メッセージ ハンドラー
void CColorListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
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
		pNMCD->clrText		= GetTextColor( iItem, iSubItem );
		pNMCD->clrTextBk	= GetBkColor( iItem, iSubItem );
		*pResult	= CDRF_NEWFONT;
		break;

	default:
		*pResult	= CDRF_DODEFAULT;
		break;
	}
}
