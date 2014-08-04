// DropTarget.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCOLEDragAndDrop.h"
#include "DropTarget.h"


// CDropTarget

IMPLEMENT_DYNAMIC(CDropTarget, COleDropTarget)

BEGIN_MESSAGE_MAP(CDropTarget, COleDropTarget)
END_MESSAGE_MAP()

// CDropTarget メッセージ ハンドラー

DROPEFFECT	CDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if( !pDataObject->IsDataAvailable( m_cfFormat ) ){
		return(DROPEFFECT_NONE);
	}

	return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);
}


void CDropTarget::OnDragLeave(CWnd* pWnd)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	COleDropTarget::OnDragLeave(pWnd);
}


DROPEFFECT	CDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if( !pDataObject->IsDataAvailable( m_cfFormat ) ){
		return(DROPEFFECT_NONE);
	}

	return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);
}


DROPEFFECT CDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	return COleDropTarget::OnDragScroll(pWnd, dwKeyState, point);
}


BOOL CDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if( pDataObject->IsDataAvailable( m_cfFormat ) ){
		return(DROPEFFECT_NONE);
	}

	return COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point);
}
