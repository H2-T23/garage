// DropTarget.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MFCOLEDragAndDrop.h"
#include "DropTarget.h"


// CDropTarget

IMPLEMENT_DYNAMIC(CDropTarget, COleDropTarget)

BEGIN_MESSAGE_MAP(CDropTarget, COleDropTarget)
END_MESSAGE_MAP()

// CDropTarget ���b�Z�[�W �n���h���[

DROPEFFECT	CDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	if( !pDataObject->IsDataAvailable( m_cfFormat ) ){
		return(DROPEFFECT_NONE);
	}

	return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);
}


void CDropTarget::OnDragLeave(CWnd* pWnd)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	COleDropTarget::OnDragLeave(pWnd);
}


DROPEFFECT	CDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	if( !pDataObject->IsDataAvailable( m_cfFormat ) ){
		return(DROPEFFECT_NONE);
	}

	return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);
}


DROPEFFECT CDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	return COleDropTarget::OnDragScroll(pWnd, dwKeyState, point);
}


BOOL CDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	if( pDataObject->IsDataAvailable( m_cfFormat ) ){
		return(DROPEFFECT_NONE);
	}

	return COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point);
}
