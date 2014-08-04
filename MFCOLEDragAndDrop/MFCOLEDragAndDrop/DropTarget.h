
#pragma once


// CDropTarget コマンド ターゲット

class CDropTarget : public COleDropTarget {
	DECLARE_DYNAMIC(CDropTarget)
public:
	CDropTarget()
		: m_cfFormat(CF_UNICODETEXT)
	{
	}

	virtual ~CDropTarget(){
	}

protected:
	CLIPFORMAT			m_cfFormat;

	DECLARE_MESSAGE_MAP()
public:
	virtual DROPEFFECT	OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void		OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT	OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT	OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
	virtual BOOL		OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};


